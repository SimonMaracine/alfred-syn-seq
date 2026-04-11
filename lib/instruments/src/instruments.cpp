#include "alfred/instruments.hpp"

#include <alfred/math.hpp>
#include <alfred/definitions.hpp>

namespace alfred::instruments {
    void initialize_builtin_instruments(synthesizer::Synthesizer& synthesizer) {
        (void) synthesizer.store_instrument(std::make_unique<ShortSynthPiano>());
        (void) synthesizer.store_instrument(std::make_unique<Metronome>());
        (void) synthesizer.store_instrument(std::make_unique<Ghost>());
        (void) synthesizer.store_instrument(std::make_unique<Harmonica>());
        (void) synthesizer.store_instrument(std::make_unique<DrumBass>());
        (void) synthesizer.store_instrument(std::make_unique<DrumSnare>());
        (void) synthesizer.store_instrument(std::make_unique<DrumHiHat>());
        (void) synthesizer.store_instrument(std::make_unique<SynthPiano>());
        (void) synthesizer.store_instrument(std::make_unique<Guitar>());
        (void) synthesizer.store_instrument(std::make_unique<Strings>());
        (void) synthesizer.store_instrument(std::make_unique<Cello>());
    }

    double ShortSynthPiano::sound(double time, const syn::voice::Voice& voice) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 25.0 });

        return
            amp[0] * syn::oscillator::triangle(time, 1.0 * syn::frequency(voice.note), 0.0) +
            amp[1] * syn::oscillator::triangle(time, 2.0 * syn::frequency(voice.note), 0.0) +
            amp[2] * syn::oscillator::triangle(time, 4.0 * syn::frequency(voice.note), 0.0) +
            amp[3] * syn::noise();
    }

    double Ghost::sound(double time, const syn::voice::Voice& voice) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(voice.note), 0.0, { 5.0, 0.02 }) +
            amp[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(voice.note), 0.0) +
            amp[2] * syn::oscillator::sine(time, 4.0 * syn::frequency(voice.note), 0.0);
    }

    double Harmonica::sound(double time, const syn::voice::Voice& voice) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 50.0 });

        return
            amp[0] * syn::oscillator::square(time, 1.0 * syn::frequency(voice.note), 0.0, { 5.0, 0.001 }) +
            amp[1] * syn::oscillator::square(time, 2.0 * syn::frequency(voice.note), 0.0) +
            amp[2] * syn::oscillator::square(time, 4.0 * syn::frequency(voice.note), 0.0) +
            amp[3] * syn::noise();
    }

    double DrumBass::sound(double time, const syn::voice::Voice&) const noexcept {
        static constexpr syn::NoteId C3 = syn::note(syn::C, syn::Octave3);
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 6.0, 15.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[1] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[2] * syn::noise();
    }

    double DrumSnare::sound(double time, const syn::voice::Voice&) const noexcept {
        static constexpr syn::NoteId C3 = syn::note(syn::C, syn::Octave3);
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0, 5.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[1] * syn::oscillator::sine(time, 4.0 * syn::frequency(C3), 0.0) +
            amp[3] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3), 0.0) +
            amp[4] * syn::noise();
    }

    double DrumHiHat::sound(double time, const syn::voice::Voice&) const noexcept {
        static constexpr syn::NoteId C4 = syn::note(syn::C, syn::Octave4);
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 4.0, 0.5 });

        return
            amp[0] * syn::oscillator::square(time, 1.0 * syn::frequency(C4), 0.0) +
            amp[1] * syn::oscillator::square(time, 2.0 * syn::frequency(C4), 0.0) +
            amp[2] * syn::noise();
    }

    double SynthPiano::sound(double time, const syn::voice::Voice& voice) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 40.0, 30.0, 1.0, 1.5, 6.0, 12.0, 20.0, 90.0 });

        return
            amp[0] * syn::oscillator::sine(time, 0.85 * syn::frequency(voice.note), 0.0) +
            amp[1] * syn::oscillator::sine(time, 0.95 * syn::frequency(voice.note), 0.0) +
            amp[2] * syn::oscillator::sine(time, 1.0 * syn::frequency(voice.note), 0.0, { 4.5, 0.001 }) +
            amp[3] * syn::oscillator::triangle(time, 2.0 * syn::frequency(voice.note), 0.0) +
            amp[4] * syn::oscillator::sine(time, 3.025 * syn::frequency(voice.note), 0.0) +
            amp[5] * syn::oscillator::sine(time, 4.05 * syn::frequency(voice.note), 0.0) +
            amp[6] * syn::oscillator::sine(time, 5.1 * syn::frequency(voice.note), 0.0) +
            amp[7] * syn::noise();
    }

    double Guitar::sound(double time, const syn::voice::Voice& voice) const noexcept {
        static constexpr auto amp = syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 });

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(voice.note), 0.0, { 10.0, 0.00001 }) +
            amp[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(voice.note), 0.0) +
            amp[2] * syn::oscillator::sine(time, 3.0 * syn::frequency(voice.note), 0.0) +
            amp[3] * syn::oscillator::sawtooth(time, 4.0 * syn::frequency(voice.note), 0.0);
    }

    Strings::Strings() {
        double amplitude_harmonics[64] {};

        for (std::size_t i = 1; i < std::size(amplitude_harmonics); i++) {
            amplitude_harmonics[i] = 1.0 / double(i);

            if (i % 2 == 0) {
                amplitude_harmonics[i] *= 2.0;
            }
        }

        m_sample = syn::padsynth::SampleCopyable(
            syn::padsynth::padsynth(
                SIZE,
                def::SAMPLE_FREQUENCY,
                FREQUENCY,
                40.0,
                amplitude_harmonics,
                std::size(amplitude_harmonics)
            ),
            SIZE
        );
    }

    double Strings::sound(double time, const syn::voice::Voice& voice) const noexcept {
        return syn::util::sound(time, voice.note, m_sample.get().get(), SIZE, FREQUENCY);
    }

    Cello::Cello() {
        double amplitude_harmonics[64] {};

        for (std::size_t i = 1; i < std::size(amplitude_harmonics); i++) {
            amplitude_harmonics[i] = 1.0 / double(i);

            if (i % 2 == 0) {
                amplitude_harmonics[i] *= 2.0;
            }
        }

        m_sample = syn::padsynth::SampleCopyable(
            syn::padsynth::padsynth(
                SIZE,
                def::SAMPLE_FREQUENCY,
                FREQUENCY,
                2.0,
                amplitude_harmonics,
                std::size(amplitude_harmonics)
            ),
            SIZE
        );

        double t {};

        for (std::size_t i {}; i < SIZE; i++) {
            m_sample.get()[i] *= 1.0 + LFO_DEVIATION * (syn::oscillator::sine(t, LFO_FREQUENCY, 0.0) - 1.0);
            t += 1.0 / double(def::SAMPLE_FREQUENCY);
        }
    }

    double Cello::sound(double time, const syn::voice::Voice& voice) const noexcept {
        return syn::util::sound(time, voice.note, m_sample.get().get(), SIZE, FREQUENCY);
    }

    double EasterEgg::sound(double time, const syn::voice::Voice& voice) const noexcept {
        return syn::oscillator::sawtooth(time, syn::frequency(voice.note), 0.0, { 4.0, 0.05 });
    }
}
