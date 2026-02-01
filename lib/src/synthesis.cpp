#include "alfred/synthesis.hpp"

#include <random>
#include <ranges>
#include <array>
#include <cmath>
#include <cassert>

#include "alfred/math.hpp"

namespace syn {
    struct LowFrequencyOscillator {
        double frequency {};
        double deviation {};
    };

    static double frequency_modulation(double time, double frequency, LowFrequencyOscillator lfo = {}) {
        return lfo.deviation * frequency * std::sin(math::w(lfo.frequency) * time);
    }

    namespace oscillators {
        static double wave_sine(double time, double frequency) {
            return std::sin(math::w(frequency) * time);
        }

        static double wave_sine(double time, double frequency, LowFrequencyOscillator lfo) {
            return std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo));
        }

        static double wave_square(double time, double frequency) {
            const double value {std::sin(math::w(frequency) * time)};

            return value >= 0.0 ? 1.0 : -1.0;
        }

        static double wave_square(double time, double frequency, LowFrequencyOscillator lfo) {
            const double value {std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))};

            return value >= 0.0 ? 1.0 : -1.0;
        }

        static double wave_triangle(double time, double frequency) {
            return std::asin(std::sin(math::w(frequency) * time)) * (2.0 / math::PI);
        }

        static double wave_triangle(double time, double frequency, LowFrequencyOscillator lfo) {
            return std::asin(std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))) * (2.0 / math::PI);
        }

        static double wave_sawtooth(double time, double frequency) {
            double result {};

            for (double n {1.0}; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time) / n;
            }

            return result * (2.0 / math::PI);
        }

        static double wave_sawtooth(double time, double frequency, LowFrequencyOscillator lfo) {
            double result {};

            for (double n {1.0}; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time + frequency_modulation(time, frequency, lfo)) / n;
            }

            return result * (2.0 / math::PI);
        }
    }

    thread_local struct {
        std::mt19937_64 random;
        std::uniform_real_distribution<> distribution {-1.0, 1.0};
    } g_random;

    static double noise() {
        return g_random.distribution(g_random.random);
    }

    static constexpr double id_frequency(Id id) {
        static constexpr double BASE_FREQUENCY {55.0};  // A1
        static constexpr double STEP_FREQUENCY {1.059463094};  // 2.0 ** (1.0 / 12.0)

        return BASE_FREQUENCY * std::pow(STEP_FREQUENCY, double(id));
    }

    static constexpr double note_frequency(const Note& note) {
        return id_frequency(note.id);
    }

    double EnvelopeAdsr::get_value(double time, double time_note_on, double time_note_off) const {
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
        }

        return math::less_than_eps(r(time, time_note_on, time_note_off));
    }

    double EnvelopeAdsr::ads(double life_time) const {
        double amplitude {};

        // Attack
        if (life_time <= m_description.time_attack) {
            amplitude = life_time / m_description.time_attack * m_description.value_start;
        }

        // Decay
        if (life_time > m_description.time_attack && life_time <= m_description.time_attack + m_description.time_decay) {
            amplitude = (
                (life_time - m_description.time_attack) / m_description.time_decay *
                (m_description.value_sustain - m_description.value_start) +
                m_description.value_start
            );
        }

        // Sustain
        if (life_time > m_description.time_attack + m_description.time_decay) {
            amplitude = m_description.value_sustain;
        }

        return amplitude;
    }

    double EnvelopeAdsr::r(double time, double time_note_on, double time_note_off) const {
        // Release
        const double amplitude {ads(time_note_off - time_note_on)};

        return (time - time_note_off) / m_description.time_release * -amplitude + amplitude;
    }

    double EnvelopeAdr::get_value(double time, double time_note_on, double time_note_off) const {
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
        }

        return math::less_than_eps(r(time, time_note_on, time_note_off));
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

    Id Note::get_id(Name name, Octave octave) {
        const unsigned int base {name};
        const unsigned int multiplier {octave};

        if (base < 3) {
            return base + 12 * multiplier;
        } else {
            return base + 12 * (multiplier - 1);
        }
    }

    template<std::size_t N>
    static constexpr std::array<double, N> amplitudes(std::array<double, N> denominators) {
        assert(denominators[0] == 1.0);

        for (const auto [i, denominator] : denominators | std::views::enumerate) {
            denominators[i] = 1.0 / double(denominator);
        }

        const double sum {std::accumulate(denominators.begin(), denominators.end(), 0.0)};

        for (double& denominator : denominators) {
            denominator /= sum;
        }

        return denominators;
    }

    namespace instruments {
        double Metronome::sound(double time, const Note& note) const {
            static constexpr auto AMP {amplitudes(std::array { 1.0, 2.0, 4.0, 25.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_triangle(time, 1.0 * note_frequency(note)) +
                AMP[1] * oscillators::wave_triangle(time, 2.0 * note_frequency(note)) +
                AMP[2] * oscillators::wave_triangle(time, 4.0 * note_frequency(note)) +
                AMP[3] * noise()
            );
        }

        double Bell::sound(double time, const Note& note) const {
            static constexpr auto AMP {amplitudes(std::array { 1.0, 2.0, 4.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_sine(time, 1.0 * note_frequency(note), { 5.0, 0.001 }) +
                AMP[1] * oscillators::wave_sine(time, 2.0 * note_frequency(note)) +
                AMP[2] * oscillators::wave_sine(time, 4.0 * note_frequency(note))
            );
        }

        std::pair<Id, Id> Bell::range() const {
            return std::make_pair(12, 51);
        }

        double Harmonica::sound(double time, const Note& note) const {
            static constexpr auto AMP {amplitudes(std::array { 1.0, 2.0, 4.0, 50.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_square(time, 1.0 * note_frequency(note), { 5.0, 0.001 }) +
                AMP[1] * oscillators::wave_square(time, 2.0 * note_frequency(note)) +
                AMP[2] * oscillators::wave_square(time, 4.0 * note_frequency(note)) +
                AMP[3] * noise()
            );
        }

        double DrumBass::sound(double time, const Note& note) const {
            static constexpr Id C3 {15};
            static constexpr auto AMP {amplitudes(std::array { 1.0, 6.0, 15.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_sine(time, 1.0 * id_frequency(C3)) +
                AMP[1] * oscillators::wave_sawtooth(time, 1.0 * id_frequency(C3)) +
                AMP[2] * noise()
            );
        }

        double DrumSnare::sound(double time, const Note& note) const {
            static constexpr Id C3 {15};
            static constexpr auto AMP {amplitudes(std::array { 1.0, 2.0, 4.0, 8.0, 5.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_sine(time, 1.0 * id_frequency(C3)) +
                AMP[1] * oscillators::wave_sine(time, 4.0 * id_frequency(C3)) +
                AMP[3] * oscillators::wave_sawtooth(time, 1.0 * id_frequency(C3)) +
                AMP[4] * noise()
            );
        }

        double DrumHiHat::sound(double time, const Note& note) const {
            static constexpr Id C4 {27};
            static constexpr auto AMP {amplitudes(std::array { 1.0, 4.0, 0.5 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_square(time, 1.0 * id_frequency(C4)) +
                AMP[1] * oscillators::wave_square(time, 2.0 * id_frequency(C4)) +
                AMP[2] * noise()
            );
        }

        double Piano::sound(double time, const Note& note) const {
            static constexpr auto AMP {amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_sine(time, 1.0 * note_frequency(note), { 8.0, 0.00001 }) +
                AMP[1] * oscillators::wave_sine(time, 2.0 * note_frequency(note)) +
                AMP[2] * oscillators::wave_sine(time, 4.0 * note_frequency(note)) +
                AMP[3] * oscillators::wave_sawtooth(time, 1.0 * note_frequency(note))
            );
        }

        double Guitar::sound(double time, const Note& note) const {
            static constexpr auto AMP {amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 })};

            return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
                AMP[0] * oscillators::wave_sine(time, 1.0 * note_frequency(note), { 10.0, 0.00001 }) +
                AMP[1] * oscillators::wave_sine(time, 2.0 * note_frequency(note)) +
                AMP[2] * oscillators::wave_sine(time, 3.0 * note_frequency(note)) +
                AMP[3] * oscillators::wave_sawtooth(time, 4.0 * note_frequency(note))
            );
        }

        std::pair<Id, Id> Guitar::range() const {
            return std::make_pair(7, 51);
        }
    }
}
