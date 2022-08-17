#pragma once
#include <iostream>
#include <fstream>

class Wave;

class WavGenerator final {
public:
	WavGenerator();
	~WavGenerator();

	void Initialize();
	void Run();
	void Destroy();


	void WriteToFile(std::ofstream& file, const int& value, const int& size);
	void MakeAudioFile();
	void InputAudioData();
	void ProcessAudioData();
	std::ofstream mAudioFile;

private:
	int gFrequentie;
	float gAmplitude;
	Wave* gWave;

};