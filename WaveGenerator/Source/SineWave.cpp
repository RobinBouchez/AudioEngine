#include "SineWave.h"

#define M_PI 3.14159265359

SineWave::SineWave(int freq, float ampl) 
	: Wave{ freq, ampl }
{
	m_Offset = 2 * float(M_PI) * m_Frequency / g_SampleRate;
}
float SineWave::Process() {
	auto sample = m_Amplitude * sin(m_Angle);
	m_Angle += m_Offset;
	return sample;
}