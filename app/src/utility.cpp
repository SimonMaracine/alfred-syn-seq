#include "utility.hpp"

#include <fstream>
#include <memory>

#include <SDL3/SDL.h>

namespace alfred::utility {
    void read_file(const std::filesystem::path& path, Buffer& buffer) {
        std::ifstream stream {path, std::ios_base::binary};

        if (!stream.is_open()) {
            throw FileError("Could not open file");
        }

        stream.seekg(0, stream.end);
        const auto size = stream.tellg();
        stream.seekg(0, stream.beg);

        buffer.resize(std::size_t(size));
        stream.read(buffer.data(), size);

        if (stream.fail()) {
            throw FileError("Could not read from file");
        }
    }

    void write_file(const std::filesystem::path& path, const Buffer& buffer) {
        std::ofstream stream {path, std::ios_base::binary};

        if (!stream.is_open()) {
            throw FileError("Could not open file");
        }

        stream.write(buffer.data(), std::streamsize(buffer.size()));

        if (stream.fail()) {
            throw FileError("Could not write to file");
        }
    }

    std::filesystem::path data_file_path([[maybe_unused]] const char* organization, [[maybe_unused]] const char* application) {
#ifdef ALFRED_DISTRIBUTION
        static const auto file_path =
            std::unique_ptr<char, decltype(&SDL_free)>(
                SDL_GetPrefPath(organization, application),
                SDL_free
            );

        return file_path.get();
#else
        return "./";  // Relative directory
#endif
    }

    void set_property(const char* property, const char* value) {
        (void) SDL_SetAppMetadataProperty(property, value);
    }

    const char* get_property(const char* property) {
        const char* value = SDL_GetAppMetadataProperty(property);

        if (!value) {
            return "";
        }

        return value;
    }

    void show_error_message_box(const char* title, const char* message) {
        (void) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
    }
}
