#pragma once

struct Envelope {
    virtual ~Envelope() = default;

    virtual double get_amplitude(double time) const = 0;
    virtual void trigger_on(double time) = 0;
    virtual void trigger_off(double time) = 0;
};

struct EnvelopeAdsrDescription {
    double attack_time {0.1};
    double decay_time {0.01};
    double release_time {0.2};

    double start_amplitude {1.0};
    double sustain_amplitude {0.8};
};

class EnvelopeAdsr : public Envelope {
public:
    EnvelopeAdsr(const EnvelopeAdsrDescription& description)
        : m_description(description) {}

    double get_amplitude(double time) const override;
    void trigger_on(double time) override;
    void trigger_off(double time) override;
private:
    EnvelopeAdsrDescription m_description;

    double m_trigger_on_time {};
    double m_trigger_off_time {};

    bool m_trigger_on {};
};

struct EnvelopeAdDescription {
    double attack_time {0.01};
    double decay_time {1.0};
};

class EnvelopeAd : public Envelope {
public:
    EnvelopeAd(const EnvelopeAdDescription& description)
        : m_description(description) {}

    double get_amplitude(double time) const override;
    void trigger_on(double time) override;
    void trigger_off(double) override {}
private:
    EnvelopeAdDescription m_description;

    double m_trigger_on_time {};
};

struct Instrument {
    virtual ~Instrument() = default;

    virtual Envelope& get_envelope() = 0;
    virtual const Envelope& get_envelope() const = 0;
    virtual double sound(double time, double frequency) const = 0;
};

namespace instruments {
    class Bell : public Instrument {
    public:
        Envelope& get_envelope() override {
            return m_envelope;
        }

        const Envelope& get_envelope() const override {
            return m_envelope;
        }

        double sound(double time, double frequency) const override;
    private:
        static constexpr EnvelopeAdDescription ENVELOPE {
            0.01,
            1.2
        };

        EnvelopeAd m_envelope {ENVELOPE};
    };

    class Harmonica : public Instrument {
    public:
        Envelope& get_envelope() override {
            return m_envelope;
        }

        const Envelope& get_envelope() const override {
            return m_envelope;
        }

        double sound(double time, double frequency) const override;
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
