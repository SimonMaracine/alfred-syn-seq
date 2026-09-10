#pragma once

#include <stdexcept>

namespace alfred::error {
    struct Error : std::runtime_error {
        using runtime_error::runtime_error;
    };
}
