#include "HarmonicWave.h"
#define USE_MATH_DEFINES
#include <cmath>

HarmonicWave::HarmonicWave(int freq, float amp)
	: Wave{ freq, amp }
{
	int x = m_Frequency / g_SampleRate;
	m_Offset = float(0 + sin(x) + (m_ModWave * (cos(x)))) / 5;// +(2 * sin(2 * x) + (m_ModWave * cos(2 * x))))) / 5.0f;
}

float HarmonicWave::Process()
{
	auto sample = m_Amplitude * m_Angle;
	m_Angle += m_Offset;
	return sample;
}
