#pragma once

struct Envelope {
    virtual ~Envelope() = default;

    virtual double get_amplitude(double time, double time_note_on, double time_note_off) const = 0;
    virtual bool is_done(double time, double time_note_on, double time_note_off) const = 0;
};

struct EnvelopeAdsrDescription {
    double time_attack {0.1};
    double time_decay {0.01};
    double time_release {0.2};

    double amplitude_start {1.0};
    double amplitude_sustain {0.8};
};

class EnvelopeAdsr : public Envelope {  // TODO figure out the logic by making a sketch
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

struct EnvelopeAdDescription {
    double time_attack {0.01};
    double time_decay {1.0};
};

class EnvelopeAd : public Envelope {  // TODO turn into ADR
public:
    EnvelopeAd(const EnvelopeAdDescription& description)
        : m_description(description) {}

    double get_amplitude(double time, double time_note_on, double time_note_off) const override;
    bool is_done(double time, double time_note_on, double time_note_off) const override;
private:
    EnvelopeAdDescription m_description;
};

enum Note : unsigned int {
    A,
    AS,
    B,
    C,
    CS,
    D,
    DS,
    E,
    F,
    FS,
    G,
    GS,
    A2,
    AS2,
    B2,
    C2
};

struct Sound {
    Note note {};
    unsigned int octave {};  // TODO
    unsigned int voice {};
    double time_on {};
    double time_off {};
};

struct Instrument {
    virtual ~Instrument() = default;

    virtual const Envelope& get_envelope() const = 0;
    virtual double sound(double time, const Sound& sound) const = 0;
};

namespace instruments {
    class Bell : public Instrument {
    public:
        const Envelope& get_envelope() const override { return m_envelope; }

        double sound(double time, const Sound& sound) const override;
    private:
        static constexpr EnvelopeAdDescription ENVELOPE {
            0.01,
            1.2
        };

        EnvelopeAd m_envelope {ENVELOPE};
    };

    class Harmonica : public Instrument {
    public:
        const Envelope& get_envelope() const override { return m_envelope; }

        double sound(double time, const Sound& sound) const override;
    private:
        static constexpr EnvelopeAdsrDescription ENVELOPE {
            0.1,
            0.01,
            0.2,
            1.0,
            0.8
        };

        EnvelopeAdsr m_envelope {ENVELOPE};
    };
};
