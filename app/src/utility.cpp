#include "utility.hpp"

#include <fstream>

#include <SDL3/SDL.h>

namespace utility {
    void read_file(const std::filesystem::path& path, Buffer& buffer) {
        std::ifstream stream {path, std::ios_base::binary};

        if (!stream.is_open()) {
            throw FilerError("Could not open file");
        }

        stream.seekg(0, stream.end);
        const auto size {stream.tellg()};
        stream.seekg(0, stream.beg);

        buffer.resize(std::size_t(size));
        stream.read(buffer.data(), size);

        if (stream.fail()) {
            throw FilerError("Could not read from file");
        }
    }

    void write_file(const std::filesystem::path& path, const Buffer& buffer) {
        std::ofstream stream {path, std::ios_base::binary};

        if (!stream.is_open()) {
            throw FilerError("Could not open file");
        }

        stream.write(buffer.data(), std::streamsize(buffer.size()));

        if (stream.fail()) {
            throw FilerError("Could not write to file");
        }
    }

    std::filesystem::path data_file_path() {
#ifdef ALFRED_DISTRIBUTION
    #ifdef ALFRED_LINUX
        // TODO
        return "./";
    #elifdef ALFRED_WINDOWS
        // TODO
        return "./";
    #endif
#else
        return "./";  // Relative directory
#endif
    }

    void set_property(const char* property, const char* value) {
        (void) SDL_SetAppMetadataProperty(property, value);
    }

    const char* get_property(const char* property) {
        const char* value {SDL_GetAppMetadataProperty(property)};

        if (!value) {
            return "";
        }

        return value;
    }

    void show_error_message_box(const char* title, const char* message) {
        (void) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
    }
}
