#pragma once

#include <tuple>
#include <utility>

namespace syn {
    struct Envelope {
        virtual ~Envelope() = default;

        virtual double get_amplitude(double time, double time_note_on, double time_note_off) const = 0;
        virtual bool is_done(double time, double time_note_on, double time_note_off) const = 0;
    };

    struct EnvelopeAdsrDescription {
        double time_attack {0.1};
        double time_decay {0.02};
        double time_release {0.2};

        double amplitude_start {1.0};
        double amplitude_sustain {0.8};
    };

    class EnvelopeAdsr : public Envelope {
    public:
        EnvelopeAdsr(const EnvelopeAdsrDescription& description)
            : m_description(description) {}

        double get_amplitude(double time, double time_note_on, double time_note_off) const override;
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
        EnvelopeAdr(const EnvelopeAdrDescription& description)
            : m_description(description) {}

        double get_amplitude(double time, double time_note_on, double time_note_off) const override;
        bool is_done(double time, double time_note_on, double time_note_off) const override;
    private:
        double ad(double life_time) const;
        double r(double time, double time_note_on, double time_note_off) const;

        EnvelopeAdrDescription m_description;
    };

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
        Gs,
        A2,
        As2,
        B2,
        C2
    };

    enum Octave : unsigned int {
        Octave0,
        Octave1,
        Octave2
    };

    enum Voice : unsigned int {
        VoiceMetronome,
        VoiceBell,
        VoiceHarmonica,
        VoiceDrumKick
    };

    struct Note {
        Name name {};
        Octave octave {};
        Voice voice {};
        double time_on {};
        double time_off {};
    };

    struct Instrument {
        virtual ~Instrument() = default;

        virtual const Envelope& get_envelope() const = 0;
        virtual double sound(double time, const Note& note) const = 0;
    };

    namespace instruments {  // TODO note range, name, description
        class Metronome : public Instrument {
        public:
            const Envelope& get_envelope() const override { return m_envelope; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                0.15,
                0.01
            };

            EnvelopeAdr m_envelope {ENVELOPE};
        };

        class Bell : public Instrument {
        public:
            const Envelope& get_envelope() const override { return m_envelope; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                1.2,
                0.2
            };

            EnvelopeAdr m_envelope {ENVELOPE};
        };

        class Harmonica : public Instrument {
        public:
            const Envelope& get_envelope() const override { return m_envelope; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdsrDescription ENVELOPE {
                0.1,
                0.02,
                0.2,
                1.0,
                0.8
            };

            EnvelopeAdsr m_envelope {ENVELOPE};
        };

        class DrumKick : public Instrument {
        public:
            const Envelope& get_envelope() const override { return m_envelope; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                0.15,
                0.01
            };

            EnvelopeAdr m_envelope {ENVELOPE};
        };
    }

    struct Voices : private std::tuple<
        instruments::Metronome,
        instruments::Bell,
        instruments::Harmonica,
        instruments::DrumKick
    > {
        const Instrument& operator[](Voice voice_index) const {
            switch (voice_index) {
                case 0: return std::get<0>(*this);
                case 1: return std::get<1>(*this);
                case 2: return std::get<2>(*this);
                case 3: return std::get<3>(*this);
            }

            std::unreachable();
        }
    };
}
