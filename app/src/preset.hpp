#pragma once

#include <string>
#include <variant>
#include <vector>
#include <stdexcept>

#include <cereal/cereal.hpp>
#include <alfred/preset.hpp>

#include "utility.hpp"

namespace syn {
    template<typename Archive>
    void serialize(Archive& archive, LowFrequencyOscillator& self, const std::uint32_t) {
        archive(
            cereal::make_nvp("frequency", self.frequency),
            cereal::make_nvp("deviation", self.deviation)
        );
    }

    namespace envelope {
        template<typename Archive>
        void serialize(Archive& archive, DescriptionAdsr& self, const std::uint32_t) {
            archive(
                cereal::make_nvp("duration_attack", self.duration_attack),
                cereal::make_nvp("duration_decay", self.duration_decay),
                cereal::make_nvp("duration_release", self.duration_release),
                cereal::make_nvp("value_sustain", self.value_sustain)
            );
        }

        template<typename Archive>
        void serialize(Archive& archive, DescriptionAdr& self, const std::uint32_t) {
            archive(
                cereal::make_nvp("duration_attack", self.duration_attack),
                cereal::make_nvp("duration_decay", self.duration_decay),
                cereal::make_nvp("duration_release", self.duration_release)
            );
        }
    }
}

namespace preset {
    template<typename Archive>
    void save(Archive& archive, const Partial& self, const std::uint32_t) {
        archive(
            cereal::make_nvp("type", self.type),
            cereal::make_nvp("frequency_multiplier", self.frequency_multiplier),
            cereal::make_nvp("amplitude_divisor", self.amplitude_divisor),
            cereal::make_nvp("phase", self.phase),
            cereal::make_nvp("lfo", self.lfo)
        );
    }

    template<typename Archive>
    void load(Archive& archive, Partial& self, const std::uint32_t) {
        archive(
            cereal::make_nvp("type", self.type),
            cereal::make_nvp("frequency_multiplier", self.frequency_multiplier),
            cereal::make_nvp("amplitude_divisor", self.amplitude_divisor),
            cereal::make_nvp("phase", self.phase),
            cereal::make_nvp("lfo", self.lfo)
        );
    }
}

namespace preset {
    struct Preset {
        std::string name;
        std::string description;
        syn::InstrumentRange range;
        std::variant<syn::envelope::DescriptionAdsr, syn::envelope::DescriptionAdr> envelope_description;
        syn::envelope::Type envelope_type {};
        std::vector<Partial> partials;

        template<typename Archive>
        void save(Archive& archive, const std::uint32_t) const {
            archive(
                cereal::make_nvp("name", name),
                cereal::make_nvp("description", description),
                cereal::make_nvp("range", range),
                cereal::make_nvp("envelope_description", envelope_description),
                cereal::make_nvp("envelope_type", envelope_type),
                cereal::make_nvp("partials", partials)
            );
        }

        template<typename Archive>
        void load(Archive& archive, const std::uint32_t) {
            archive(
                cereal::make_nvp("name", name),
                cereal::make_nvp("description", description),
                cereal::make_nvp("range", range),
                cereal::make_nvp("envelope_description", envelope_description),
                cereal::make_nvp("envelope_type", envelope_type),
                cereal::make_nvp("partials", partials)
            );
        }
    };

    void export_preset(const Preset& preset, utility::Buffer& buffer);
    void import_preset(Preset& preset, const utility::Buffer& buffer);

    struct PresetError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

CEREAL_CLASS_VERSION(preset::Preset, 1)
