#pragma once
class Filter
{
public:
	enum class Type { BANDPASS, HIGHPASS, LOWPASS, };


private:
	float m_Frequency;
	float m_Resonance;
	float m_CutoffFrequency;
	float m_Rolloff;
};

