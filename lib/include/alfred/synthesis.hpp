#pragma once

#include <memory>
#include <array>
#include <numeric>
#include <ranges>
#include <utility>

#include "alfred/allocator.hpp"

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

    struct DescriptionAdsr {
        double time_attack {0.1};
        double time_decay {0.02};
        double time_release {0.2};

        double value_start {1.0};
        double value_sustain {0.8};
    };

    struct DescriptionAdr {
        double time_attack {0.01};
        double time_decay {1.0};
        double time_release {0.1};
    };

    class EnvelopeAdsr : public Envelope, public allocator::StaticAllocated<EnvelopeAdsr> {
    public:
        EnvelopeAdsr(const DescriptionAdsr& description = {})
            : m_description(description) {}

        double get_value(double time, double time_note_on, double time_note_off) const override;
        bool is_done(double time, double time_note_on, double time_note_off) const override;
    private:
        double ads(double life_time) const;
        double r(double time, double time_note_on, double time_note_off) const;

        DescriptionAdsr m_description;
    };

    class EnvelopeAdr : public Envelope, public allocator::StaticAllocated<EnvelopeAdr> {
    public:
        EnvelopeAdr(const DescriptionAdr& description = {})
            : m_description(description) {}

        double get_value(double time, double time_note_on, double time_note_off) const override;
        bool is_done(double time, double time_note_on, double time_note_off) const override;
    private:
        double ad(double life_time) const;
        double r(double time, double time_note_on, double time_note_off) const;

        DescriptionAdr m_description;
    };

    using EnvelopePtr = std::unique_ptr<Envelope>;

    // MIDI-like note ID
    using NoteId = unsigned int;

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

    using InstrumentId = unsigned int;

    using InstrumentRange = std::pair<NoteId, NoteId>;

    struct Voice {
        NoteId note {};
        InstrumentId instrument {};
        EnvelopePtr envelope;  // Overall envelope
        double time_on {};
        double time_off {};

        static NoteId get_note(NoteName name, NoteOctave octave);
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

        virtual double sound(double time, const Voice& voice) const = 0;
        virtual InstrumentRange range() const { return  keyboard::ID_FULL_RANGE; }

        virtual EnvelopePtr new_envelope() const = 0;
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
    double frequency(const Voice& voice);
    double time_on(double time, const Voice& voice);

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
