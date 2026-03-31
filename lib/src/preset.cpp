#include "alfred/preset.hpp"

#include <ranges>
#include <utility>

#include <alfred/audio.hpp>

namespace preset {
    namespace generic {
        template<typename Preset>
        static syn::envelope::Ptr new_overall_envelope(const Preset& preset) {
            switch (preset.envelope_description.index()) {
                case 0:
                    switch (preset.envelope_type) {
                        case syn::envelope::Type::Linear:
                            return std::make_unique<syn::envelope::AdsrLinear>(std::get<0>(preset.envelope_description));
                        case syn::envelope::Type::Exponential:
                            return std::make_unique<syn::envelope::Adsr>(std::get<0>(preset.envelope_description));
                    }
                    std::unreachable();
                case 1:
                    switch (preset.envelope_type) {
                        case syn::envelope::Type::Linear:
                            return std::make_unique<syn::envelope::AdrLinear>(std::get<1>(preset.envelope_description));
                        case syn::envelope::Type::Exponential:
                            return std::make_unique<syn::envelope::Adr>(std::get<1>(preset.envelope_description));
                    }
                    std::unreachable();
            }

            std::unreachable();
        }

        template<typename Preset>
        static double attack_duration(const Preset& preset) {
            switch (preset.envelope_description.index()) {
                case 0:
                    return std::get<0>(preset.envelope_description).duration_attack;
                case 1:
                    return std::get<1>(preset.envelope_description).duration_attack;
            }

            std::unreachable();
        }

        template<typename Preset>
        static double release_duration(const Preset& preset) {
            switch (preset.envelope_description.index()) {
                case 0:
                    return std::get<0>(preset.envelope_description).duration_release;
                case 1:
                    return std::get<1>(preset.envelope_description).duration_release;
            }

            std::unreachable();
        }
    }

    namespace add {
        RuntimeInstrument::RuntimeInstrument(Preset preset)
            : BaseRuntimeInstrument(std::move(preset)) {
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

        syn::envelope::Ptr RuntimeInstrument::new_overall_envelope() const {
            return generic::new_overall_envelope(m_preset);
        }

        double RuntimeInstrument::attack_duration() const {
            return generic::attack_duration(m_preset);
        }

        double RuntimeInstrument::release_duration() const {
            return generic::release_duration(m_preset);
        }
    }

    namespace pad {
        RuntimeInstrument::RuntimeInstrument(Preset preset)
            : BaseRuntimeInstrument(std::move(preset)) {
            m_sample = syn::padsynth::SampleCopyable(
                syn::padsynth::padsynth(
                    SIZE,
                    audio::SAMPLE_FREQUENCY,
                    m_preset.frequency,
                    m_preset.bandwidth,
                    m_preset.amplitude_harmonics.data(),
                    m_preset.amplitude_harmonics.size(),
                    profile(m_preset)
                ),
                SIZE
            );
        }

        double RuntimeInstrument::sound(double time, double, syn::NoteId note) const noexcept {
            return syn::util::sound(time, note, m_sample.get().get(), SIZE, m_preset.frequency);
        }

        syn::envelope::Ptr RuntimeInstrument::new_overall_envelope() const {
            return generic::new_overall_envelope(m_preset);
        }

        double RuntimeInstrument::attack_duration() const {
            return generic::attack_duration(m_preset);
        }

        double RuntimeInstrument::release_duration() const {
            return generic::release_duration(m_preset);
        }

        syn::padsynth::Profile RuntimeInstrument::profile(const Preset& preset) {
            switch (preset.profile) {
                case Profile::Default:
                    return nullptr;
            }

            std::unreachable();
        }
    }
}
