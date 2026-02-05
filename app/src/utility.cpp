#include "utility.hpp"

#include <fstream>

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
    #elifdef ALFRED_WINDOWS
        // TODO
    #endif
#else
        return "./";  // Relative directory
#endif
    }
}
