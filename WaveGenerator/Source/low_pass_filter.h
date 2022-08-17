#pragma once
#include "filter.h"

class LowPassFilter : public Filter {
    public:
        LowPassFilter();
        virtual ~LowPassFilter();

        virtual double f(double sample);

        double fast_f(double sample);

        void setCutoffFrequency(double f);

        double m_dt;

    protected:
        double m_y;
        double m_rc;
};

