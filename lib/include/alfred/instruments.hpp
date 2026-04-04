#pragma once

#include "alfred/synthesis.hpp"
#include "alfred/hash.hpp"

// Built in compiled instruments/presets

#define ALFRED_INSTRUMENT_STATIC_NAME_ID(NAME_STRING) \
    static consteval auto static_name() { return NAME_STRING; } \
    static consteval auto static_id() { return hash::HashedStr32(static_name()); } \
    const char* name() const override { return static_name(); } \
    syn::InstrumentId id() const override { return static_id(); }

#define ALFRED_INSTRUMENT_DESCRIPTION(DESCRIPTION_STRING) \
    const char* description() const override { return DESCRIPTION_STRING; }

#define ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED() \
    const char* description() const override { return "This instrument/preset is unfinished"; }

namespace instruments {
    class ShortSynthPiano : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Short Synth Piano")
        ALFRED_INSTRUMENT_DESCRIPTION("Very short attack, decay and release durations")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::AdrLinear>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<ShortSynthPiano>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.007,
            .duration_decay = 0.15,
            .duration_release = 0.007
        };
    };

    struct Metronome : ShortSynthPiano {
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Metronome")
        ALFRED_INSTRUMENT_DESCRIPTION("It's just a metronome")
    };

    class Ghost : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Ghost")
        ALFRED_INSTRUMENT_DESCRIPTION("Here to haunt you")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::A, syn::Octave2), syn::note(syn::C, syn::Octave6) }; }

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<Ghost>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.7,
            .duration_decay = 1.5,
            .duration_release = 1.0
        };
    };

    class Harmonica : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Harmonica")
        ALFRED_INSTRUMENT_DESCRIPTION("Isn't it easy to play a virtual harmonica?")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::A, syn::Octave3), syn::note(syn::C, syn::Octave6) }; }

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adsr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<Harmonica>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.15,
            .duration_decay = 0.02,
            .duration_release = 0.3,
            .value_sustain = 0.7
        };
    };

    class DrumBass : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Bass?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<DrumBass>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.15,
            .duration_release = 0.02
        };
    };

    class DrumSnare : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Snare?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<DrumSnare>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.2,
            .duration_release = 0.03
        };
    };

    class DrumHiHat : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Drum Hi-Hat?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<DrumHiHat>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 0.15,
            .duration_release = 0.02
        };
    };

    class SynthPiano : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Synth Piano")
        ALFRED_INSTRUMENT_DESCRIPTION("Pretend it sounds like a synthetic piano")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<SynthPiano>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.01,
            .duration_decay = 4.0,
            .duration_release = 0.7
        };
    };

    class Guitar : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Guitar?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::E, syn::Octave2), syn::note(syn::C, syn::Octave6) }; }

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<Guitar>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdr ENVELOPE {
            .duration_attack = 0.07,
            .duration_decay = 5.0,
            .duration_release = 0.3
        };
    };

    class Strings : public syn::Instrument {
    public:
        Strings();

        ALFRED_INSTRUMENT_STATIC_NAME_ID("Strings?")
        ALFRED_INSTRUMENT_DESCRIPTION_UNFINISHED()

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::Adsr>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<Strings>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.2,
            .duration_decay = 0.03,
            .duration_release = 0.8,
            .value_sustain = 0.9
        };

        static constexpr std::size_t SIZE = 262144;
        static constexpr double FREQUENCY = 261.63;

        syn::padsynth::SampleCopyable m_sample;
    };

    class Cello : public syn::Instrument {
    public:
        Cello();

        ALFRED_INSTRUMENT_STATIC_NAME_ID("Cello")
        ALFRED_INSTRUMENT_DESCRIPTION("It's close, but not quite; sounds nice though")

        double sound(double time, double time_on, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { syn::note(syn::C, syn::Octave2), syn::note(syn::A, syn::Octave5) }; }

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::AdsrLinear>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<Cello>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.28,
            .duration_decay = 0.05,
            .duration_release = 0.25,
            .value_sustain = 0.95
        };

        static constexpr std::size_t SIZE = 262144;
        static constexpr double FREQUENCY = 261.63;
        static constexpr double LFO_FREQUENCY = 8.0;
        static constexpr double LFO_DEVIATION = 0.07;

        syn::padsynth::SampleCopyable m_sample;
    };

    class EasterEgg : public syn::Instrument {
    public:
        ALFRED_INSTRUMENT_STATIC_NAME_ID("Fart")
        ALFRED_INSTRUMENT_DESCRIPTION("It sounds like it")

        double sound(double time, double, syn::NoteId note) const noexcept override;
        syn::InstrumentRange range() const override { return { 0, 3 }; }

        syn::envelope::Ptr new_overall_envelope() const override { return std::make_unique<syn::envelope::AdsrLinear>(ENVELOPE); }
        double attack_duration() const override { return ENVELOPE.duration_attack; }
        double release_duration() const override { return ENVELOPE.duration_release; }

        std::unique_ptr<Instrument> clone() const override { return std::make_unique<EasterEgg>(*this); }
    private:
        static constexpr syn::envelope::DescriptionAdsr ENVELOPE {
            .duration_attack = 0.1,
            .duration_decay = 0.0,
            .duration_release = 0.1,
            .value_sustain = 1.0
        };
    };
}
