#include "alfred/synthesis.hpp"

#include <random>
#include <algorithm>
#include <cmath>
#include <cassert>

#include "alfred/audio.hpp"
#include "alfred/math.hpp"

namespace syn {
    static constexpr double eps(double duration) {
        // Magic number comes from here: e ** -eps * t = 1 / 100
        return 4.605170186 / duration;
    }

    static constexpr double increment_of_linear(double height, double duration) {
        return height / (duration * double(audio::SAMPLE_FREQUENCY));
    }

    static double exponential(double t, double duration, double top, double bottom) {
        return (top - bottom) * std::exp(-eps(duration) * t) + bottom;
    }

    static double inverse_exponential(double t, double duration, double bottom) {
        return 1.0 - (1.0 - bottom) * std::exp(-eps(duration) * t);
    }

    static double increment_of_exponential(double t, double duration, double top = 1.0, double bottom = 0.0) {
        static constexpr double eps = 1.0 / double(audio::SAMPLE_FREQUENCY);
        return std::abs(exponential(t + eps, duration, top, bottom) - exponential(t, duration, top, bottom));
    }

    static double increment_of_inverse_exponential(double t, double duration, double bottom = 0.0) {
        static constexpr double eps = 1.0 / double(audio::SAMPLE_FREQUENCY);
        return std::abs(inverse_exponential(t + eps, duration, bottom) - inverse_exponential(t, duration, bottom));
    }

    void EnvelopeAdsrLinear::note_on(double) {
        m_segment = Segment::Attack;

        m_attack_increment = increment_of_linear(1.0, m_description.duration_attack);
        m_decay_increment = increment_of_linear(1.0 - m_description.value_sustain, m_description.duration_decay);
    }

    void EnvelopeAdsrLinear::note_off(double) {
        m_segment = Segment::Release;

        m_release_increment = increment_of_linear(m_description.value_sustain, m_description.duration_release);
    }

    void EnvelopeAdsrLinear::update(double) {
        switch (m_segment) {
            case Segment::None:
                break;
            case Segment::Attack:
                m_value_current += m_attack_increment;

                if (m_value_current >= 1.0) {
                    m_value_current = 1.0;
                    m_segment = Segment::Decay;
                }

                break;
            case Segment::Decay:
                m_value_current -= m_decay_increment;

                if (m_value_current <= m_description.value_sustain) {
                    m_value_current = m_description.value_sustain;
                    m_segment = Segment::Sustain;
                }

                break;
            case Segment::Sustain:
                m_value_current = m_description.value_sustain;

                break;
            case Segment::Release:
                m_value_current -= m_release_increment;

                if (m_value_current <= 0.0) {
                    m_value_current = 0.0;
                    m_segment = Segment::None;
                }

                break;
        }
    }

    double EnvelopeAdsrLinear::value() const {
        return math::clamp(m_value_current);
    }

    bool EnvelopeAdsrLinear::done() const {
        return m_segment == Segment::None;
    }

    void EnvelopeAdsr::note_on(double time) {
        m_segment = Segment::Attack;

        m_time_note_on = time;
        m_value_note_on = m_value_current;
    }

    void EnvelopeAdsr::note_off(double time) {
        m_segment = Segment::Release;

        m_time_note_off = time;
        m_value_note_off = m_value_current;
    }

    void EnvelopeAdsr::update(double time) {
        switch (m_segment) {
            case Segment::None:
                break;
            case Segment::Attack:
                m_value_current += math::clamp_min(
                    increment_of_inverse_exponential(time - m_time_note_on, m_description.duration_attack, m_value_note_on)
                );

                if (m_value_current >= 1.0) {
                    m_value_current = 1.0;
                    m_time_decay = time;
                    m_segment = Segment::Decay;
                }

                break;
            case Segment::Decay:
                m_value_current -= math::clamp_min(
                    increment_of_exponential(time - m_time_decay, m_description.duration_decay, 1.0, m_description.value_sustain)
                );

                if (m_value_current <= m_description.value_sustain) {
                    m_value_current = m_description.value_sustain;
                    m_segment = Segment::Sustain;
                }

                break;
            case Segment::Sustain:
                m_value_current = m_description.value_sustain;

                break;
            case Segment::Release:
                m_value_current -= math::clamp_min(
                    increment_of_exponential(time - m_time_note_off, m_description.duration_release, m_value_note_off)
                );

                if (m_value_current <= 0.0) {
                    m_value_current = 0.0;
                    m_segment = Segment::None;
                }

                break;
        }
    }

