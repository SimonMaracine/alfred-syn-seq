#pragma once

#include <string>
#include <filesystem>

#include "error.hpp"

namespace alfred::utility {
    using Buffer = std::string;

    void read_file(const std::filesystem::path& path, Buffer& buffer);
    void write_file(const std::filesystem::path& path, const Buffer& buffer);

    std::filesystem::path data_file_path(const char* organization, const char* application);

    void set_property(const char* property, const char* value);
    const char* get_property(const char* property);
    void show_error_message_box(const char* title, const char* message);

    struct FileError : error::Error {
        using Error::Error;

        ALFRED_ERROR_NAME(FileError)
    };
}
