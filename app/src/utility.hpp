#pragma once

#include <string>
#include <filesystem>
#include <stdexcept>

namespace utility {
    void read_file(const std::filesystem::path& path, std::string& buffer);
    void write_file(const std::filesystem::path& path, const std::string& buffer);

    struct FilerError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
