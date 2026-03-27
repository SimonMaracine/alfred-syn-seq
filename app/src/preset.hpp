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
    void save(Archive& archive, const BasePreset& self, const std::uint32_t) {
        archive(
            cereal::make_nvp("name", self.name),
            cereal::make_nvp("description", self.description),
            cereal::make_nvp("range", self.range),
            cereal::make_nvp("envelope_description", self.envelope_description),
            cereal::make_nvp("envelope_type", self.envelope_type)
        );
    }

    template<typename Archive>
    void load(Archive& archive, BasePreset& self, const std::uint32_t) {
        archive(
            cereal::make_nvp("name", self.name),
            cereal::make_nvp("description", self.description),
            cereal::make_nvp("range", self.range),
            cereal::make_nvp("envelope_description", self.envelope_description),
            cereal::make_nvp("envelope_type", self.envelope_type)
        );
    }

    namespace add {
        template<typename Archive>
        void save(Archive& archive, const Partial& self, const std::uint32_t) {
            archive(
                cereal::make_nvp("oscillator_type", self.oscillator_type),
                cereal::make_nvp("frequency_multiplier", self.frequency_multiplier),
                cereal::make_nvp("amplitude_divisor", self.amplitude_divisor),
                cereal::make_nvp("phase", self.phase),
                cereal::make_nvp("lfo", self.lfo)
            );
        }

        template<typename Archive>
        void load(Archive& archive, Partial& self, const std::uint32_t) {
            archive(
                cereal::make_nvp("oscillator_type", self.oscillator_type),
                cereal::make_nvp("frequency_multiplier", self.frequency_multiplier),
                cereal::make_nvp("amplitude_divisor", self.amplitude_divisor),
                cereal::make_nvp("phase", self.phase),
                cereal::make_nvp("lfo", self.lfo)
            );
        }

        template<typename Archive>
        void save(Archive& archive, const Preset& self, const std::uint32_t) {
            archive(cereal::make_nvp("partials", self.partials));
        }

        template<typename Archive>
        void load(Archive& archive, Preset& self, const std::uint32_t) {
            archive(cereal::make_nvp("partials", self.partials));
        }
    }

    namespace pad {
        template<typename Archive>
        void save(Archive& archive, const Preset& self, const std::uint32_t) {

        }

        template<typename Archive>
        void load(Archive& archive, Preset& self, const std::uint32_t) {

        }
    }
}

namespace preset {
    void export_preset(const add::Preset& preset, utility::Buffer& buffer);
    void import_preset(add::Preset& preset, const utility::Buffer& buffer);
    void export_preset(const pad::Preset& preset, utility::Buffer& buffer);
    void import_preset(pad::Preset& preset, const utility::Buffer& buffer);

    struct PresetError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

CEREAL_CLASS_VERSION(preset::add::Preset, 1)
