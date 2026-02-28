#include "alfred/instrument.hpp"

#include <cmath>

#include "alfred/math.hpp"
#include "alfred/audio.hpp"

namespace instrument {
    double Metronome::sound(double time, syn::NoteId note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 25.0 })};

        return
            AMP[0] * syn::oscillator::triangle(time, 1.0 * syn::frequency(note)) +
            AMP[1] * syn::oscillator::triangle(time, 2.0 * syn::frequency(note)) +
            AMP[2] * syn::oscillator::triangle(time, 4.0 * syn::frequency(note)) +
            AMP[3] * syn::noise();
    }

    double Bell::sound(double time, syn::NoteId note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0 })};

        return
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), { 5.0, 0.001 }) +
            AMP[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note)) +
            AMP[2] * syn::oscillator::sine(time, 4.0 * syn::frequency(note));
    }

    double Harmonica::sound(double time, syn::NoteId note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 50.0 })};

        return
            AMP[0] * syn::oscillator::square(time, 1.0 * syn::frequency(note), { 5.0, 0.001 }) +
            AMP[1] * syn::oscillator::square(time, 2.0 * syn::frequency(note)) +
            AMP[2] * syn::oscillator::square(time, 4.0 * syn::frequency(note)) +
            AMP[3] * syn::noise();
    }

    double DrumBass::sound(double time, syn::NoteId) const {
        static constexpr syn::NoteId C3 {15};
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 6.0, 15.0 })};

        return
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3)) +
            AMP[1] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3)) +
            AMP[2] * syn::noise();
    }

    double DrumSnare::sound(double time, syn::NoteId) const {
        static constexpr syn::NoteId C3 {15};
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0, 5.0 })};

        return
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3)) +
            AMP[1] * syn::oscillator::sine(time, 4.0 * syn::frequency(C3)) +
            AMP[3] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3)) +
            AMP[4] * syn::noise();
    }

    double DrumHiHat::sound(double time, syn::NoteId) const {
        static constexpr syn::NoteId C4 {27};
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 4.0, 0.5 })};

        return
            AMP[0] * syn::oscillator::square(time, 1.0 * syn::frequency(C4)) +
            AMP[1] * syn::oscillator::square(time, 2.0 * syn::frequency(C4)) +
            AMP[2] * syn::noise();
    }

    double Piano::sound(double time, syn::NoteId note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 7.943, 12.589, 19.952, 28.183, 50.118, 199.526, 15.848, 15.848, 19.952, 17.782, 22.387 })};

        return
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note)) +
            AMP[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note)) +
            AMP[2] * syn::oscillator::sine(time, 3.0 * syn::frequency(note)) +
            AMP[3] * syn::oscillator::sine(time, 4.0 * syn::frequency(note)) +
            AMP[4] * syn::oscillator::sine(time, 5.0 * syn::frequency(note)) +
            AMP[5] * syn::oscillator::sine(time, 6.0 * syn::frequency(note)) +
            AMP[6] * syn::oscillator::sine(time, 7.142 * syn::frequency(note)) +

            AMP[7] * syn::oscillator::sine(time, 0.133 * syn::frequency(note)) +
            AMP[8] * syn::oscillator::sine(time, 0.210 * syn::frequency(note)) +
            AMP[9] * syn::oscillator::sine(time, 0.267 * syn::frequency(note)) +
            AMP[10] * syn::oscillator::sine(time, 0.326 * syn::frequency(note)) +
            AMP[11] * syn::oscillator::sine(time, 0.523 * syn::frequency(note));
    }

    double Guitar::sound(double time, syn::NoteId note) const {
        static constexpr auto AMP {syn::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 })};

        return
            AMP[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), { 10.0, 0.00001 }) +
            AMP[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note)) +
            AMP[2] * syn::oscillator::sine(time, 3.0 * syn::frequency(note)) +
            AMP[3] * syn::oscillator::sawtooth(time, 4.0 * syn::frequency(note));
    }

    Strings::Strings() {
        double amplitude_harmonics[64] {};

        for (std::size_t i {1}; i < std::size(amplitude_harmonics); ++i) {
            amplitude_harmonics[i] = 1.0 / double(i);

            if (i % 2 == 0) {
                amplitude_harmonics[i] *= 2.0;
            }
        }

        m_sample = syn::padsynth::padsynth(
            SIZE,
            audio::SAMPLE_FREQUENCY,
            FREQUENCY,
            40.0,
            amplitude_harmonics,
            int(std::size(amplitude_harmonics))
        );
    }

    double Strings::sound(double time, syn::NoteId note) const {
        static constexpr double SAMPLE_DURATION {double(SIZE) / double(audio::SAMPLE_FREQUENCY)};

        const double pitch {syn::frequency(note) / FREQUENCY};
        double _;
        const double part {std::modf(time * pitch / SAMPLE_DURATION, &_)};
        const auto index {std::size_t(part * double(SIZE))};

        return m_sample[index];
    }
}
