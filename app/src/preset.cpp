#include "preset.hpp"

#include <sstream>
#include <format>

#include <cereal/archives/xml.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/utility.hpp>

namespace preset {
    void export_preset(const add::Preset& preset, utility::Buffer& buffer) {
        std::ostringstream stream;
        stream.exceptions(std::ios_base::failbit);

        try {
            cereal::XMLOutputArchive archive {stream};
            archive(cereal::make_nvp("preset", preset));
        } catch (const cereal::Exception& e) {
            throw PresetError(std::format("Could not write to stream: {}", e.what()));
        } catch (...) {
            throw PresetError("Unexpected error writing to stream");
        }

        buffer = stream.str();
    }

    void import_preset(add::Preset& preset, const utility::Buffer& buffer) {
        std::istringstream stream {buffer};
        stream.exceptions(std::ios_base::failbit);

        try {
            cereal::XMLInputArchive archive {stream};
            archive(cereal::make_nvp("preset", preset));
        } catch (const cereal::Exception& e) {
            throw PresetError(std::format("Could not read from stream: {}", e.what()));
        } catch (...) {
            throw PresetError("Unexpected error reading from stream");
        }
    }
}
