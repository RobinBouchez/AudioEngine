#pragma once
#include <string>

class Wave
{
public:
	Wave() = default;
	Wave(int freq, float ampl);
	virtual ~Wave() = default;

public:

	virtual float Process() = 0;

	static const int g_SampleRate = 48'000;
	static const int g_BitDepth = 16;
	static const int g_Duration = 6;

	std::string sType;

protected:
	int m_Frequency;
	float m_Amplitude;
	float m_Angle;
	float m_Offset;

	const float PI = 3.141592653f;
	const float TWOPI = 2 * PI;
	float phase = 0;
	float phase2 = 0;

	int random_number{};
	float normalised_rand{};

private:
	float GetPhaseVelocity(float frequency, float wavelenght);

	float env = 0;
	int time = 0;
	float attacktime = 256;
	float decaytime = 768;
	float attackslope = 1 / attacktime;
	float decayslope = 1 / decaytime;
};

