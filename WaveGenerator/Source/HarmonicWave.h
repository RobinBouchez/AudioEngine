#pragma once
#include "Wave.h"

class HarmonicWave final : public Wave
{
public:
	HarmonicWave() = default;
	HarmonicWave(int freq, float amp);
	virtual ~HarmonicWave() = default;

public:
	virtual float Process() override;

private:
	float m_ModWave{};
};

