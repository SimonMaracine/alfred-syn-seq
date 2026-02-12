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
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            0.007,
            0.15,
            0.007
        };
    };

    class Bell : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Bell")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;
        syn::InstrumentRange range() const override { return { 12, 51 }; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            1.01,
            1.2,
            1.2
        };
    };

    class Harmonica : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Harmonica")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdsr>(ENVELOPE); }
    private:
        static constexpr syn::EnvelopeAdsrDescription ENVELOPE {
            0.1,
            0.02,
            0.2,
            1.0,
            0.8
        };
    };

    class DrumBass : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Bass")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            0.01,
            0.15,
            0.02
        };
    };

    class DrumSnare : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Snare")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            0.01,
            0.2,
            0.04
        };
    };

    class DrumHiHat : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Hi-Hat")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            0.01,
            0.15,
            0.02
        };
    };

    class Piano : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Piano")

        const char* name() const override { return static_name(); }
        syn::InstrumentId id() const override { return static_id(); }

        double sound(double time, const syn::Voice& voice) const override;

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            0.01,
            5.0,
            0.2
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
    private:
        static constexpr syn::EnvelopeAdrDescription ENVELOPE {
            0.01,
            5.0,
            0.2
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
    private:
        static constexpr syn::EnvelopeAdsrDescription ENVELOPE {
            0.1,
            0.02,
            0.2,
            1.0,
            0.8
        };

        static constexpr std::size_t SIZE {262144};
        static constexpr double FREQUENCY {261.63};

        syn::padsynth::Sample m_sample;
    };
}
