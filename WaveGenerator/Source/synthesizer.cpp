#include "synthesizer.h"

#include "utilities.h"
#include "delta.h"

#include <cassert>
#include <cmath>
#include <algorithm>

Synthesizer::Synthesizer() {
    m_inputChannels = nullptr;
    m_inputChannelCount = 0;
    m_inputBufferSize = 0;
    m_inputWriteOffset = 0.0;
    m_inputSamplesRead = 0;

    m_audioBufferSize = 0;

    m_inputSampleRate = 0.0;
    m_audioSampleRate = 0.0;

    m_lastInputSampleOffset = 0.0;

    m_run = true;
    m_thread = nullptr;
    m_filters = nullptr;
}

Synthesizer::~Synthesizer() {
    assert(m_inputChannels == nullptr);
    assert(m_thread == nullptr);
    assert(m_filters == nullptr);
}

void Synthesizer::initialize(const Parameters &p) {
    m_inputChannelCount = p.InputChannelCount;
    m_inputBufferSize = p.InputBufferSize;
    m_inputWriteOffset = p.InputBufferSize;
    m_audioBufferSize = p.AudioBufferSize;
    m_inputSampleRate = p.InputSampleRate;
    m_audioSampleRate = p.AudioSampleRate;
    m_audioParameters = p.InitialAudioParameters;

    m_inputWriteOffset = 0;
    m_processed = true;

    m_audioBuffer.initialize(p.AudioBufferSize);
    m_inputChannels = new InputChannel[p.InputChannelCount];
    for (int i = 0; i < p.InputChannelCount; ++i) {
        m_inputChannels[i].TransferBuffer = new double[p.InputBufferSize];
        m_inputChannels[i].Data.initialize(p.InputBufferSize);
    }

    m_filters = new ProcessingFilters[p.InputChannelCount];
    for (int i = 0; i < p.InputChannelCount; ++i) {
        m_filters[i].AirNoiseLowPass.setCutoffFrequency(
            m_audioParameters.AirNoiseFrequencyCutoff);
        m_filters[i].AirNoiseLowPass.m_dt = 1 / m_audioSampleRate;

        m_filters[i].SampleNoiseLowPass.setCutoffFrequency(
            m_audioParameters.InputSampleNoiseFrequencyCutoff);
        m_filters[i].SampleNoiseLowPass.m_dt = 1 / m_audioSampleRate;

        m_filters[i].Derivative.m_dt = 1 / m_audioSampleRate;

        m_filters[i].InputDcFilter.setCutoffFrequency(10.0);
        m_filters[i].InputDcFilter.m_dt = 1 / m_audioSampleRate;
    }

    // temp
    /*
    ysWindowsAudioWaveFile waveFile0;
    waveFile0.OpenFile("../assets/test_engine_14_eq_adjusted_16.wav"); // test_engine_14_eq_adjusted_16.wav
    waveFile0.InitializeInternalBuffer(waveFile0.GetSampleCount());
    waveFile0.FillBuffer(0);
    waveFile0.CloseFile();

    ysWindowsAudioWaveFile waveFile1;
    waveFile1.OpenFile("../assets/test_engine_15_eq_adjusted_16.wav"); // ../assets/test_engine_16.wav
    waveFile1.InitializeInternalBuffer(waveFile1.GetSampleCount());
    waveFile1.FillBuffer(0);
    waveFile1.CloseFile();

    const unsigned int sampleCount0 = std::min((unsigned int)10000, waveFile0.GetSampleCount());
    const unsigned int sampleCount1 = std::min((unsigned int)10000, waveFile1.GetSampleCount());

    if (sampleCount0 != 0 && sampleCount1 != 0) {
        if (p.InputChannelCount > 0) {
            m_filters[0].Convolution.initialize(sampleCount0);
            for (unsigned int i = 0; i < sampleCount0; ++i) {
                m_filters[0].Convolution.getImpulseResponse()[i] =
                    2 * 0.025 * ((int16_t *)waveFile0.GetBuffer())[i] / INT16_MAX;
            }
        }

        if (p.InputChannelCount > 1) {
            m_filters[1].Convolution.initialize(sampleCount1);
            for (unsigned int i = 0; i < sampleCount1; ++i) {
                m_filters[1].Convolution.getImpulseResponse()[i] =
                    2 * 0.025 * ((int16_t *)waveFile1.GetBuffer())[i] / INT16_MAX;
            }
        }
    }
    */

    m_levelingFilter.p_target = m_audioParameters.LevelerTarget;
    m_levelingFilter.p_maxLevel = m_audioParameters.LevelerMaxGain;
    m_levelingFilter.p_minLevel = m_audioParameters.LevelerMinGain;

    for (int i = 0; i < m_audioBufferSize; ++i) {
        m_audioBuffer.write(0);
    }
}

