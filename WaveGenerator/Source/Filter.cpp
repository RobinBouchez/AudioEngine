#include "Filter.h"

Filter::Filter()
	:m_CutoffFrequency{}
	, m_Frequency{}
	, m_Resonance{}
	, m_Rolloff{}
{
}

Filter::~Filter()
{
}

double Filter::f(double sample)
{
	return sample;
}

void Filter::destroy()
{
}
