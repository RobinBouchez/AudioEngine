#include "WavGenerator.h"
#include "Includes.h"
#include "Wave.h"
#include "SineWave.h"

WavGenerator::WavGenerator()
    : mAudioFile{}
    , gAmplitude{}
    , gFrequentie{}
    , gWave{}
{
}

WavGenerator::~WavGenerator()
{
}

void WavGenerator::Initialize()
{
    InputAudioData();
    gWave = new SineWave(gFrequentie, gAmplitude);
}

void WavGenerator::Run()
{
    MakeAudioFile();
}

void WavGenerator::Destroy()
{
    delete gWave;
    gWave = nullptr;
}


void WavGenerator::WriteToFile(std::ofstream& file, const int &value, const int &size)
{
    file.write(reinterpret_cast<const char*> (&value), size);
}

void WavGenerator::MakeAudioFile()
{
    mAudioFile.open("waveForm.wav", std::ios::binary); //Open Audio File
    
    //Header chunk
    mAudioFile << "RIFF"; //chunk ID
    mAudioFile << "size"; //chunk data size
    mAudioFile << "WAVE"; //RIFF type

    //Format chunk
    mAudioFile << "fmt "; //chunk ID
    WriteToFile(mAudioFile, 16, 4); //chunk data size
    WriteToFile(mAudioFile, 1, 2); //compression code
    WriteToFile(mAudioFile, 1, 2); //number of channels
    WriteToFile(mAudioFile, Wave::g_SampleRate, 4); //sample rate
    WriteToFile(mAudioFile, Wave::g_SampleRate * Wave::g_BitDepth / 8, 4); //Average bytes per second
    WriteToFile(mAudioFile, Wave::g_BitDepth / 8, 2); //Block align
    WriteToFile(mAudioFile, Wave::g_BitDepth, 2); //Significant bits per sample --> bit depth
    //WriteToFile(audioFile, ... ); //Extra format bytes

    //Data chunk
    mAudioFile << "data";
    mAudioFile << "size";

    int preAudioPosition = int(mAudioFile.tellp());

    //Audio Data
    ProcessAudioData();

    int postAudioPosition = int(mAudioFile.tellp());

    mAudioFile.seekp(long long int(preAudioPosition) - 4);
    WriteToFile(mAudioFile, postAudioPosition - preAudioPosition, 4); //write size to header chunk

    mAudioFile.seekp(4, std::ios::beg);
    WriteToFile(mAudioFile, postAudioPosition - 8, 4); //write size to data chunk

    mAudioFile.close();
}

void WavGenerator::InputAudioData()
{
    gFrequentie = 440;
    gAmplitude = 0.5;

    std::cout << "WaveType: " << std::endl;
    std::cout << "Frequency: " << gFrequentie << std::endl;
    std::cout << "Amplitude: " << gAmplitude << std::endl;
}
void WavGenerator::ProcessAudioData()
{
    auto maxAmplitude = pow(2, audioConstants::bitdepth - 1) - 1;
    for (int i = 0; i < audioConstants::sampleRate * Wave::g_Duration; i++)
    {
        auto sample = gWave->Process();

        int intSample = static_cast<int> (sample * maxAmplitude);
        WriteToFile(mAudioFile, intSample, 2);
    }
}