void Synthesizer::initializeImpulseResponse(
    const int16_t *impulseResponse,
    unsigned int samples,
    double volume,
    int index)
{
    const unsigned int sampleCount = std::min((unsigned int)10000, samples);
    m_filters[index].Convolution.initialize(sampleCount);
    for (unsigned int i = 0; i < sampleCount; ++i) {
        m_filters[index].Convolution.getImpulseResponse()[i] =
            volume * impulseResponse[i] / INT16_MAX;
    }
}

void Synthesizer::startAudioRenderingThread() {
    m_run = true;
    m_thread = new std::thread(&Synthesizer::audioRenderingThread, this);
}

void Synthesizer::endAudioRenderingThread() {
    m_run = false;
    endInputBlock();

    m_thread->join();
    delete m_thread;

    m_thread = nullptr;
}

void Synthesizer::destroy() {
    m_audioBuffer.destroy();

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_inputChannels[i].Data.destroy();
        m_filters[i].Convolution.destroy();
    }

    delete[] m_inputChannels;

    delete[] m_filters;

    m_inputChannels = nullptr;
    m_filters = nullptr;
}

int Synthesizer::readAudioOutput(int samples, int16_t *buffer) {
    std::lock_guard<std::mutex> lock(m_lock0);

    const int newDataLength = m_audioBuffer.size();
    if (newDataLength >= samples) {
        m_audioBuffer.readAndRemove(samples, buffer);
    }
    else {
        m_audioBuffer.readAndRemove(newDataLength, buffer);
        memset(
            buffer + newDataLength,
            0,
            sizeof(int16_t) * ((size_t)samples - newDataLength));
    }
    
    const int samplesConsumed = std::min(samples, newDataLength);

    return samplesConsumed;
}

void Synthesizer::waitProcessed() {
    {
        std::unique_lock<std::mutex> lk(m_lock0);
        m_cv0.wait(lk, [this] { return m_processed; });
    }
}

void Synthesizer::writeInput(const double *data) {
    m_inputWriteOffset += (double)m_audioSampleRate / m_inputSampleRate;
    if (m_inputWriteOffset >= (double)m_inputBufferSize) {
        m_inputWriteOffset -= (double)m_inputBufferSize;
    }

    for (int i = 0; i < m_inputChannelCount; ++i) {
        RingBuffer<double> &buffer = m_inputChannels[i].Data;
        const double lastInputSample = m_inputChannels[i].LastInputSample;
        const size_t baseIndex = buffer.writeIndex();
        const double distance =
            inputDistance(m_inputWriteOffset, m_lastInputSampleOffset);
        double s =
            inputDistance(double(baseIndex), m_lastInputSampleOffset);
        for (; s <= distance; s += 1.0) {
            if (s >= m_inputBufferSize) s -= m_inputBufferSize;

            const double f = s / distance;
            const double r = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
            const double r_filtered = m_filters[i].SampleNoiseLowPass.f(r);
            const double r_s = m_audioParameters.InputSampleNoise * r_filtered;
            const double s_aug = f + r_s;

            buffer.write(lastInputSample * (1 - s_aug) + data[i] * s_aug);
        }

        m_inputChannels[i].LastInputSample = data[i];
    }

    m_lastInputSampleOffset = m_inputWriteOffset;
}

void Synthesizer::endInputBlock() {
    std::unique_lock<std::mutex> lk(m_inputLock); 

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_inputChannels[i].Data.removeBeginning(m_inputSamplesRead);
    }

    m_inputSamplesRead = 0;
    m_latency = int(m_inputChannels[0].Data.size());
    
    m_processed = false;

    lk.unlock();
    m_cv0.notify_one();
}

