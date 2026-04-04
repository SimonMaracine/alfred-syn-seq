#pragma once

#include <stdexcept>

namespace error {
    struct Error : std::runtime_error {
        using std::runtime_error::runtime_error;

        virtual const char* name() const = 0;
    };
}

#define ALFRED_ERROR_NAME(TYPE) \
    const char* name() const override { return #TYPE; }
