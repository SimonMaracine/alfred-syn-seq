#include "alfred/instrument.hpp"

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

        // static constexpr std::array sounds {
            // syn::util::SoundAtTime {
            //     0.034 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.177, 0.2, 0.27, 0.5 },
            //             std::array { 1.0, 5.011, 4.466, 17.782, 11.22, 31.622, 22.387, 8.912, 12.589, 7.943, 10.0, 12.589 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.044 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.177, 0.2, 0.27, 0.5 },
            //             std::array { 1.0, 5.011, 5.623, 19.952, 14.125, 39.81, 79.432, 11.22, 14.125, 8.912, 14.125, 17.782 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.088 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.2, 0.27, 0.5 },
            //             std::array { 1.0, 79.432, 4.466, 7.079, 25.118, 22.387, 50.118, 125.892, 14.125, 10.0, 19.952, 25.118 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.128 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 9.3, 0.139, 0.2, 0.27, 0.33, 0.5 },
            //             std::array { 1.0, 4.466, 8.912, 39.81, 31.622, 199.526, 251.188, 19.952, 11.22, 28.183, 50.118, 39.81 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.172 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 9.3, 0.139, 0.2, 0.27, 0.36, 0.38, 0.513, 0.577, 0.73 },
            //             std::array { 1.0, 398.107, 4.466, 12.589, 44.668, 63.095, 158.489, 794.328, 707.945, 31.622, 14.125, 44.668, 100.0, 100.0, 100.0, 100.0, 100.0 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.218 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 9.3, 0.12, 0.139, 0.15, 0.195, 0.2, 0.27, 0.36, 0.38, 0.61, 0.73 },
            //             std::array { 1.0, 5.011, 17.782, 70.794, 56.234, 398.107, 1122.018, 70.7946, 446.6836, 70.794, 56.234, 50.118, 22.387, 17.782, 89.125, 177.827, 177.827, 223.872, 316.227 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.258 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.15, 0.2, 0.27 },
            //             std::array { 1.0, 5.623, 28.183, 158.489, 56.234, 398.107, 79.432, 501.187, 79.432, 44.668, 19.952, 89.125 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.3 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.15, 0.2, 0.27 },
            //             std::array { 1.0, 7.079, 39.81, 251.188, 50.118, 446.683, 141.253, 3162.277, 50.118, 50.118, 15.848, 158.489 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.344 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.2 },
            //             std::array { 1.0, 8.912, 39.81, 199.526, 56.234, 398.107, 141.253, 630.957, 35.481, 17.782 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.384 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.2 },
            //             std::array { 1.0, 8.912, 39.81, 281.838, 56.234, 316.227, 79.432, 398.107, 31.622, 22.387 }
            //         );
            //     }
            // },
            // syn::util::SoundAtTime {
            //     0.426 - 0.024,
            //     [](double time, syn::NoteId note) {
            //         return syn::util::sound(
            //             time,
            //             note,
            //             std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 8.2, 9.3, 0.12, 0.2 },
            //             std::array { 1.0, 8.912, 39.81, 446.683, 50.118, 398.107, 79.432, 398.107, 31.622, 25.118 }
            //         );
            //     }
            // }
        // };

        static constexpr std::array sounds {
syn::util::SoundAtTime {
    0.0,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.1, 6.1, 7.1, 8.2, 10.4, 0.3, 0.4, 0.6, 0.7 },
            std::array { 1.0, 1.2589, 3.1623, 5.6234, 12.5893, 35.4813, 63.0957, 50.1187, 398.1072, 25.1189, 31.6228, 28.1838, 11.2202 }
        );
    }
},
syn::util::SoundAtTime {
    0.038,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.1, 6.1, 7.1, 8.2, 10.4, 0.3, 0.4, 0.6, 0.7 },
            std::array { 1.0, 1.4125, 3.5481, 6.3096, 12.5893, 44.6684, 100.0, 63.0957, 707.9458, 28.1838, 39.8107, 35.4813, 14.1254 }
        );
    }
},
syn::util::SoundAtTime {
    0.059,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.1, 6.1, 7.1, 8.2, 10.4, 0.2, 0.4, 0.4, 0.6, 0.7 },
            std::array { 1.0, 1.5849, 3.9811, 7.9433, 12.5893, 56.2341, 141.2538, 79.4328, 891.2509, 31.6228, 70.7946, 50.1187, 44.6684, 15.8489 }
        );
    }
},
syn::util::SoundAtTime {
    0.086,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.1, 6.1, 7.1, 8.2, 10.4, 0.2, 0.4, 0.4, 0.6, 0.7 },
            std::array { 1.0, 1.7783, 5.0119, 10.0, 12.5893, 79.4328, 177.8279, 100.0, 1122.0185, 35.4813, 89.1251, 63.0957, 56.2341, 19.9526 }
        );
    }
},
syn::util::SoundAtTime {
    0.106,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 10.4, 0.2, 0.4, 0.4, 0.6, 0.7 },
            std::array { 1.0, 2.2387, 5.6234, 12.5893, 12.5893, 100.0, 281.8383, 125.8925, 1584.8932, 44.6684, 112.2018, 89.1251, 79.4328, 25.1189 }
        );
    }
},
syn::util::SoundAtTime {
    0.129,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 10.4, 0.2, 0.4, 0.4, 0.6, 0.7 },
            std::array { 1.0, 2.8184, 7.0795, 17.7828, 12.5893, 141.2538, 446.6836, 158.4893, 2238.7211, 50.1187, 141.2538, 125.8925, 112.2018, 31.6228 }
        );
    }
},
syn::util::SoundAtTime {
    0.153,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.5, 0.7 },
            std::array { 1.0, 3.5481, 10.0, 25.1189, 12.5893, 199.5262, 794.3282, 223.8721, 63.0957, 141.2538, 177.8279, 44.6684 }
        );
    }
},
syn::util::SoundAtTime {
    0.176,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.5, 0.7 },
            std::array { 1.0, 4.4668, 15.8489, 31.6228, 12.5893, 316.2278, 1122.0185, 316.2278, 79.4328, 141.2538, 223.8721, 63.0957 }
        );
    }
},
syn::util::SoundAtTime {
    0.2,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.5, 0.7 },
            std::array { 1.0, 5.0119, 28.1838, 70.7946, 12.5893, 501.1872, 1258.9254, 446.6836, 100.0, 141.2538, 316.2278, 100.0 }
        );
    }
},
syn::util::SoundAtTime {
    0.222,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.5, 0.8 },
            std::array { 1.0, 5.0119, 56.2341, 63.0957, 14.1254, 630.9573, 1258.9254, 630.9573, 141.2538, 141.2538, 398.1072, 125.8925 }
        );
    }
},
syn::util::SoundAtTime {
    0.247,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.5, 0.8 },
            std::array { 1.0, 5.0119, 44.6684, 31.6228, 14.1254, 630.9573, 1258.9254, 630.9573, 177.8279, 177.8279, 562.3413, 141.2538 }
        );
    }
},
syn::util::SoundAtTime {
    0.269,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.8 },
            std::array { 1.0, 5.0119, 28.1838, 28.1838, 14.1254, 501.1872, 891.2509, 630.9573, 223.8721, 199.5262, 177.8279 }
        );
    }
},
syn::util::SoundAtTime {
    0.292,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.3, 0.4, 0.8 },
            std::array { 1.0, 5.0119, 19.9526, 31.6228, 15.8489, 354.8134, 630.9573, 630.9573, 316.2278, 281.8383, 251.1886 }
        );
    }
},
syn::util::SoundAtTime {
    0.315,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.8 },
            std::array { 1.0, 5.0119, 14.1254, 31.6228, 17.7828, 316.2278, 501.1872, 707.9458, 398.1072, 316.2278, 316.2278 }
        );
    }
},
syn::util::SoundAtTime {
    0.339,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.7 },
            std::array { 1.0, 5.0119, 11.2202, 35.4813, 22.3872, 316.2278, 501.1872, 794.3282, 354.8134, 281.8383, 398.1072 }
        );
    }
},
syn::util::SoundAtTime {
    0.362,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 4.4668, 10.0, 39.8107, 25.1189, 446.6836, 501.1872, 1122.0185, 316.2278, 281.8383, 354.8134 }
        );
    }
},
syn::util::SoundAtTime {
    0.384,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 3.9811, 7.9433, 50.1187, 31.6228, 630.9573, 398.1072, 1995.2623, 281.8383, 316.2278, 398.1072 }
        );
    }
},
syn::util::SoundAtTime {
    0.409,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 3.1623, 7.0795, 63.0957, 39.8107, 630.9573, 354.8134, 2238.7211, 251.1886, 316.2278, 354.8134 }
        );
    }
},
syn::util::SoundAtTime {
    0.431,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 2.8184, 6.3096, 79.4328, 44.6684, 794.3282, 354.8134, 2511.8864, 251.1886, 354.8134, 446.6836 }
        );
    }
},
syn::util::SoundAtTime {
    0.454,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 2.5119, 5.6234, 89.1251, 39.8107, 707.9458, 446.6836, 3162.2777, 251.1886, 398.1072, 398.1072 }
        );
    }
},
syn::util::SoundAtTime {
    0.478,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 2.2387, 5.0119, 89.1251, 35.4813, 501.1872, 707.9458, 2818.3829, 281.8383, 398.1072, 794.3282 }
        );
    }
},
syn::util::SoundAtTime {
    0.501,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 1.9953, 4.4668, 79.4328, 28.1838, 446.6836, 1000.0, 1995.2623, 251.1886, 446.6836, 398.1072 }
        );
    }
},
syn::util::SoundAtTime {
    0.524,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 1.9953, 4.4668, 79.4328, 25.1189, 398.1072, 794.3282, 1412.5375, 251.1886, 281.8383, 446.6836, 398.1072 }
        );
    }
},
syn::util::SoundAtTime {
    0.547,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 1.9953, 3.9811, 70.7946, 22.3872, 354.8134, 501.1872, 1778.2794, 223.8721, 354.8134, 316.2278 }
        );
    }
},
syn::util::SoundAtTime {
    0.571,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 1.9953, 3.5481, 70.7946, 19.9526, 316.2278, 398.1072, 1412.5375, 199.5262, 316.2278, 316.2278 }
        );
    }
},
syn::util::SoundAtTime {
    0.598,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 1.9953, 3.5481, 63.0957, 19.9526, 251.1886, 398.1072, 1412.5375, 199.5262, 316.2278, 281.8383 }
        );
    }
},
syn::util::SoundAtTime {
    0.617,
    [](double time, syn::NoteId note) {
        return syn::util::sound(
            time,
            note,
            std::array { 1.0, 2.0, 3.0, 4.0, 5.0, 6.1, 7.1, 8.2, 0.2, 0.4, 0.4 },
            std::array { 1.0, 1.9953, 3.1623, 56.2341, 17.7828, 251.1886, 398.1072, 1412.5375, 199.5262, 316.2278, 281.8383 }
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

        for (std::size_t i {1}; i < std::size(amplitude_harmonics); i++) {
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
        return syn::util::sound(time, note, m_sample.get(), SIZE, FREQUENCY);
    }

    Cello::Cello() {
        double amplitude_harmonics[64] {};

        for (std::size_t i {1}; i < std::size(amplitude_harmonics); i++) {
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
            m_sample[i] *= 1.0 + LFO_DEVIATION * (syn::oscillator::sine(t, LFO_FREQUENCY) - 1.0);
            t += 1.0 / double(audio::SAMPLE_FREQUENCY);
        }
    }

    double Cello::sound(double time, double, syn::NoteId note) const {
        return syn::util::sound(time, note, m_sample.get(), SIZE, FREQUENCY);
    }
}
