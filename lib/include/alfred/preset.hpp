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
        syn::oscillator::Type type {};
        double frequency_multiplier = 1.0;
        double amplitude_divisor = 1.0;
        double phase {};
        std::optional<syn::LowFrequencyOscillator> lfo;
    };

    // Template for any kind of runtime-defined instrument
    class RuntimeInstrument : public syn::Instrument {
    public:
        template<typename EnvelopeDescription>
        RuntimeInstrument(
            std::string name,
            std::string description,
            syn::InstrumentRange range,
            const EnvelopeDescription& envelope_description,
            syn::envelope::Type envelope_type,
            std::vector<Partial> partials
        ) :
            m_name(std::move(name)),
            m_description(std::move(description)),
            m_range(range),
            m_id(hash::HashedStr32(m_name)),
            m_envelope_description(envelope_description),
            m_envelope_type(envelope_type),
            m_partials(std::move(partials))
        {
            for (const auto& partial : m_partials) {
                m_amplitudes.push_back(partial.amplitude_divisor);
            }

            syn::util::amplitudes(m_amplitudes);
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

        std::variant<syn::envelope::DescriptionAdsr, syn::envelope::DescriptionAdr> m_envelope_description;
        syn::envelope::Type m_envelope_type {};

        std::vector<Partial> m_partials;
        std::vector<double> m_amplitudes;
    };
}
