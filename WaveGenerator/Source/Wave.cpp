#include "Wave.h"
#include <stdlib.h>


Wave::Wave(int freq, float ampl)
	: m_Frequency{freq}
	, m_Amplitude{ampl}
	, m_Angle{}
	, m_Offset{}
{
}

/*
float Wave::Process()
{
	enum Type { SINE, COS, HARMONICSINE };;
	Type mType;
	switch (mType)
	{
	case Type::SINE:
	{
		auto sample = m_Amplitude * sin(m_Angle);
		m_Angle += (2 * float(PI) * m_Frequency / g_SampleRate);
		return sample;
	}
		break;
	case Type::COS:
	{
		float DELTA = float (m_Frequency) / g_SampleRate;
		auto sample = m_Amplitude * cos(TWOPI * phase);
		phase = phase + DELTA;
		if (phase > 0.99999)
		{
			phase = 0.f;
		}
		return sample;
	}
		break;
	case Type::HARMONICSINE:
	{
		const float DELTA = float(m_Frequency) / g_SampleRate;
		const float DELTA2 = float(m_Frequency * 10) / g_SampleRate;

		auto sample = m_Amplitude * (cos(TWOPI * phase) + cos(TWOPI * phase2));
		phase = phase + DELTA;
		phase2 = phase2 + DELTA2;

		if (phase >= 1)
		{
			phase = 0.f;
		}
		if (phase2 >= 1)
		{
			phase2 = 0.f;
		}
		return sample;
	}
		break;
	case Type::TAN:
	{
		auto sample = m_Amplitude * tan(m_Angle);
		m_Angle += (2 * float(PI) * m_Frequency / g_SampleRate);
		return sample;
	}
		break;
	case Type::NOISE:
	{
		int seed = 4294967295;

		srand(seed);

		random_number = rand() % 2;
		normalised_rand = (random_number / (4294967295 / 2)) - 1.0f;

		auto sample = m_Amplitude * normalised_rand;
		return sample;
	}
		break;

	case Type::ENV:
	{
		auto sample = env;
		if (time < attacktime)
		{
			env += attackslope;
		}
		else if(time < attacktime + decaytime)
		{
			env -= decayslope;
		}
		return sample;
	}
		break;
	}
}*/

//----------------------------------------------------------------
// PRIVATE FUNTIONS
//----------------------------------------------------------------
float Wave::GetPhaseVelocity(float frequency, float wavelenght)
{
	return frequency *= wavelenght;
}

