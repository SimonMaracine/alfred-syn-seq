#include "alfred/voice.hpp"

#include <cmath>

#include "alfred/math.hpp"
#include "alfred/audio.hpp"

namespace voice {
    double Metronome::sound(double time, const syn::Note& note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 25.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::triangle(time, 1.0 * note_frequency(note)) +
            AMP[1] * syn::oscillator::triangle(time, 2.0 * note_frequency(note)) +
            AMP[2] * syn::oscillator::triangle(time, 4.0 * note_frequency(note)) +
            AMP[3] * syn::noise()
        );
    }

    double Bell::sound(double time, const syn::Note& note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::sine(time, 1.0 * note_frequency(note), { 5.0, 0.001 }) +
            AMP[1] * syn::oscillator::sine(time, 2.0 * note_frequency(note)) +
            AMP[2] * syn::oscillator::sine(time, 4.0 * note_frequency(note))
        );
    }

    syn::VoiceRange Bell::range() const {
        return std::make_pair(12, 51);
    }

    double Harmonica::sound(double time, const syn::Note& note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 50.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::square(time, 1.0 * note_frequency(note), { 5.0, 0.001 }) +
            AMP[1] * syn::oscillator::square(time, 2.0 * note_frequency(note)) +
            AMP[2] * syn::oscillator::square(time, 4.0 * note_frequency(note)) +
            AMP[3] * syn::noise()
        );
    }

    double DrumBass::sound(double time, const syn::Note& note) const {
        static constexpr syn::Id C3 {15};
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 6.0, 15.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::id_frequency(C3)) +
            AMP[1] * syn::oscillator::sawtooth(time, 1.0 * syn::id_frequency(C3)) +
            AMP[2] * syn::noise()
        );
    }

    double DrumSnare::sound(double time, const syn::Note& note) const {
        static constexpr syn::Id C3 {15};
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0, 5.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::id_frequency(C3)) +
            AMP[1] * syn::oscillator::sine(time, 4.0 * syn::id_frequency(C3)) +
            AMP[3] * syn::oscillator::sawtooth(time, 1.0 * syn::id_frequency(C3)) +
            AMP[4] * syn::noise()
        );
    }

    double DrumHiHat::sound(double time, const syn::Note& note) const {
        static constexpr syn::Id C4 {27};
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 4.0, 0.5 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::square(time, 1.0 * syn::id_frequency(C4)) +
            AMP[1] * syn::oscillator::square(time, 2.0 * syn::id_frequency(C4)) +
            AMP[2] * syn::noise()
        );
    }

    double Piano::sound(double time, const syn::Note& note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::sine(time, 1.0 * note_frequency(note), { 8.0, 0.00001 }) +
            AMP[1] * syn::oscillator::sine(time, 2.0 * note_frequency(note)) +
            AMP[2] * syn::oscillator::sine(time, 4.0 * note_frequency(note)) +
            AMP[3] * syn::oscillator::sawtooth(time, 1.0 * note_frequency(note))
        );
    }

    double Guitar::sound(double time, const syn::Note& note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 })};

        return OVERALL_ENVELOPE.get_value(time, note.time_on, note.time_off) * (
            AMP[0] * syn::oscillator::sine(time, 1.0 * note_frequency(note), { 10.0, 0.00001 }) +
            AMP[1] * syn::oscillator::sine(time, 2.0 * note_frequency(note)) +
            AMP[2] * syn::oscillator::sine(time, 3.0 * note_frequency(note)) +
            AMP[3] * syn::oscillator::sawtooth(time, 4.0 * note_frequency(note))
        );
    }

    syn::VoiceRange Guitar::range() const {
        return std::make_pair(7, 51);
    }

    Test::Test() {
        double amplitude_harmonics[64] {};

        for (std::size_t i {1}; i < std::size(amplitude_harmonics); ++i) {
            amplitude_harmonics[i] = 1.0 / double(i);

            if (i % 2 == 0) {
                amplitude_harmonics[i] *= 2.0;
            }
        }

        m_sample = syn::padsynth::padsynth(
            262144,
            audio::SAMPLE_FREQUENCY,
            261.0,
            40.0,
            amplitude_harmonics,
            int(std::size(amplitude_harmonics))
        );
    }

    double Test::sound(double time, const syn::Note& note) const {
        static constexpr double SAMPLE_DURATION {262144.0 / 44100.0};

        double _;
        const double part {std::modf(time / SAMPLE_DURATION, &_)};

        const auto index {std::size_t(part * 262144.0)};

        return m_sample[index];
    }
}
