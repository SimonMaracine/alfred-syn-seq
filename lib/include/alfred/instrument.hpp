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

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.007,
            .time_decay = 0.15,
            .time_release = 0.007
        };
    };

    class Bell : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Bell?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;
        syn::InstrumentRange range() const override { return { 12, 51 }; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.5,
            .time_decay = 1.0,
            .time_release = 0.7
        };
    };

    class Harmonica : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Harmonica")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdsr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdsr ENVELOPE {
            .time_attack = 0.08,
            .time_decay = 0.02,
            .time_release = 0.12,
            .value_sustain = 0.7
        };
    };

    class DrumBass : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Bass")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.01,
            .time_decay = 0.15,
            .time_release = 0.02
        };
    };

    class DrumSnare : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Snare")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.01,
            .time_decay = 0.2,
            .time_release = 0.03
        };
    };

    class DrumHiHat : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Hi-Hat")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.01,
            .time_decay = 0.15,
            .time_release = 0.02
        };
    };

    class Piano : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Piano")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.01,
            .time_decay = 5.0,
            .time_release = 0.15
        };
    };

    class Guitar : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Guitar")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;
        syn::InstrumentRange range() const override { return { 7, 51 }; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .time_attack = 0.03,
            .time_decay = 4.0,
            .time_release = 0.17
        };
    };

    class Strings : public syn::Instrument {
    public:
        Strings();

        ALFRED_INSTRUMENT_STATIC_NAME_ID("Strings?")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdsr>(ENVELOPE); }
        double attack_time() const override { return ENVELOPE.time_attack; }
        double release_time() const override { return ENVELOPE.time_release; }
    private:
        static constexpr syn::DescriptionAdsr ENVELOPE {
            .time_attack = 0.12,
            .time_decay = 0.03,
            .time_release = 0.3,
            .value_sustain = 0.9
        };

        static constexpr std::size_t SIZE {262144};
        static constexpr double FREQUENCY {261.63};

        syn::padsynth::Sample m_sample;
    };
}
