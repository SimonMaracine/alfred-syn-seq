#include "alfred/instrument.hpp"

#include <utility>
#include <cmath>

#include "alfred/math.hpp"
#include "alfred/audio.hpp"

namespace instrument {
    double Metronome::sound(double time, double, syn::NoteId note) const {
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 25.0 })};

        return
            amp[0] * syn::oscillator::triangle(time, 1.0 * syn::frequency(note)) +
            amp[1] * syn::oscillator::triangle(time, 2.0 * syn::frequency(note)) +
            amp[2] * syn::oscillator::triangle(time, 4.0 * syn::frequency(note)) +
            amp[3] * syn::noise();
    }

    double Bell::sound(double time, double, syn::NoteId note) const {
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 2.0, 4.0 })};

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), { 5.0, 0.001 }) +
            amp[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note)) +
            amp[2] * syn::oscillator::sine(time, 4.0 * syn::frequency(note));
    }

    double Harmonica::sound(double time, double, syn::NoteId note) const {
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 50.0 })};

        return
            amp[0] * syn::oscillator::square(time, 1.0 * syn::frequency(note), { 5.0, 0.001 }) +
            amp[1] * syn::oscillator::square(time, 2.0 * syn::frequency(note)) +
            amp[2] * syn::oscillator::square(time, 4.0 * syn::frequency(note)) +
            amp[3] * syn::noise();
    }

    double DrumBass::sound(double time, double, syn::NoteId) const {
        static constexpr syn::NoteId C3 {15};
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 6.0, 15.0 })};

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3)) +
            amp[1] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3)) +
            amp[2] * syn::noise();
    }

    double DrumSnare::sound(double time, double, syn::NoteId) const {
        static constexpr syn::NoteId C3 {15};
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0, 5.0 })};

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(C3)) +
            amp[1] * syn::oscillator::sine(time, 4.0 * syn::frequency(C3)) +
            amp[3] * syn::oscillator::sawtooth(time, 1.0 * syn::frequency(C3)) +
            amp[4] * syn::noise();
    }

    double DrumHiHat::sound(double time, double, syn::NoteId) const {
        static constexpr syn::NoteId C4 {27};
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 4.0, 0.5 })};

        return
            amp[0] * syn::oscillator::square(time, 1.0 * syn::frequency(C4)) +
            amp[1] * syn::oscillator::square(time, 2.0 * syn::frequency(C4)) +
            amp[2] * syn::noise();
    }

    double Piano::sound(double time, double time_on, syn::NoteId note) const {
        // return syn::util::sound(
        //     time,
        //     note,
        //     std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.142, 0.133, 0.210, 0.267, 0.326, 0.523 },
        //     std::array { 1.0, 7.943, 12.589, 19.952, 28.183, 50.118, 199.526, 15.848, 15.848, 19.952, 17.782, 22.387 }
        // );

        static constexpr std::array sounds {
            syn::util::SoundAtTime {
                0.034 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.177, 0.2, 0.27, 0.5 },
                        std::array { 1.0, 5.011, 4.466, 17.782, 11.22, 31.622, 22.387, 8.912, 12.589, 7.943, 10.0, 12.589 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.044 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.177, 0.2, 0.27, 0.5 },
                        std::array { 1.0, 5.011, 5.623, 19.952, 14.125, 39.81, 79.432, 11.22, 14.125, 8.912, 14.125, 17.782 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.088 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.2, 0.27, 0.5 },
                        std::array { 1.0, 79.432, 4.466, 7.079, 25.118, 22.387, 50.118, 125.892, 14.125, 10.0, 19.952, 25.118 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.128 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.2, 0.27, 0.33, 0.5 },
                        std::array { 1.0, 4.466, 8.912, 39.81, 31.622, 199.526, 251.188, 19.952, 11.22, 28.183, 50.118, 39.81 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.172 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 9.3, 0.139, 0.2, 0.27, 0.36, 0.38, 0.513, 0.577, 0.73 },
                        std::array { 1.0, 398.107, 4.466, 12.589, 44.668, 63.095, 158.489, 794.328, 707.945, 31.622, 14.125, 44.668, 100.0, 100.0, 100.0, 100.0, 100.0 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.218 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 9.3, 0.12, 0.139, 0.15, 0.195, 0.2, 0.27, 0.36, 0.38, 0.61, 0.73 },
                        std::array { 1.0, 5.011, 17.782, 70.794, 56.234, 398.107, 1122.018, 70.7946, 446.6836, 70.794, 56.234, 50.118, 22.387, 17.782, 89.125, 177.827, 177.827, 223.872, 316.227 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.258 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.15, 0.2, 0.27 },
                        std::array { 1.0, 5.623, 28.183, 158.489, 56.234, 398.107, 79.432, 501.187, 79.432, 44.668, 19.952, 89.125 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.3 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.15, 0.2, 0.27 },
                        std::array { 1.0, 7.079, 39.81, 251.188, 50.118, 446.683, 141.253, 3162.277, 50.118, 50.118, 15.848, 158.489 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.344 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.2 },
                        std::array { 1.0, 8.912, 39.81, 199.526, 56.234, 398.107, 141.253, 630.957, 35.481, 17.782 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.384 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.2 },
                        std::array { 1.0, 8.912, 39.81, 281.838, 56.234, 316.227, 79.432, 398.107, 31.622, 22.387 }
                    );
                }
            },
            syn::util::SoundAtTime {
                0.426 - 0.024,
                [](double time, syn::NoteId note) {
                    return syn::util::sound(
                        time,
                        note,
                        std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.2 },
                        std::array { 1.0, 8.912, 39.81, 446.683, 50.118, 398.107, 79.432, 398.107, 31.622, 25.118 }
                    );
                }
            },
        };

        return syn::util::sound(time, time_on, note, sounds);
    }

    double Guitar::sound(double time, double, syn::NoteId note) const {
        static constexpr auto amp {syn::util::amplitudes(std::array { 1.0, 2.0, 4.0, 8.0 })};

        return
            amp[0] * syn::oscillator::sine(time, 1.0 * syn::frequency(note), { 10.0, 0.00001 }) +
            amp[1] * syn::oscillator::sine(time, 2.0 * syn::frequency(note)) +
            amp[2] * syn::oscillator::sine(time, 3.0 * syn::frequency(note)) +
            amp[3] * syn::oscillator::sawtooth(time, 4.0 * syn::frequency(note));
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

    double Strings::sound(double time, double, syn::NoteId note) const {
        static constexpr double SAMPLE_DURATION {double(SIZE) / double(audio::SAMPLE_FREQUENCY)};

        const double pitch {syn::frequency(note) / FREQUENCY};
        double _;
        const double part {std::modf(time * pitch / SAMPLE_DURATION, &_)};
        const auto index {std::size_t(part * double(SIZE))};

        return m_sample[index];
    }
}