void Synthesizer::audioRenderingThread() {
    while (m_run) {
        renderAudio();
    }
}

#undef max
void Synthesizer::renderAudio() {
    std::unique_lock<std::mutex> lk0(m_lock0);

    m_cv0.wait(lk0, [this] {
        const bool inputAvailable =
            m_inputChannels[0].Data.size() > 0
            && m_audioBuffer.size() < 2000;
        return !m_run || (inputAvailable && !m_processed);
    });

    const int n = std::min(
        std::max(0, 2000 - (int)m_audioBuffer.size()),
        (int)m_inputChannels[0].Data.size());

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_inputChannels[i].Data.read(n, m_inputChannels[i].TransferBuffer);
    }
    
    m_inputSamplesRead = n;
    m_processed = true;

    lk0.unlock();

    for (int i = 0; i < m_inputChannelCount; ++i) {
        m_filters[i].AirNoiseLowPass.setCutoffFrequency(
            m_audioParameters.AirNoiseFrequencyCutoff);
        m_filters[i].SampleNoiseLowPass.setCutoffFrequency(
            m_audioParameters.InputSampleNoiseFrequencyCutoff);
    }

    for (int i = 0; i < n; ++i) {
        m_audioBuffer.write(renderAudio(i));
    }

    m_cv0.notify_one();
}

double Synthesizer::getLatency() const {
    return (double)m_latency / m_audioSampleRate;
}

int Synthesizer::inputDelta(int s1, int s0) const {
    return (s1 < s0)
        ? m_inputBufferSize - s0 + s1
        : s1 - s0;
}

double Synthesizer::inputDistance(double s1, double s0) const {
    return (s1 < s0)
        ? (double)m_inputBufferSize - s0 + s1
        : s1 - s0;
}

void Synthesizer::setInputSampleRate(double sampleRate) {
    if (sampleRate != m_inputSampleRate) {
        std::lock_guard<std::mutex> lock(m_lock0);
        m_inputSampleRate = sampleRate;
    }
}

#undef max

int16_t Synthesizer::renderAudio(int inputSample) {
    const double airNoise = m_audioParameters.AirNoise;
    const double dF_F_mix = m_audioParameters.dF_F_mix;
    const double convAmount = m_audioParameters.Convolution;
    double signal = 0;

    double dc[2] = { 0.0, 0.0 };

    for (int i = 0; i < m_inputChannelCount; ++i) {
        const double f_in = m_inputChannels[i].TransferBuffer[inputSample];
        const double f_dc = m_filters[i].InputDcFilter.fast_f(f_in);
        const double f = f_in - f_dc;
        const double f_p = m_filters[i].Derivative.f(f);

        const double noise = 2.0 * ((double)rand() / RAND_MAX) - 1.0;
        const double r =
            m_filters->AirNoiseLowPass.fast_f(noise);
        const double r_mixed =
            airNoise * r + (1 - airNoise);

        const double v_in = f_p * dF_F_mix + f * r_mixed * (1 - dF_F_mix);
        const double v =
            convAmount * m_filters[i].Convolution.f(v_in) + (1 - convAmount) * v_in;

        signal += v;
    }

    m_levelingFilter.p_target = m_audioParameters.LevelerTarget;
    const double v_leveled = m_levelingFilter.f(signal) * m_audioParameters.Volume;
    int r_int = std::lround(v_leveled);
    if (r_int > INT16_MAX) {
        r_int = INT16_MAX;
    }
    else if (r_int < INT16_MIN) {
        r_int = INT16_MIN;
    }

    return (int16_t)r_int;
}

double Synthesizer::getLevelerGain() {
    std::lock_guard<std::mutex> lock(m_lock0);
    return m_levelingFilter.getAttenuation();
}

Synthesizer::AudioParameters Synthesizer::getAudioParameters() {
    std::lock_guard<std::mutex> lock(m_lock0);
    return m_audioParameters;
}

void Synthesizer::setAudioParameters(const AudioParameters &params) {
    std::lock_guard<std::mutex> lock(m_lock0);
    m_audioParameters = params;
}
