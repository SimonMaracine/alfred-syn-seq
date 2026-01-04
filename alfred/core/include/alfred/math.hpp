#pragma once

#include <numbers>

inline constexpr double PI {std::numbers::pi};

constexpr double w(double hertz) {
    return hertz * 2.0 * std::numbers::pi;
}

consteval double operator""_hz(long double hertz) {
    return w(double(hertz));
}

constexpr bool less_than_eps(double x, double epsilon = 0.00001) {
    if (x <= epsilon) {
        return true;
    } else {
        return false;
    }
}

constexpr double zero_if_less_than_eps(double x, double epsilon = 0.00001) {
    if (less_than_eps(x, epsilon)) {
        return 0.0;
    } else {
        return x;
    }
}
