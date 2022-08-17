#pragma once
#include "Wave.h"

class TriangleWave : public Wave
{
public:
	TriangleWave(int freq, float amp);
	float Process();
};

