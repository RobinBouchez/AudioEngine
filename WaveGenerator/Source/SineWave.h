#pragma once
#include "Wave.h"

class SineWave final : public Wave
{
public:
	SineWave(int freq, float amp);
	virtual ~SineWave() = default;
	
	virtual float Process();
};