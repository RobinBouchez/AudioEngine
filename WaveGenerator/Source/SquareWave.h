#pragma once
#include "Wave.h"

class SquareWave :public Wave
{
public:
	SquareWave(int freq, float amp);
	float Process();
};

