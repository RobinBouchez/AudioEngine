#include "Includes.h"

#include "Wave.h"
#include "SineWave.h"

using namespace std;
//------------------------------
// Global Variables
//------------------------------
int gFrequentie;
float gAmplitude;
Wave* gWave;

ofstream audioFile;

//------------------------------
// Fuctions Defines
//------------------------------
void Init();
void Process();
void End();


void WriteToFile(ofstream& file, int value, int size);
void MakeAudioFile();
void InputAudioData();
void ProcessAudioData();

//-----------------------------

int main()
{
    Init(); // INITIALIZE

    Process(); // PROCESING

    End(); // CLEANUP 
}

//------------------------------
// Function Declarations
//------------------------------
void Init()
{
    InputAudioData();
    gWave = new SineWave(gFrequentie, gAmplitude);
}

void Process()
{
    MakeAudioFile();
}

void End()
{
    delete gWave;
    gWave = nullptr;
}


void WriteToFile(ofstream& file, int value, int size)
{
    file.write(reinterpret_cast<const char*> (&value), size);
}

void MakeAudioFile()
{
    audioFile.open("waveForm.wav", ios::binary); //Open Audio File
    
    //Header chunk
    audioFile << "RIFF"; //chunk ID
    audioFile << "size"; //chunk data size
    audioFile << "WAVE"; //RIFF type

    //Format chunk
    audioFile << "fmt "; //chunk ID
    WriteToFile(audioFile, 16, 4); //chunk data size
    WriteToFile(audioFile, 1, 2); //compression code
    WriteToFile(audioFile, 1, 2); //number of channels
    WriteToFile(audioFile, Wave::g_SampleRate, 4); //sample rate
    WriteToFile(audioFile, Wave::g_SampleRate * Wave::g_BitDepth / 8, 4); //Average bytes per second
    WriteToFile(audioFile, Wave::g_BitDepth / 8, 2); //Block align
    WriteToFile(audioFile, Wave::g_BitDepth, 2); //Significant bits per sample --> bit depth
    //WriteToFile(audioFile, ... ); //Extra format bytes

    //Data chunk
    audioFile << "data";
    audioFile << "size";

    int preAudioPosition = int(audioFile.tellp());

    //Audio Data
    ProcessAudioData();

    int postAudioPosition = int(audioFile.tellp());

    audioFile.seekp(long long int(preAudioPosition) - 4);
    WriteToFile(audioFile, postAudioPosition - preAudioPosition, 4); //write size to header chunk

    audioFile.seekp(4, ios::beg);
    WriteToFile(audioFile, postAudioPosition - 8, 4); //write size to data chunk

    audioFile.close();
}

void InputAudioData()
{
    gFrequentie = 440;
    gAmplitude = 0.5;

    cout << "WaveType: " << endl;
    cout << "Frequency: " << gFrequentie << endl;
    cout << "Amplitude: " << gAmplitude << endl;
}
void ProcessAudioData() 
{
    auto maxAmplitude = pow(2, Wave::g_BitDepth - 1) - 1;
    for (int i = 0; i < Wave::g_SampleRate * Wave::g_Duration; i++)
    {
        auto sample = gWave->Process();

        int intSample = static_cast<int> (sample * maxAmplitude);
        WriteToFile(audioFile, intSample, 2);
    }
}

