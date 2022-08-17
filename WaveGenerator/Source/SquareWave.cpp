#include "SquareWave.h"
#define _USE_MATH_DEFINES
#include <cmath>

SquareWave::SquareWave(int freq, float amp)
	: Wave{ freq, amp }
{
	//m_Offset = ;
}

float SquareWave::Process()
{
	auto sample = m_Amplitude * (m_Angle);
	m_Angle += m_Offset;
	return sample;
}
