#pragma once

#include <string>
#include <utility>
#include <variant>
#include <optional>
#include <vector>

#include "alfred/synthesis.hpp"
#include "alfred/hash.hpp"

// Runtime dynamic instruments/presets

namespace preset {
    // One harmonic/inharmonic
    struct Partial {
        syn::oscillator::Type oscillator_type {};
        double frequency_multiplier = 1.0;
        double amplitude_divisor = 1.0;
        double phase {};
        std::optional<syn::LowFrequencyOscillator> lfo;
    };

    using EnvelopeDescription = std::variant<syn::envelope::DescriptionAdsr, syn::envelope::DescriptionAdr>;

    // Parameters that make up a runtime instrument
    struct Preset {
        std::string name;
        std::string description;
        syn::InstrumentRange range;
        EnvelopeDescription envelope_description;
        syn::envelope::Type envelope_type {};
        std::vector<Partial> partials;
    };

    // Template for any kind of runtime-defined instrument
    class RuntimeInstrument : public syn::Instrument {
    public:
        explicit RuntimeInstrument(Preset preset)
            : m_preset(std::move(preset)), m_id(hash::HashedStr32(preset.name))
        {
            for (const Partial& partial : m_preset.partials) {
                m_amplitudes.push_back(partial.amplitude_divisor);
            }

            m_amplitudes = syn::util::amplitudes(std::move(m_amplitudes));
        }

        // Retrieve a read only reference to the parameters that make up this instrument
        const Preset& preset() const { return m_preset; }

        const char* name() const override { return m_preset.name.c_str(); }
        syn::InstrumentId id() const override { return m_id; }
        const char* description() const override { return m_preset.description.c_str(); }

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return m_preset.range; }

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::envelope::Ptr new_envelope() const override;

        double attack_duration() const override;
        double release_duration() const override;

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<RuntimeInstrument>(*this); }
    private:
        Preset m_preset;
        syn::InstrumentId m_id {};
        syn::Volume m_volume {};
        std::vector<double> m_amplitudes;
    };
}