    double EnvelopeAdsr::value() const {
        return math::clamp(m_value_current);
    }

    bool EnvelopeAdsr::done() const {
        return m_segment == Segment::None;
    }

    void EnvelopeAdrLinear::note_on(double) {
        m_segment = Segment::Attack;

        m_attack_increment = increment_of_linear(1.0, m_description.duration_attack);
        m_decay_increment = increment_of_linear(1.0, m_description.duration_decay);
    }

    void EnvelopeAdrLinear::note_off(double) {
        m_segment = Segment::Release;

        m_release_increment = increment_of_linear(1.0, m_description.duration_release);
    }

    void EnvelopeAdrLinear::update(double) {
        switch (m_segment) {
            case Segment::None:
                break;
            case Segment::Attack:
                m_value_current += m_attack_increment;

                if (m_value_current >= 1.0) {
                    m_value_current = 1.0;
                    m_segment = Segment::Decay;
                }

                break;
            case Segment::Decay:
                m_value_current -= m_decay_increment;

                if (m_value_current <= 0.0) {
                    m_value_current = 0.0;
                    m_segment = Segment::None;
                }

                break;
            case Segment::Release:
                m_value_current -= m_release_increment;

                if (m_value_current <= 0.0) {
                    m_value_current = 0.0;
                    m_segment = Segment::None;
                }

                break;
        }
    }

    double EnvelopeAdrLinear::value() const {
        return math::clamp(m_value_current);
    }

    bool EnvelopeAdrLinear::done() const {
        return m_segment == Segment::None;
    }

    void EnvelopeAdr::note_on(double time) {
        m_segment = Segment::Attack;

        m_time_note_on = time;
        m_value_note_on = m_value_current;
    }

    void EnvelopeAdr::note_off(double time) {
        m_segment = Segment::Release;

        m_time_note_off = time;
        m_value_note_off = m_value_current;
    }

    void EnvelopeAdr::update(double time) {
        switch (m_segment) {
            case Segment::None:
                break;
            case Segment::Attack: {
                m_value_current += math::clamp_min(
                    increment_of_inverse_exponential(time - m_time_note_on, m_description.duration_attack, m_value_note_on)
                );

                if (m_value_current >= 1.0) {
                    m_value_current = 1.0;
                    m_time_decay = time;
                    m_segment = Segment::Decay;
                }

                break;
            }
            case Segment::Decay:
                m_value_current -= math::clamp_min(
                    increment_of_exponential(time - m_time_decay, m_description.duration_decay)
                );

                if (m_value_current <= 0.0) {
                    m_value_current = 0.0;
                    m_segment = Segment::None;
                }

                break;
            case Segment::Release:
                m_value_current -= math::clamp_min(
                    increment_of_exponential(time - m_time_note_off, m_description.duration_release, m_value_note_off)
                );

                if (m_value_current <= 0.0) {
                    m_value_current = 0.0;
                    m_segment = Segment::None;
                }

                break;
        }
    }

    double EnvelopeAdr::value() const {
        return math::clamp(m_value_current);
    }

    bool EnvelopeAdr::done() const {
        return m_segment == Segment::None;
    }

    namespace oscillator {
        double sine(double time, double frequency) {
            return std::sin(math::w(frequency) * time);
        }

