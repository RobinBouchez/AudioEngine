#include "SineWave.h"
#include "constants.h"

SineWave::SineWave(int freq, float ampl)
	: Wave{ freq, ampl }
{
	CalculateOffset();
}

float SineWave::Process() {
	auto sample = m_Amplitude * sin(m_Angle);
	m_Angle += m_Offset;
	return float(sample);
}

float SineWave::CalculateOffset()
{
	m_Offset = 2 * float(constants::pi) * m_Frequency / g_SampleRate;
	return m_Offset;
}
