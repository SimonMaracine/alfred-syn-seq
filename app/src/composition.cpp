#include "composition.hpp"

#include <sstream>
#include <format>

#include <cereal/archives/binary.hpp>

#include "error.hpp"

namespace alfred::composition {
    void export_composition(const Composition& composition, utility::Buffer& buffer) {
        std::ostringstream stream {std::ios_base::binary};
        stream.exceptions(std::ios_base::failbit);

        try {
            cereal::BinaryOutputArchive archive {stream};
            archive(composition);
        } catch (const cereal::Exception& e) {
            throw error::Error(std::format("Could not write to stream: {}", e.what()));
        } catch (...) {
            throw error::Error("Unexpected error writing to stream");
        }

        buffer.data = stream.str();
    }

    void import_composition(Composition& composition, const utility::Buffer& buffer) {
        std::istringstream stream {buffer.data, std::ios_base::binary};
        stream.exceptions(std::ios_base::failbit);

        try {
            cereal::BinaryInputArchive archive {stream};
            archive(composition);
        } catch (const cereal::Exception& e) {
            throw error::Error(std::format("Could not read from stream: {}", e.what()));
        } catch (...) {
            throw error::Error("Unexpected error reading from stream");
        }
    }
}
