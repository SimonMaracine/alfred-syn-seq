#pragma once

#include <memory>
#include <array>
#include <numeric>
#include <ranges>
#include <limits>
#include <utility>

#include "alfred/allocator.hpp"

namespace syn {
    using EnvelopeStorage = allocator::StaticAllocatorStorage<64, 96, 8>;

    struct Envelope {
        Envelope() = default;
        virtual ~Envelope() = default;

        Envelope(const Envelope&) = default;
        Envelope& operator=(const Envelope&) = default;
        Envelope(Envelope&&) = default;
        Envelope& operator=(Envelope&&) = default;

        virtual void note_on(double time) = 0;
        virtual void note_off(double time) = 0;
        virtual void update(double time) = 0;
        virtual double value() const = 0;
        virtual bool done() const = 0;
    };

    struct DescriptionAdsr {
        double duration_attack {0.1};
        double duration_decay {0.02};
        double duration_release {0.2};
        double value_sustain {0.8};
    };

    struct DescriptionAdr {
        double duration_attack {0.1};
        double duration_decay {1.0};
        double duration_release {0.2};
    };

    class EnvelopeAdsrLinear : public Envelope, public allocator::StaticAllocated<EnvelopeAdsrLinear, EnvelopeStorage> {
    public:
        explicit EnvelopeAdsrLinear(const DescriptionAdsr& description = {})
            : m_description(description) {}

        void note_on(double time) override;
        void note_off(double time) override;
        void update(double time) override;
        double value() const override;
        bool done() const override;
    private:
        enum class Segment {
            None,
            Attack,
            Decay,
            Sustain,
            Release
        } m_segment {};

        DescriptionAdsr m_description;

        double m_value_current {};

        double m_attack_increment {};
        double m_decay_increment {};
        double m_release_increment {};
    };

    class EnvelopeAdsr : public Envelope, public allocator::StaticAllocated<EnvelopeAdsr, EnvelopeStorage> {
    public:
        explicit EnvelopeAdsr(const DescriptionAdsr& description = {})
            : m_description(description) {}

        void note_on(double time) override;
        void note_off(double time) override;
        void update(double time) override;
        double value() const override;
        bool done() const override;
    private:
        enum class Segment {
            None,
            Attack,
            Decay,
            Sustain,
            Release
        } m_segment {};

        DescriptionAdsr m_description;

        double m_time_note_on {};
        double m_time_note_off {};
        double m_time_decay {};

        double m_value_current {};
        double m_value_note_on {};
        double m_value_note_off {};
    };

    class EnvelopeAdrLinear : public Envelope, public allocator::StaticAllocated<EnvelopeAdrLinear, EnvelopeStorage> {
    public:
        explicit EnvelopeAdrLinear(const DescriptionAdr& description = {})
            : m_description(description) {}

        void note_on(double time) override;
        void note_off(double time) override;
        void update(double time) override;
        double value() const override;
        bool done() const override;
    private:
        enum class Segment {
            None,
            Attack,
            Decay,
            Release
        } m_segment {};

        DescriptionAdr m_description;

        double m_value_current {};

        double m_attack_increment {};
        double m_decay_increment {};
        double m_release_increment {};
    };

    class EnvelopeAdr : public Envelope, public allocator::StaticAllocated<EnvelopeAdr, EnvelopeStorage> {
    public:
        explicit EnvelopeAdr(const DescriptionAdr& description = {})
            : m_description(description) {}

        void note_on(double time) override;
        void note_off(double time) override;
        void update(double time) override;
        double value() const override;
        bool done() const override;
    private:
        enum class Segment {
            None,
            Attack,
            Decay,
            Release
        } m_segment {};

        DescriptionAdr m_description;

        double m_time_note_on {};
        double m_time_note_off {};
        double m_time_decay {};

        double m_value_current {};
        double m_value_note_on {};
        double m_value_note_off {};
    };

    using EnvelopePtr = std::unique_ptr<Envelope>;

    enum NoteName : unsigned int {
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

    enum NoteOctave : unsigned int {
        Octave1,
        Octave2,
        Octave3,
        Octave4,
        Octave5,
        Octave6,
        Octave7
    };

    // MIDI-like note ID
    using NoteId = unsigned int;

    using InstrumentId = unsigned int;

    using InstrumentRange = std::pair<NoteId, NoteId>;

    constexpr NoteId note(NoteName name, NoteOctave octave) {
        const unsigned int base {name};
        const unsigned int multiplier {octave};

        if (base < 3) {
            return base + 12 * multiplier;
        }

        return base + 12 * (multiplier - 1);
    }

    static_assert(note(A, Octave5) == 48);
    static_assert(note(B, Octave5) == 50);
    static_assert(note(C, Octave2) == 3);

    struct Voice {
        NoteId note {};
        InstrumentId instrument {};
        EnvelopePtr envelope;  // Overall envelope
        double loudness {};
        double time_on {};
        double time_off {-std::numeric_limits<double>::infinity()};
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

        inline constexpr NoteId ID_BEGIN {0};
        inline constexpr NoteId ID_END {NOTES - 1};
        inline constexpr InstrumentRange ID_FULL_RANGE {std::make_pair(ID_BEGIN, ID_END)};
    }

    struct Instrument {
        Instrument() = default;
        virtual ~Instrument() = default;

        Instrument(const Instrument&) = default;
        Instrument& operator=(const Instrument&) = default;
        Instrument(Instrument&&) = default;
        Instrument& operator=(Instrument&&) = default;

        virtual const char* name() const = 0;
        virtual InstrumentId id() const = 0;

        virtual double sound(double time, NoteId note) const = 0;
        virtual InstrumentRange range() const { return keyboard::ID_FULL_RANGE; }

        virtual EnvelopePtr new_envelope() const = 0;
        virtual double attack_duration() const = 0;
        virtual double release_duration() const = 0;
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
    double random();
    double frequency(NoteId note);

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
        using Sample = std::unique_ptr<double[]>;

        Sample padsynth(
            std::size_t size,
            int sample_rate,
            double frequency,
            double bandwidth,
            const double* amplitude_harmonics,
            int number_harmonics
        );
    }
}
