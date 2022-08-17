#include "convolution_filter.h"
#include "leveling_filter.h"
#include "derivative_filter.h"
#include "low_pass_filter.h"
#pragma once
#include "ring_buffer.h"
#include "audioDataConstants.h"

#include <cinttypes>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class Synthesizer {
    public:
        struct AudioParameters {
            double Volume = 1.0;
            double Convolution = 1.0;
            double dF_F_mix = 0.01; //0.00990099
            double InputSampleNoise = 0.5; //0.125
            double InputSampleNoiseFrequencyCutoff = 80000.0;
            double AirNoise = 1.0;
            double AirNoiseFrequencyCutoff = 10000.0;
            double LevelerTarget = 30000;
            double LevelerMaxGain = 1.9;
            double LevelerMinGain = 0.00001;
        };

        struct Parameters {
            int InputChannelCount = 1;
            int InputBufferSize = 1024;
            int AudioBufferSize = int(audioConstants::sampleRate);
            double InputSampleRate = 10000;
            double AudioSampleRate = audioConstants::sampleRate;
            AudioParameters InitialAudioParameters;
        };

        struct InputChannel {
            RingBuffer<double> Data;
            double *TransferBuffer = nullptr;

            double LastInputSample = 0.0;
        };

        struct ProcessingFilters {
            ConvolutionFilter Convolution;
            DerivativeFilter Derivative;
            LowPassFilter AirNoiseLowPass;
            LowPassFilter SampleNoiseLowPass;
            LowPassFilter InputDcFilter;
        };

    public:
        Synthesizer();
        ~Synthesizer();

        void initialize(const Parameters &p);
        void initializeImpulseResponse(
            const int16_t *impulseResponse,
            unsigned int samples,
            double volume,
            int index);
        void startAudioRenderingThread();
        void endAudioRenderingThread();
        void destroy();

        int readAudioOutput(int samples, int16_t *buffer);

        void writeInput(const double *data);
        void endInputBlock();

        void waitProcessed();

        void audioRenderingThread();
        void renderAudio();

        double getLatency() const;

        int inputDelta(int s1, int s0) const;
        double inputDistance(double s1, double s0) const;

        void setInputSampleRate(double sampleRate);
        double getInputSampleRate() const { return m_inputSampleRate; }

        int16_t renderAudio(int inputOffset);

        double getLevelerGain();
        AudioParameters getAudioParameters();
        void setAudioParameters(const AudioParameters &params);

    //protected:
        LevelingFilter m_levelingFilter;
        InputChannel *m_inputChannels;
        AudioParameters m_audioParameters;
        int m_inputChannelCount;
        int m_inputBufferSize;
        int m_inputSamplesRead;
        int m_latency;
        double m_inputWriteOffset;
        double m_lastInputSampleOffset;

        RingBuffer<int16_t> m_audioBuffer;
        int m_audioBufferSize;

        double m_inputSampleRate;
        double m_audioSampleRate;

        std::thread *m_thread;
        std::atomic<bool> m_run;
        bool m_processed;

        std::mutex m_inputLock;
        std::mutex m_lock0;
        std::condition_variable m_cv0;

        ProcessingFilters *m_filters;
};
