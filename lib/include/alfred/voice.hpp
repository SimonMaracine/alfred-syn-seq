#pragma once

#include "alfred/synthesis.hpp"
#include "alfred/hash.hpp"

#define ALFRED_VOICE_STATIC_NAME_ID(NAME_STRING) \
    static consteval auto static_name() { return NAME_STRING; } \
    static consteval auto static_id() { return hash::HashedStr32(static_name()); }

namespace voice {  // TODO description
    class Metronome : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Metronome")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.007,
                0.15,
                0.007
            }
        };
    };

    class Bell : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Bell")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
        syn::VoiceRange range() const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.01,
                1.2,
                0.2
            }
        };
    };

    class Harmonica : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Harmonica")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdsr OVERALL_ENVELOPE {
            syn::EnvelopeAdsrDescription {
                0.1,
                0.02,
                0.2,
                1.0,
                0.8
            }
        };
    };

    class DrumBass : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Drum Bass")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.01,
                0.15,
                0.02
            }
        };
    };

    class DrumSnare : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Drum Snare")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.01,
                0.2,
                0.04
            }
        };
    };

    class DrumHiHat : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Drum Hi-Hat")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.01,
                0.15,
                0.02
            }
        };
    };

    class Piano : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Piano")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.01,
                5.0,
                0.2
            }
        };
    };

    class Guitar : public syn::Voice {
    public:
        ALFRED_VOICE_STATIC_NAME_ID("Guitar")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
        syn::VoiceRange range() const override;
    private:
        static constexpr syn::EnvelopeAdr OVERALL_ENVELOPE {
            syn::EnvelopeAdrDescription {
                0.01,
                5.0,
                0.2
            }
        };
    };

    class Test : public syn::Voice {
    public:
        Test();

        ALFRED_VOICE_STATIC_NAME_ID("Test")

        const char* name() const override { return static_name(); }
        syn::VoiceId id() const override { return static_id(); }
        const syn::Envelope& overall_envelope() const override { return OVERALL_ENVELOPE; }
        double sound(double time, const syn::Note& note) const override;
    private:
        static constexpr syn::EnvelopeAdsr OVERALL_ENVELOPE {
            syn::EnvelopeAdsrDescription {
                0.1,
                0.02,
                0.2,
                1.0,
                0.8
            }
        };

        syn::padsynth::Sample m_sample;
    };
}
