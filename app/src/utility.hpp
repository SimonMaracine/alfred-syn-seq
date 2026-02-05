#pragma once

#include <string>
#include <filesystem>
#include <stdexcept>

namespace utility {
    using Buffer = std::string;

    void read_file(const std::filesystem::path& path, Buffer& buffer);
    void write_file(const std::filesystem::path& path, const Buffer& buffer);

    std::filesystem::path data_file_path();

    struct FilerError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
