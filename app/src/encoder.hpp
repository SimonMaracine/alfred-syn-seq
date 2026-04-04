#pragma once

#include "utility.hpp"
#include "error.hpp"

namespace encoder {
    // Encode a bunch of samples into a WAV file
    utility::Buffer encode_wav(std::size_t count, const double* samples);

    struct EncoderError : error::Error {
        using error::Error::Error;

        ALFRED_ERROR_NAME(EncoderError)
    };
}
