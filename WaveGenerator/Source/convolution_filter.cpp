#include "convolution_filter.h"

#include <assert.h>
#include <string.h>

ConvolutionFilter::ConvolutionFilter() {
    m_shiftRegister = nullptr;
    m_impulseResponse = nullptr;

    m_shiftOffset = 0;
    m_sampleCount = 0;
}

ConvolutionFilter::~ConvolutionFilter() {
    assert(m_shiftRegister == nullptr);
    assert(m_impulseResponse == nullptr);
}

void ConvolutionFilter::initialize(int samples) {
    m_sampleCount = samples;
    m_shiftOffset = 0;
    m_shiftRegister = new double[samples];
    m_impulseResponse = new double[samples];

    memset(m_shiftRegister, 0, sizeof(double) * samples);
    memset(m_impulseResponse, 0, sizeof(double) * samples);
}

void ConvolutionFilter::destroy() {
    delete[] m_shiftRegister;
    delete[] m_impulseResponse;

    m_shiftRegister = nullptr;
    m_impulseResponse = nullptr;
}

double ConvolutionFilter::f(double sample) {
    m_shiftRegister[m_shiftOffset] = sample;

    double result = 0;
    for (int i = 0; i < m_sampleCount - m_shiftOffset; ++i) {
        result += m_impulseResponse[i] * m_shiftRegister[i + m_shiftOffset];
    }

    for (int i = m_sampleCount - m_shiftOffset; i < m_sampleCount; ++i) {
        result += m_impulseResponse[i] * m_shiftRegister[i - (m_sampleCount - m_shiftOffset)];
    }

    m_shiftOffset = (m_shiftOffset - 1 + m_sampleCount) % m_sampleCount;

    return result;
}
