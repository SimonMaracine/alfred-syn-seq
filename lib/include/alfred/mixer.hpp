#pragma once

#include <algorithm>
#include <cmath>

namespace mixer {
    // In decibels
    using Volume = int;

    inline constexpr Volume MIN {-40};
    inline constexpr Volume DEFAULT {};
    inline constexpr Volume MAX {12};

    constexpr Volume clamp(Volume volume) {
        return std::clamp(volume, MIN, MAX);
    }

    inline double amplitude(Volume volume) {
        return std::pow(10.0, double(volume) / 20.0);
    }
}
