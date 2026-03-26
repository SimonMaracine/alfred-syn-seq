#pragma once

#include <string>
#include <utility>
#include <variant>
#include <optional>
#include <vector>

#include "alfred/synthesis.hpp"

// Runtime dynamic instruments/presets

namespace preset {
    using EnvelopeDescription = std::variant<syn::envelope::DescriptionAdsr, syn::envelope::DescriptionAdr>;

    // Parameters that make up a runtime instrument
    struct BasePreset {
        std::string name;
        std::string description;
        syn::InstrumentRange range;
        EnvelopeDescription envelope_description;
        syn::envelope::Type envelope_type {};
    };

    // Template for any kind of runtime-defined instrument
    template<typename Preset>
    class BaseRuntimeInstrument : public syn::Instrument {
    public:
        BaseRuntimeInstrument(Preset preset, syn::InstrumentId id)
            : m_preset(std::move(preset)), m_id(id) {}

        // Retrieve a read only reference to the parameters that make up this instrument
        const Preset& preset() const { return m_preset; }

        const char* name() const override { return m_preset.name.c_str(); }
        const char* description() const override { return m_preset.description.c_str(); }
        syn::InstrumentId id() const override { return m_id; }
        syn::InstrumentRange range() const override { return m_preset.range; }
        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }
    protected:
        Preset m_preset;
        syn::InstrumentId m_id {};
        syn::VolumeA m_volume;
    };

    namespace add {
        // One harmonic/inharmonic
        struct Partial {
            syn::oscillator::Type oscillator_type {};
            double frequency_multiplier = 1.0;
            double amplitude_divisor = 1.0;
            double phase {};
            std::optional<syn::LowFrequencyOscillator> lfo;
        };

        struct Preset : BasePreset {
            std::vector<Partial> partials;
        };

        class RuntimeInstrument : public BaseRuntimeInstrument<Preset> {
        public:
            explicit RuntimeInstrument(Preset preset);

            double sound(double time, double time_on, syn::NoteId note) const noexcept override;
            syn::envelope::Ptr new_envelope() const override;
            double attack_duration() const override;
            double release_duration() const override;
            std::unique_ptr<Instrument> clone() const override { return std::make_unique<RuntimeInstrument>(*this); }
        private:
            std::vector<double> m_amplitudes;
        };
    }

    namespace pad {
        struct Preset : BasePreset {

        };

        class RuntimeInstrument : public BaseRuntimeInstrument<Preset> {
        public:
            explicit RuntimeInstrument(Preset preset);

            double sound(double time, double time_on, syn::NoteId note) const noexcept override;
            syn::envelope::Ptr new_envelope() const override;
            double attack_duration() const override;
            double release_duration() const override;
            std::unique_ptr<Instrument> clone() const override { return std::make_unique<RuntimeInstrument>(*this); }
        private:

        };
    }
}
