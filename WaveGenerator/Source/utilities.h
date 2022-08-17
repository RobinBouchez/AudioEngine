#ifndef ATG_ENGINE_SIM_UTILITIES_H
#define ATG_ENGINE_SIM_UTILITIES_H

double modularDistance(double a, double b, double mod = 1.0);
double positiveMod(double x, double mod);
double erfApproximation(double x);
inline double clamp(double x, double x0 = 0.0, double x1 = 1.0) {
    if (x <= x0) return x0;
    else if (x >= x1) return x1;
    else return x;
}

#endif /* ATG_ENGINE_SIM_UTILITIES_H */
