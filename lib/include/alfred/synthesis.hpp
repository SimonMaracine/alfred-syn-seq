#pragma once

#include <tuple>
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

    enum Voice : unsigned int {
        VoiceMetronome,
        VoiceBell,
        VoiceHarmonica,
        VoiceDrumBass,
        VoiceDrumSnare,
        VoiceDrumHiHat,
        VoicePiano,
        VoiceGuitar
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

    struct Note {
        Id id {};
        Voice voice {};
        double time_on {};
        double time_off {};

        static Id get_id(Name name, Octave octave);
    };

    struct Instrument {
        Instrument() = default;
        virtual ~Instrument() = default;

        Instrument(const Instrument&) = default;
        Instrument& operator=(const Instrument&) = default;
        Instrument(Instrument&&) = default;
        Instrument& operator=(Instrument&&) = default;

        virtual const char* name() const = 0;
        virtual Voice voice() const = 0;
        virtual const Envelope& envelope_amplitude() const = 0;
        virtual double sound(double time, const Note& note) const = 0;
        virtual double amplitude() const { return 0.5; }
        virtual std::pair<Id, Id> range() const { return std::make_pair(keyboard::ID_BEGIN, keyboard::ID_END); }
    };

    namespace instruments {  // TODO description
        class Metronome : public Instrument {
        public:
            const char* name() const override { return "Metronome"; }
            Voice voice() const override { return VoiceMetronome; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.007,
                0.15,
                0.007
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };

        class Bell : public Instrument {
        public:
            const char* name() const override { return "Bell"; }
            Voice voice() const override { return VoiceBell; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
            std::pair<Id, Id> range() const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                1.2,
                0.2
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };

        class Harmonica : public Instrument {
        public:
            const char* name() const override { return "Harmonica"; }
            Voice voice() const override { return VoiceHarmonica; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdsrDescription ENVELOPE {
                0.1,
                0.02,
                0.2,
                1.0,
                0.8
            };

            static constexpr EnvelopeAdsr m_envelope_amplitude {ENVELOPE};
        };

        class DrumBass : public Instrument {
        public:
            const char* name() const override { return "Drum Bass"; }
            Voice voice() const override { return VoiceDrumBass; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                0.15,
                0.02
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };

        class DrumSnare : public Instrument {
        public:
            const char* name() const override { return "Drum Snare"; }
            Voice voice() const override { return VoiceDrumSnare; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                0.2,
                0.04
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };

        class DrumHiHat : public Instrument {
        public:
            const char* name() const override { return "Drum Hi-Hat"; }
            Voice voice() const override { return VoiceDrumHiHat; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                0.15,
                0.02
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };

        class Piano : public Instrument {
        public:
            const char* name() const override { return "Piano"; }
            Voice voice() const override { return VoicePiano; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                5.0,
                0.2
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };

        class Guitar : public Instrument {
        public:
            const char* name() const override { return "Guitar"; }
            Voice voice() const override { return VoiceGuitar; }
            const Envelope& envelope_amplitude() const override { return m_envelope_amplitude; }

            double sound(double time, const Note& note) const override;
            std::pair<Id, Id> range() const override;
        private:
            static constexpr EnvelopeAdrDescription ENVELOPE {
                0.01,
                5.0,
                0.2
            };

            static constexpr EnvelopeAdr m_envelope_amplitude {ENVELOPE};
        };
    }

    class Voices {
    public:
        using Storage = std::tuple<
            instruments::Metronome,
            instruments::Bell,
            instruments::Harmonica,
            instruments::DrumBass,
            instruments::DrumSnare,
            instruments::DrumHiHat,
            instruments::Piano,
            instruments::Guitar
        >;

        const Storage& get() const { return m_storage; }

        const Instrument& operator[](Voice voice_index) const {
            switch (voice_index) {
                case 0: return std::get<0>(m_storage);
                case 1: return std::get<1>(m_storage);
                case 2: return std::get<2>(m_storage);
                case 3: return std::get<3>(m_storage);
                case 4: return std::get<4>(m_storage);
                case 5: return std::get<5>(m_storage);
                case 6: return std::get<6>(m_storage);
                case 7: return std::get<7>(m_storage);
            }

            std::unreachable();
        }
    private:
        Storage m_storage;
    };
}
