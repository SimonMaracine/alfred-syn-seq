#pragma once

#include "utility.hpp"
#include "error.hpp"

namespace alfred::encoder {
    // Encode an array of samples into a WAV file buffer
    utility::Buffer encode_wav(std::size_t count, const double* samples);

    struct EncoderError : error::Error {
        using Error::Error;

        ALFRED_ERROR_NAME(EncoderError)
    };
}
