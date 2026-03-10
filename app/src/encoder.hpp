#pragma once

#include <stdexcept>

#include "utility.hpp"

namespace encoder {
    // Encode a bunch of samples into a WAV file
    utility::Buffer encode_wav(std::size_t count, const double* samples);

    struct EncoderError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
