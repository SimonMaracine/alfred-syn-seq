#pragma once

#include <numbers>

inline constexpr double PI {std::numbers::pi};

constexpr double w(double hertz) {
    return hertz * 2.0 * std::numbers::pi;
}

consteval double operator""_hz(long double hertz) {
    return w(double(hertz));
}
