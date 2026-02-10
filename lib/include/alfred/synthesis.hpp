#pragma once

#include <array>
#include <numeric>
#include <ranges>
#include <utility>

namespace syn {
    struct Envelope {
        Envelope() = default;
        virtual ~Envelope() = default;

        Envelope(const Envelope&) = default;
        Envelope& operator=(const Envelope&) = default;
        Envelope(Envelope&&) = default;
        Envelope& operator=(Envelope&&) = default;

        virtual double get_value(double time, double time_note_on, double time_note_off) const = 0;
        virtual bool is_done(double time, double time_note_on, double time_note_off) const = 0;
    };

    struct EnvelopeAdsrDescription {
        double time_attack {0.1};
        double time_decay {0.02};
        double time_release {0.2};

        double value_start {1.0};
        double value_sustain {0.8};
    };

    class EnvelopeAdsr : public Envelope {
    public:
        constexpr EnvelopeAdsr(const EnvelopeAdsrDescription& description)
            : m_description(description) {}

        double get_value(double time, double time_note_on, double time_note_off) const override;
        bool is_done(double time, double time_note_on, double time_note_off) const override;
    private:
        double ads(double life_time) const;
        double r(double time, double time_note_on, double time_note_off) const;

        EnvelopeAdsrDescription m_description;
    };

    struct EnvelopeAdrDescription {
        double time_attack {0.01};
        double time_decay {1.0};
        double time_release {0.1};
    };

    class EnvelopeAdr : public Envelope {
    public:
        constexpr EnvelopeAdr(const EnvelopeAdrDescription& description)
            : m_description(description) {}

        double get_value(double time, double time_note_on, double time_note_off) const override;
        bool is_done(double time, double time_note_on, double time_note_off) const override;
    private:
        double ad(double life_time) const;
        double r(double time, double time_note_on, double time_note_off) const;

        EnvelopeAdrDescription m_description;
    };

    // MIDI-like note ID
    using Id = unsigned int;

    enum Name : unsigned int {
        A,
        As,
        B,
        C,
        Cs,
        D,
        Ds,
        E,
        F,
        Fs,
        G,
        Gs
    };

    enum Octave : unsigned int {
        Octave1,
        Octave2,
        Octave3,
        Octave4,
        Octave5,
        Octave6,
        Octave7
    };

    using VoiceId = unsigned int;

    using VoiceRange = std::pair<Id, Id>;

    struct Note {
        Id id {};
        VoiceId voice {};
        double time_on {};
        double time_off {};

        static Id get_id(Name name, Octave octave);
    };

    namespace keyboard {
        enum Octave : unsigned int {
            Octave1,
            Octave2,
            Octave3,
            Octave4,
            Octave5
        };

        inline constexpr int OCTAVES {5};
        inline constexpr int EXTRA {4};
        inline constexpr int NOTES {OCTAVES * 12 + EXTRA};

        inline constexpr Id ID_BEGIN {0};
        inline constexpr Id ID_END {NOTES - 1};
    }

    struct Voice {
        Voice() = default;
        virtual ~Voice() = default;

        Voice(const Voice&) = default;
        Voice& operator=(const Voice&) = default;
        Voice(Voice&&) = default;
        Voice& operator=(Voice&&) = default;

        virtual const char* name() const = 0;
        virtual VoiceId id() const = 0;
        virtual const Envelope& overall_envelope() const = 0;
        virtual double sound(double time, const Note& note) const = 0;
        virtual VoiceRange range() const { return std::make_pair(keyboard::ID_BEGIN, keyboard::ID_END); }
    };

    struct LowFrequencyOscillator {
        double frequency {};
        double deviation {};
    };

    namespace oscillator {
        double sine(double time, double frequency);
        double sine(double time, double frequency, LowFrequencyOscillator lfo);
        double square(double time, double frequency);
        double square(double time, double frequency, LowFrequencyOscillator lfo);
        double triangle(double time, double frequency);
        double triangle(double time, double frequency, LowFrequencyOscillator lfo);
        double sawtooth(double time, double frequency);
        double sawtooth(double time, double frequency, LowFrequencyOscillator lfo);
    }

    double frequency_modulation(double time, double frequency, LowFrequencyOscillator lfo);
    double noise();
    double id_frequency(Id id);
    double note_frequency(const Note& note);

    template<std::size_t N>
    constexpr std::array<double, N> amplitudes(std::array<double, N> denominators) {
        for (const auto [i, denominator] : denominators | std::views::enumerate) {
            denominators[i] = 1.0 / double(denominator);
        }

        const double sum {std::accumulate(denominators.begin(), denominators.end(), 0.0)};

        for (double& denominator : denominators) {
            denominator /= sum;
        }

        return denominators;
    }

    namespace padsynth {

    }
}
