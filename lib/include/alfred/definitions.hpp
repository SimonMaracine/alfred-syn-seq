#pragma once

#include <cstdint>

namespace def {
    inline constexpr int SAMPLE_FREQUENCY = 44100;
    inline constexpr int BITS_PER_SAMPLE = 16;

    // Sample type
    using Resolution = std::int16_t;
}
