#include "filter.h"

class DerivativeFilter : public Filter {
    public:
        DerivativeFilter();
        virtual ~DerivativeFilter();

        virtual double f(double sample);

        double m_dt;

    protected:
        double m_previous;
};