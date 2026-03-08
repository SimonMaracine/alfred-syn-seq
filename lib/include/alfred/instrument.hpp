#pragma once

#include "alfred/synthesis.hpp"
#include "alfred/hash.hpp"

#define ALFRED_INSTRUMENT_STATIC_NAME_ID(NAME_STRING) \
    static consteval auto static_name() { return NAME_STRING; } \
    static consteval auto static_id() { return hash::HashedStr32(static_name()); } \
    const char* name() const override { return static_name(); } \
    syn::InstrumentId id() const override { return static_id(); }

#define ALFRED_INSTRUMENT_DESCRIPTION(DESCRIPTION_STRING) \
    const char* description() const override { return DESCRIPTION_STRING; }

#define ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED() \
    const char* description() const override { return "This instrument/preset is unfinished"; }

namespace instrument {
    class Metronome : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Metronome")
        ALFRED_INSTRUMENT_DESCRIPTION("Simply a metronome")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdrLinear>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.007,
            .duration_decay = 0.15,
            .duration_release = 0.007
        };

        syn::Volume m_volume {};
    };

    class Bell : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Bell?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::C, syn::Octave2), syn::note(syn::C, syn::Octave6) }; }

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.5,
            .duration_decay = 1.0,
            .duration_release = 0.7
        };

        syn::Volume m_volume {};
    };

    class Harmonica : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Harmonica?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

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

        syn::Volume m_volume {};
    };

    class DrumBass : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Bass?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.15,
            .duration_release = 0.02
        };

        syn::Volume m_volume {};
    };

    class DrumSnare : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Snare?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.2,
            .duration_release = 0.03
        };

        syn::Volume m_volume {};
    };

    class DrumHiHat : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Hi-Hat?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.15,
            .duration_release = 0.02
        };

        syn::Volume m_volume {};
    };

    class SynthPiano : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Synth Piano")
        ALFRED_INSTRUMENT_DESCRIPTION("Pretend it sounds like a synthetic piano")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 4.0,
            .duration_release = 0.7
        };

        syn::Volume m_volume {};
    };

    class Guitar : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Guitar?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::E, syn::Octave2), syn::note(syn::C, syn::Octave6) }; }

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdr ENVELOPE {
            .duration_attack = 0.07,
            .duration_decay = 5.0,
            .duration_release = 0.3
        };

        syn::Volume m_volume {};
    };

    class Strings : public syn::Instrument {
    public:
        Strings();

        ALFRED_INSTRUMENT_STATIC_NAME_ID("Strings?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

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
        syn::Volume m_volume {};
    };

    class Cello : public syn::Instrument {
    public:
        Cello();

        ALFRED_INSTRUMENT_STATIC_NAME_ID("Cello")
        ALFRED_INSTRUMENT_DESCRIPTION("It's close, but not quite; sounds nice though")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::C, syn::Octave2), syn::note(syn::A, syn::Octave5) }; }

        syn::Volume volume() const override { return m_volume; }
        void volume(syn::Volume volume) override { m_volume = volume; }

        syn::EnvelopePtr new_envelope() const override { return std::make_unique<syn::EnvelopeAdsrLinear>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }
    private:
        static constexpr syn::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.28,
            .duration_decay = 0.05,
            .duration_release = 0.25,
            .value_sustain = 0.95
        };

        static constexpr std::size_t SIZE {262144};
        static constexpr double FREQUENCY {261.63};
        static constexpr double LFO_FREQUENCY {8.0};
        static constexpr double LFO_DEVIATION {0.07};

        syn::padsynth::Sample m_sample;
        syn::Volume m_volume {};
    };
}
