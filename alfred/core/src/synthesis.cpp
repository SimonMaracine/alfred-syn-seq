#include "alfred/synthesis.hpp"

#include <cmath>
#include <random>

#include "alfred/math.hpp"

namespace syn {
    namespace oscillators {
        struct LowFrequencyOscillator {
            double frequency {};
            double amplitude {};
        };

        static double frequency_modulation(double time, double frequency, LowFrequencyOscillator lfo) {
            return lfo.amplitude * frequency * std::sin(math::w(lfo.frequency) * time);
        }

        static double wave_sine(double time, double frequency, LowFrequencyOscillator lfo) {
            return std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo));
        }

        static double wave_square(double time, double frequency, LowFrequencyOscillator lfo) {
            const double value {std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))};

            if (value >= 0.0) {
                return 1.0;
            } else {
                return -1.0;
            }
        }

        static double wave_triangle(double time, double frequency, LowFrequencyOscillator lfo) {
            return std::asin(std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))) * (2.0 / math::PI);
        }

        static double wave_saw(double time, double frequency, LowFrequencyOscillator lfo) {
            double result {};

            for (double n {1.0}; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time + frequency_modulation(time, frequency, lfo)) / n;
            }

            return result * (2.0 / math::PI);
        }

        static double noise() {
            static thread_local std::mt19937_64 s_random;  // TODO

            std::uniform_real_distribution distribution {-1.0, 1.0};

            return distribution(s_random);
        }
    }

    static double note_frequency(Name name, unsigned int octave) {
        static constexpr double BASE_FREQUENCY {110.0};  // A
        static constexpr double STEP_FREQUENCY {1.059463094};  // 2.0 ** (1.0 / 12.0)

        return BASE_FREQUENCY * std::pow(STEP_FREQUENCY, double(name) + double(octave) * 12.0);
    }

    double EnvelopeAdsr::get_amplitude(double time, double time_note_on, double time_note_off) const {
        double amplitude {};

        if (time_note_on > time_note_off) {
            amplitude = ads(time - time_note_on);
        } else {
            amplitude = r(time, time_note_on, time_note_off);
        }

        return math::zero_if_less_than_eps(amplitude);
    }

    bool EnvelopeAdsr::is_done(double time, double time_note_on, double time_note_off) const {
        if (time_note_on > time_note_off) {
            return false;
        } else {
            return math::less_than_eps(r(time, time_note_on, time_note_off));
        }
    }

    double EnvelopeAdsr::ads(double life_time) const {
        double amplitude {};

        // Attack
        if (life_time <= m_description.time_attack) {
            amplitude = life_time / m_description.time_attack * m_description.amplitude_start;
        }

        // Decay
        if (life_time > m_description.time_attack && life_time <= m_description.time_attack + m_description.time_decay) {
            amplitude = (
                (life_time - m_description.time_attack) / m_description.time_decay *
                (m_description.amplitude_sustain - m_description.amplitude_start) +
                m_description.amplitude_start
            );
        }

        // Sustain
        if (life_time > m_description.time_attack + m_description.time_decay) {
            amplitude = m_description.amplitude_sustain;
        }

        return amplitude;
    }

    double EnvelopeAdsr::r(double time, double time_note_on, double time_note_off) const {
        // Release
        const double amplitude {ads(time_note_off - time_note_on)};

        return (time - time_note_off) / m_description.time_release * -amplitude + amplitude;
    }

    double EnvelopeAdr::get_amplitude(double time, double time_note_on, double time_note_off) const {
        double amplitude {};

        if (time_note_on > time_note_off) {
            amplitude = ad(time - time_note_on);
        } else {
            amplitude = r(time, time_note_on, time_note_off);
        }

        return math::zero_if_less_than_eps(amplitude);
    }

    bool EnvelopeAdr::is_done(double time, double time_note_on, double time_note_off) const {
        if (time_note_on > time_note_off) {
            return false;
        } else {
            return math::less_than_eps(r(time, time_note_on, time_note_off));
        }
    }

    double EnvelopeAdr::ad(double life_time) const {
        double amplitude {};

        // Attack
        if (life_time <= m_description.time_attack) {
            amplitude = life_time / m_description.time_attack;
        }

        // Decay
        if (life_time > m_description.time_attack && life_time <= m_description.time_attack + m_description.time_decay) {
            amplitude = (life_time - m_description.time_attack) / -m_description.time_decay + 1.0;
        }

        return amplitude;
    }

    double EnvelopeAdr::r(double time, double time_note_on, double time_note_off) const {
        // Release
        const double amplitude {ad(time_note_off - time_note_on)};

        return (time - time_note_off) / m_description.time_release * -amplitude + amplitude;
    }

    namespace instruments {
        double Bell::sound(double time, const Note& note) const {
            return m_envelope.get_amplitude(time, note.time_on, note.time_off) * (
                1.0 * oscillators::wave_sine(time, note_frequency(note.name, note.octave), { 5.0, 0.001 }) +
                0.5 * oscillators::wave_sine(time, note_frequency(note.name, note.octave + 1), {}) +
                0.25 * oscillators::wave_sine(time, note_frequency(note.name, note.octave + 2), {})
            );
        }

        double Harmonica::sound(double time, const Note& note) const {
            return m_envelope.get_amplitude(time, note.time_on, note.time_off) * (
                1.0 * oscillators::wave_square(time, note_frequency(note.name, note.octave), { 5.0, 0.001 }) +
                0.5 * oscillators::wave_square(time, note_frequency(note.name, note.octave + 1), {}) +
                0.25 * oscillators::wave_square(time, note_frequency(note.name, note.octave + 2), {}) +
                0.05 * oscillators::noise()
            );
        }
    }
}
