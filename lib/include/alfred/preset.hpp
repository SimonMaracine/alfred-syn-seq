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
    struct Partial {
        syn::oscillator::Type oscillator_type {};
        double frequency_multiplier = 1.0;
        double amplitude_divisor = 1.0;
        double phase {};
        std::optional<syn::LowFrequencyOscillator> lfo;
    };

    using EnvelopeDescription = std::variant<syn::envelope::DescriptionAdsr, syn::envelope::DescriptionAdr>;

    // Template for any kind of runtime-defined instrument
    class RuntimeInstrument : public syn::Instrument {
    public:
        template<typename EnvelopeDescription>
        RuntimeInstrument(
            std::string name,
            std::string description,
            syn::InstrumentRange range,
            EnvelopeDescription envelope_description,
            syn::envelope::Type envelope_type,
            std::vector<Partial> partials
        ) : RuntimeInstrument(std::move(name), std::move(description), std::move(range), std::move(envelope_description), envelope_type, std::move(partials)) {}

        RuntimeInstrument(
            std::string name,
            std::string description,
            syn::InstrumentRange range,
            EnvelopeDescription envelope_description,
            syn::envelope::Type envelope_type,
            std::vector<Partial> partials
        ) :
            m_name(std::move(name)),
            m_description(std::move(description)),
            m_range(std::move(range)),
            m_id(hash::HashedStr32(m_name)),
            m_envelope_description(std::move(envelope_description)),
            m_envelope_type(envelope_type),
            m_partials(std::move(partials))
        {
            for (const Partial& partial : m_partials) {
                m_amplitudes.push_back(partial.amplitude_divisor);
            }

            m_amplitudes = syn::util::amplitudes(m_amplitudes);
        }

        const char* name() const override { return m_name.c_str(); }
        syn::InstrumentId id() const override { return m_id; }
        const char* description() const override { return m_description.c_str(); }

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return m_range; }

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::envelope::Ptr new_envelope() const override;

        double attack_duration() const override;
        double release_duration() const override;
    private:
        std::string m_name;
        std::string m_description;

        syn::InstrumentRange m_range;
        syn::InstrumentId m_id {};
        syn::Volume m_volume {};

        EnvelopeDescription m_envelope_description;
        syn::envelope::Type m_envelope_type {};

        std::vector<Partial> m_partials;
        std::vector<double> m_amplitudes;
    };
}
