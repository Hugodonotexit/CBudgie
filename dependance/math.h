#pragma once
#include <cmath>
class budgieMath {
public:
    static inline long double round(long double value, int decimalPlaces) {
        long double factor = std::pow(10.0, decimalPlaces);
        return std::round(value * factor) / factor;
    }

    static inline double floor(long double value, int decimalPlaces) {
        long double factor = std::pow(10.0, decimalPlaces);
        return std::floor(value * factor) / factor;
    }

    static inline long double ceil(long double value, int decimalPlaces) {
        long double factor = std::pow(10.0, decimalPlaces);
        return std::ceil(value * factor) / factor;
    }

    static inline long double abs(long double value) {
        return std::fabs(value);
    }

};