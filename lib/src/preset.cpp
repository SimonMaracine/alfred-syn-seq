#include "alfred/preset.hpp"

#include <ranges>

#include "alfred/hash.hpp"

namespace preset {
    namespace add {
        RuntimeInstrument::RuntimeInstrument(Preset preset)
            : BaseRuntimeInstrument(std::move(preset), hash::HashedStr32(preset.name)) {
            for (const Partial& partial : m_preset.partials) {
                m_amplitudes.push_back(partial.amplitude_divisor);
            }

            m_amplitudes = syn::util::amplitudes(std::move(m_amplitudes));
        }

        double RuntimeInstrument::sound(double time, double, syn::NoteId note) const noexcept {
            double output {};

            for (const auto& [i, partial] : m_preset.partials | std::views::enumerate) {
                switch (partial.oscillator_type) {
                    case syn::oscillator::Type::Sine:
                        if (partial.lfo) {
                            output += m_amplitudes[i] * syn::oscillator::sine(time, partial.frequency_multiplier * syn::frequency(note), partial.phase, *partial.lfo);
                        } else {
                            output += m_amplitudes[i] * syn::oscillator::sine(time, partial.frequency_multiplier * syn::frequency(note), partial.phase);
                        }
                        break;
                    case syn::oscillator::Type::Square:
                        if (partial.lfo) {
                            output += m_amplitudes[i] * syn::oscillator::square(time, partial.frequency_multiplier * syn::frequency(note), partial.phase, *partial.lfo);
                        } else {
                            output += m_amplitudes[i] * syn::oscillator::square(time, partial.frequency_multiplier * syn::frequency(note), partial.phase);
                        }
                        break;
                    case syn::oscillator::Type::Triangle:
                        if (partial.lfo) {
                            output += m_amplitudes[i] * syn::oscillator::triangle(time, partial.frequency_multiplier * syn::frequency(note), partial.phase, *partial.lfo);
                        } else {
                            output += m_amplitudes[i] * syn::oscillator::triangle(time, partial.frequency_multiplier * syn::frequency(note), partial.phase);
                        }
                        break;
                    case syn::oscillator::Type::Sawtooth:
                        if (partial.lfo) {
                            output += m_amplitudes[i] * syn::oscillator::sawtooth(time, partial.frequency_multiplier * syn::frequency(note), partial.phase, *partial.lfo);
                        } else {
                            output += m_amplitudes[i] * syn::oscillator::sawtooth(time, partial.frequency_multiplier * syn::frequency(note), partial.phase);
                        }
                        break;
                    case syn::oscillator::Type::Noise:
                        output += m_amplitudes[i] * syn::noise();
                        break;

                }
            }

            return output;
        }

        syn::envelope::Ptr RuntimeInstrument::new_envelope() const {
            switch (m_preset.envelope_description.index()) {
                case 0:
                    switch (m_preset.envelope_type) {
                    case syn::envelope::Type::Linear:
                            return std::make_unique<syn::envelope::AdsrLinear>(std::get<0>(m_preset.envelope_description));
                    case syn::envelope::Type::Exponential:
                            return std::make_unique<syn::envelope::Adsr>(std::get<0>(m_preset.envelope_description));
                    }
                    std::unreachable();
                case 1:
                    switch (m_preset.envelope_type) {
                    case syn::envelope::Type::Linear:
                            return std::make_unique<syn::envelope::AdrLinear>(std::get<1>(m_preset.envelope_description));
                    case syn::envelope::Type::Exponential:
                            return std::make_unique<syn::envelope::Adr>(std::get<1>(m_preset.envelope_description));
                    }
                    std::unreachable();
            }

            std::unreachable();
        }

        double RuntimeInstrument::attack_duration() const {
            switch (m_preset.envelope_description.index()) {
                case 0:
                    return std::get<0>(m_preset.envelope_description).duration_attack;
                case 1:
                    return std::get<1>(m_preset.envelope_description).duration_attack;
            }

            std::unreachable();
        }

        double RuntimeInstrument::release_duration() const {
            switch (m_preset.envelope_description.index()) {
                case 0:
                    return std::get<0>(m_preset.envelope_description).duration_release;
                case 1:
                    return std::get<1>(m_preset.envelope_description).duration_release;
            }

            std::unreachable();
        }
    }

    namespace pad {
        RuntimeInstrument::RuntimeInstrument(Preset preset)
            : BaseRuntimeInstrument(std::move(preset), 0) {
        }

        double RuntimeInstrument::sound(double time, double time_on, syn::NoteId note) const noexcept {

        }

        syn::envelope::Ptr RuntimeInstrument::new_envelope() const {

        }

        double RuntimeInstrument::attack_duration() const {

        }

        double RuntimeInstrument::release_duration() const {

        }
    }
}
