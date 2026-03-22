#include "alfred/instruments.hpp"

#include <cstring>

#include "alfred/math.hpp"
#include "alfred/audio.hpp"

namespace instruments {
    double Metronome::sound(double time, double, syn::NoteId note) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 25.0 });

        return
            amp[0] * syn::oscillator::triangle(time, 1.0 * syn::frequency(note), 0.0) +
            amp[1] * syn::oscillator::triangle(time, 2.0 * syn::frequency(note), 0.0) +
            amp[2] * syn::oscillator::triangle(time, 4.0 * syn::frequency(note), 0.0) +
            amp[3] * syn::noise();
    }

    double Ghost::sound(double time, double, syn::NoteId note) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), 0.0, { 5.0, 0.02 }) +
            amp[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note), 0.0) +
            amp[2] * syn::oscillator::sine(time, 4.0 * syn::frequency(note), 0.0);
    }

    double Harmonica::sound(double time, double, syn::NoteId note) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 50.0 });

        return
            amp[0] * syn::oscillator::square(time, 1.0 * syn::frequency(note), 0.0, { 5.0, 0.001 }) +
            amp[1] * syn::oscillator::square(time, 2.0 * syn::frequency(note), 0.0) +
            amp[2] * syn::oscillator::square(time, 4.0 * syn::frequency(note), 0.0) +
            amp[3] * syn::noise();
    }

    double DrumBass::sound(double time, double, syn::NoteId) const noexcept {
        static constexpr syn::NoteId C3 = 15;
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 6.0, 15.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[1] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[2] * syn::noise();
    }

    double DrumSnare::sound(double time, double, syn::NoteId) const noexcept {
        static constexpr syn::NoteId C3 = 15;
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0, 5.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[1] * syn::oscillator::sine(time, 4.0 * syn::frequency(C3), 0.0) +
            amp[3] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[4] * syn::noise();
    }

    double DrumHiHat::sound(double time, double, syn::NoteId) const noexcept {
        static constexpr syn::NoteId C4 = 27;
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 4.0, 0.5 });

        return
            amp[0] * syn::oscillator::square(time, 1.0 * syn::frequency(C4), 0.0) +
            amp[1] * syn::oscillator::square(time, 2.0 * syn::frequency(C4), 0.0) +
            amp[2] * syn::noise();
    }

    double SynthPiano::sound(double time, double, syn::NoteId note) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 40.0, 30.0, 1.0, 1.5, 6.0, 12.0, 20.0, 90.0 });

        return
            amp[0] * syn::oscillator::sine(time, 0.85 * syn::frequency(note), 0.0) +
            amp[1] * syn::oscillator::sine(time, 0.95 * syn::frequency(note), 0.0) +
            amp[2] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), 0.0, { 4.5, 0.001 }) +
            amp[3] * syn::oscillator::triangle(time, 2.0 * syn::frequency(note), 0.0) +
            amp[4] * syn::oscillator::sine(time, 3.025 * syn::frequency(note), 0.0) +
            amp[5] * syn::oscillator::sine(time, 4.05 * syn::frequency(note), 0.0) +
            amp[6] * syn::oscillator::sine(time, 5.1 * syn::frequency(note), 0.0) +
            amp[7] * syn::noise();
    }

    double Guitar::sound(double time, double, syn::NoteId note) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), 0.0, { 10.0, 0.00001 }) +
            amp[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note), 0.0) +
            amp[2] * syn::oscillator::sine(time, 3.0 * syn::frequency(note), 0.0) +
            amp[3] * syn::oscillator::sawtooth(time, 4.0 * syn::frequency(note), 0.0);
    }

    Strings::Strings() {
        double amplitude_harmonics[64] {};

        for (std::size_t i = 1; i < std::size(amplitude_harmonics); i++) {
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

    Strings::Strings(const Strings& other) {
        m_sample = std::make_unique<double[]>(SIZE);
        std::memcpy(m_sample.get(), other.m_sample.get(), SIZE * sizeof(double));

        m_volume = other.m_volume;
    }

    Strings& Strings::operator=(const Strings& other) {
        m_sample = std::make_unique<double[]>(SIZE);
        std::memcpy(m_sample.get(), other.m_sample.get(), SIZE * sizeof(double));

        m_volume = other.m_volume;

        return *this;
    }

    double Strings::sound(double time, double, syn::NoteId note) const noexcept {
        return syn::util::sound(time, note, m_sample.get(), SIZE, FREQUENCY);
    }

    Cello::Cello() {
        double amplitude_harmonics[64] {};

        for (std::size_t i = 1; i < std::size(amplitude_harmonics); i++) {
            amplitude_harmonics[i] = 1.0 / double(i);

            if (i % 2 == 0) {
                amplitude_harmonics[i] *= 2.0;
            }
        }

        m_sample = syn::padsynth::padsynth(
            SIZE,
            audio::SAMPLE_FREQUENCY,
            FREQUENCY,
            2.0,
            amplitude_harmonics,
            int(std::size(amplitude_harmonics))
        );

        double t {};
        for (std::size_t i {}; i < SIZE; i++) {
            m_sample[i] *= 1.0 + LFO_DEVIATION * (syn::oscillator::sine(t, LFO_FREQUENCY, 0.0) - 1.0);
            t += 1.0 / double(audio::SAMPLE_FREQUENCY);
        }
    }

    Cello::Cello(const Cello& other) {
        m_sample = std::make_unique<double[]>(SIZE);
        std::memcpy(m_sample.get(), other.m_sample.get(), SIZE * sizeof(double));

        m_volume = other.m_volume;
    }

    Cello& Cello::operator=(const Cello& other) {
        m_sample = std::make_unique<double[]>(SIZE);
        std::memcpy(m_sample.get(), other.m_sample.get(), SIZE * sizeof(double));

        m_volume = other.m_volume;

        return *this;
    }

    double Cello::sound(double time, double, syn::NoteId note) const noexcept {
        return syn::util::sound(time, note, m_sample.get(), SIZE, FREQUENCY);
    }

    double Test::sound(double time, double, syn::NoteId note) const noexcept {
        return syn::oscillator::sawtooth(time, syn::frequency(note), 0.0, { 4.0, 0.05 });
    }
}
