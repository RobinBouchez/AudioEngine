#include "TriangleWave.h"
#include "constants.h"

TriangleWave::TriangleWave(int freq, float amp) : Wave{ freq, amp }
{
	m_Offset = asin(sin(float(constants::pi) * m_Frequency / g_SampleRate));
}
float TriangleWave::Process() {
	//auto sample = (m_Amplitude / pow(float(M_PI), 2)) * m_Angle;
	auto sample = m_Amplitude * m_Angle;
	m_Angle += m_Offset;
	return sample;
}