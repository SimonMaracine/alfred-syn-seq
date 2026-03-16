#include "alfred/preset.hpp"

namespace preset {
    double RuntimeInstrument::sound(double time, double, syn::NoteId note) const noexcept {
        double output {};

        for (const auto& [i, partial] : m_partials | std::views::enumerate) {
            switch (partial.type) {
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
            }
        }

        return output;
    }

    syn::envelope::Ptr RuntimeInstrument::new_envelope() const {
        switch (m_envelope_description.index()) {
            case 0:
                switch (m_envelope_type) {
                    case syn::envelope::Type::Linear:
                        return std::make_unique<syn::envelope::AdsrLinear>(std::get<0>(m_envelope_description));
                    case syn::envelope::Type::Exponential:
                        return std::make_unique<syn::envelope::Adsr>(std::get<0>(m_envelope_description));
                }
                std::unreachable();
            case 1:
                switch (m_envelope_type) {
                    case syn::envelope::Type::Linear:
                        return std::make_unique<syn::envelope::AdrLinear>(std::get<1>(m_envelope_description));
                    case syn::envelope::Type::Exponential:
                        return std::make_unique<syn::envelope::Adr>(std::get<1>(m_envelope_description));
                }
                std::unreachable();
        }

        std::unreachable();
    }

    double RuntimeInstrument::attack_duration() const {
        switch (m_envelope_description.index()) {
            case 0:
                return std::get<0>(m_envelope_description).duration_attack;
            case 1:
                return std::get<1>(m_envelope_description).duration_attack;
        }

        std::unreachable();
    }

    double RuntimeInstrument::release_duration() const {
        switch (m_envelope_description.index()) {
            case 0:
                return std::get<0>(m_envelope_description).duration_release;
            case 1:
                return std::get<1>(m_envelope_description).duration_release;
        }

        std::unreachable();
    }
}
