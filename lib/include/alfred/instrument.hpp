#pragma once

#include "alfred/synthesis.hpp"
#include "alfred/hash.hpp"

#define ALFRED_INSTRUMENT_STATIC_NAME_ID(NAME_STRING) \
    static consteval auto static_name() { return NAME_STRING; } \
    static consteval auto static_id() { return hash::HashedStr32(static_name()); }

namespace instrument {  // TODO description
    class Metronome : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Metronome")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdrLinear>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.007,
            .duration_decay = 0.15,
            .duration_release = 0.007
        };
    };

    class Bell : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Bell?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;
        syn::InstrumentRange range() const override { return { 12, 51 }; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.5,
            .duration_decay = 1.0,
            .duration_release = 0.7
        };
    };

    class Harmonica : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Harmonica?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdsr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.15,
            .duration_decay = 0.02,
            .duration_release = 0.6,
            .value_sustain = 0.7
        };
    };

    class DrumBass : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Bass?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.15,
            .duration_release = 0.02
        };
    };

    class DrumSnare : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Snare?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.2,
            .duration_release = 0.03
        };
    };

    class DrumHiHat : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Hi-Hat?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.15,
            .duration_release = 0.02
        };
    };

    class Piano : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Piano")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 4.0,
            .duration_release = 0.7
        };
    };

    class Guitar : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Guitar?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;
        syn::InstrumentRange range() const override { return { 7, 51 }; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.07,
            .duration_decay = 5.0,
            .duration_release = 0.3
        };
    };

    class Strings : public syn::Instrument {
    public:
        Strings();

        ALFRED_INSTRUMENT_STATIC_NAME_ID("Strings?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, double time_on, syn::NoteId note) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdsr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.2,
            .duration_decay = 0.03,
            .duration_release = 0.8,
            .value_sustain = 0.9
        };

        static constexpr std::size_t SIZE {262144};
        static constexpr double FREQUENCY {261.63};

        syn::padsynth::Sample m_sample;
    };
}
