#pragma once
class Filter
{
public:
	enum class Type { BANDPASS, HIGHPASS, LOWPASS, };

public:
	Filter();
	virtual ~Filter();

	virtual double f(double sample);
	virtual void destroy();

private:
	float m_Frequency;
	float m_Resonance;
	float m_CutoffFrequency;
	float m_Rolloff;
};