        double sine(double time, double frequency, LowFrequencyOscillator lfo) {
            return std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo));
        }

        double square(double time, double frequency) {
            const double value = std::sin(math::w(frequency) * time);

            return value >= 0.0 ? 1.0 : -1.0;
        }

        double square(double time, double frequency, LowFrequencyOscillator lfo) {
            const double value = std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo));

            return value >= 0.0 ? 1.0 : -1.0;
        }

        double triangle(double time, double frequency) {
            return 2.0 * std::asin(std::sin(math::w(frequency) * time)) / math::PI;
        }

        double triangle(double time, double frequency, LowFrequencyOscillator lfo) {
            return 2.0 * std::asin(std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))) / math::PI;
        }

        double sawtooth(double time, double frequency) {
            double result {};

            for (double n = 1.0; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time) / n;
            }

            return 4.0 * result / (10.0 * math::PI);  // FIXME ...
        }

        double sawtooth(double time, double frequency, LowFrequencyOscillator lfo) {
            double result {};

            for (double n = 1.0; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time + frequency_modulation(time, frequency, lfo)) / n;
            }

            return 4.0 * result / (10.0 * math::PI);  // FIXME ...
        }
    }

    double frequency_modulation(double time, double frequency, LowFrequencyOscillator lfo) {
        return lfo.deviation * frequency * std::sin(math::w(lfo.frequency) * time);
    }

    thread_local struct {
        std::mt19937_64 engine;
        std::uniform_real_distribution<> distribution_noise {-1.0, 1.0};
        std::uniform_real_distribution<> distribution_random {0.0, 1.0};
    } g_random;

    double noise() {
        return g_random.distribution_noise(g_random.engine);
    }

    double random() {
        return g_random.distribution_random(g_random.engine);
    }

    double frequency(NoteId note) {
        static constexpr double BASE_FREQUENCY = 27.5;  // A0
        static constexpr double STEP_FREQUENCY = 1.059463094;  // 2.0 ** (1.0 / 12.0)

        return BASE_FREQUENCY * std::pow(STEP_FREQUENCY, double(note));
    }

    namespace util {
        double sound(double time, NoteId note, const double* sample, std::size_t size, double frequency) {
            const double sample_duration = double(size) / double(audio::SAMPLE_FREQUENCY);

            const double pitch = syn::frequency(note) / frequency;
            double _;
            const double part = std::modf(time * pitch / sample_duration, &_);
            const auto index = std::size_t(part * double(size));

            return sample[index];
        }
    }

    // https://zynaddsubfx.sourceforge.io/doc/PADsynth/PADsynth.htm

    namespace padsynth {
        static double default_profile(double frequency, double bandwidth) {
            const double x = frequency / bandwidth;
            return std::exp(-x * x) / bandwidth;
        }

        static void inverse_ft(std::size_t size, const double* frequency_amplitudes, const double* frequency_phases, double* sample) {
            math::ft::InverseTransform transform {size};
            math::ft::Frequencies frequencies {size / 2};

            for (std::size_t i {}; i < size / 2; i++) {
                frequencies.cosine()[i] = frequency_amplitudes[i] * std::cos(frequency_phases[i]);
                frequencies.sine()[i] = frequency_amplitudes[i] * std::sin(frequency_phases[i]);
            }

            transform.frequencies_to_samples(frequencies, sample);
        }

        Sample padsynth(
            std::size_t size,
            int sample_rate,
            double frequency,
            double bandwidth,
            const double* amplitude_harmonics,
            int number_harmonics,
            Profile profile
        ) {
            if (!profile) {
                profile = default_profile;
            }

            auto sample = std::make_unique<double[]>(size);
            auto frequency_amplitudes = std::make_unique<double[]>(size / 2);
            auto frequency_phases = std::make_unique<double[]>(size / 2);

            for (int harmonic = 1; harmonic < number_harmonics; harmonic++) {
                const double harmonic_bandwidth =
                    (std::exp2(bandwidth / 1200.0) - 1.0) * frequency * double(harmonic);

                const double bandwidth_i = harmonic_bandwidth / (2.0 * double(sample_rate));
                const double frequency_i = frequency * double(harmonic) / double(sample_rate);

                for (std::size_t i {}; i < size / 2; i++) {
                    const double harmonic_profile = profile(double(i) / double(size) - frequency_i, bandwidth_i);
                    frequency_amplitudes[i] += harmonic_profile * amplitude_harmonics[harmonic];
                }
            }

            for (std::size_t i {}; i < size / 2; i++) {
                frequency_phases[i] = math::w(random());
            }

            inverse_ft(size, frequency_amplitudes.get(), frequency_phases.get(), sample.get());
            math::normalize(sample.get(), size);

            return sample;
        }
    }
}
