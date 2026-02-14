#include "alfred/synthesis.hpp"

#include <random>
#include <algorithm>
#include <cmath>
#include <cassert>

#include "alfred/math.hpp"
#include "alfred/audio.hpp"

namespace syn {
    void EnvelopeAdsr::note_on() {
        m_segment = Segment::Attack;

        m_attack_increment = 1.0 / (m_description.time_attack * audio::SAMPLE_FREQUENCY);
        m_decay_increment = (1.0 - m_description.value_sustain) / (m_description.time_decay * audio::SAMPLE_FREQUENCY);
    }

    void EnvelopeAdsr::note_off() {
        m_segment = Segment::Release;

        m_release_increment = m_current_value / (m_description.time_release * audio::SAMPLE_FREQUENCY);
    }

    void EnvelopeAdsr::update() {
        switch (m_segment) {
            case Segment::None:
                break;
            case Segment::Attack:
                m_current_value += m_attack_increment;

                if (m_current_value >= 1.0) {
                    m_current_value = 1.0;
                    m_segment = Segment::Decay;
                }

                break;
            case Segment::Decay:
                m_current_value -= m_decay_increment;

                if (m_current_value <= m_description.value_sustain) {
                    m_current_value = m_description.value_sustain;
                    m_segment = Segment::Sustain;
                }

                break;
            case Segment::Sustain:
                m_current_value = m_description.value_sustain;

                break;
            case Segment::Release:
                m_current_value -= m_release_increment;

                if (math::zero_if_less_than_eps(m_current_value) <= 0.0) {
                    m_current_value = 0.0;
                    m_segment = Segment::None;
                }

                break;
        }
    }

    double EnvelopeAdsr::value() const {
        return math::zero_if_less_than_eps(m_current_value);
    }

    bool EnvelopeAdsr::done() const {
        return m_segment == Segment::None;
    }

    void EnvelopeAdr::note_on() {
        m_segment = Segment::Attack;

        m_attack_increment = 1.0 / (m_description.time_attack * audio::SAMPLE_FREQUENCY);
        m_decay_increment = 1.0 / (m_description.time_decay * audio::SAMPLE_FREQUENCY);
    }

    void EnvelopeAdr::note_off() {
        m_segment = Segment::Release;

        m_release_increment = m_current_value / (m_description.time_release * audio::SAMPLE_FREQUENCY);
    }

    void EnvelopeAdr::update() {
        switch (m_segment) {
            case Segment::None:
                break;
            case Segment::Attack:
                m_current_value += m_attack_increment;

                if (m_current_value >= 1.0) {
                    m_current_value = 1.0;
                    m_segment = Segment::Decay;
                }

                break;
            case Segment::Decay:
                m_current_value -= m_decay_increment;

                if (math::zero_if_less_than_eps(m_current_value) <= 0.0) {
                    m_current_value = 0.0;
                    m_segment = Segment::None;
                }

                break;
            case Segment::Release:
                m_current_value -= m_release_increment;

                if (math::zero_if_less_than_eps(m_current_value) <= 0.0) {
                    m_current_value = 0.0;
                    m_segment = Segment::None;
                }

                break;
        }
    }

    double EnvelopeAdr::value() const {
        return math::zero_if_less_than_eps(m_current_value);
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
            const double value {std::sin(math::w(frequency) * time)};

            return value >= 0.0 ? 1.0 : -1.0;
        }

        double square(double time, double frequency, LowFrequencyOscillator lfo) {
            const double value {std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))};

            return value >= 0.0 ? 1.0 : -1.0;
        }

        double triangle(double time, double frequency) {
            return std::asin(std::sin(math::w(frequency) * time)) * (2.0 / math::PI);
        }

        double triangle(double time, double frequency, LowFrequencyOscillator lfo) {
            return std::asin(std::sin(math::w(frequency) * time + frequency_modulation(time, frequency, lfo))) * (2.0 / math::PI);
        }

        double sawtooth(double time, double frequency) {
            double result {};

            for (double n {1.0}; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time) / n;
            }

            return result * (2.0 / math::PI);
        }

        double sawtooth(double time, double frequency, LowFrequencyOscillator lfo) {
            double result {};

            for (double n {1.0}; n < 10.0; n++) {
                result += std::sin(n * math::w(frequency) * time + frequency_modulation(time, frequency, lfo)) / n;
            }

            return result * (2.0 / math::PI);
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
        static constexpr double BASE_FREQUENCY {55.0};  // A1
        static constexpr double STEP_FREQUENCY {1.059463094};  // 2.0 ** (1.0 / 12.0)

        return BASE_FREQUENCY * std::pow(STEP_FREQUENCY, double(note));
    }

    double frequency(const Voice& voice) {
        return frequency(voice.note);
    }

    // https://zynaddsubfx.sourceforge.io/doc/PADsynth/PADsynth.htm

    namespace padsynth {
        static double profile(double frequency, double bandwidth) {  // TODO make customizable
            const double x {frequency / bandwidth};
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

        static void normalize(double* sample, std::size_t size) {
            double max {};

            for (std::size_t i {}; i < size; i++) {
                max = std::max(max, std::abs(sample[i]));
            }

            max = std::max(max, 1.0e-5);

            for (std::size_t i {}; i < size; i++) {
                sample[i] /= max;
            }
        }

        Sample padsynth(
            std::size_t size,
            int sample_rate,
            double frequency,
            double bandwidth,
            const double* amplitude_harmonics,
            int number_harmonics
        ) {
            auto sample {std::make_unique<double[]>(size)};
            auto frequency_amplitudes {std::make_unique<double[]>(size / 2)};
            auto frequency_phases {std::make_unique<double[]>(size / 2)};

            for (int harmonic {1}; harmonic < number_harmonics; harmonic++) {
                const double harmonic_bandwidth {
                    (std::pow(2.0, bandwidth / 1200.0) - 1.0) * frequency * double(harmonic)
                };

                const double bandwidth_i {harmonic_bandwidth / (2.0 * double(sample_rate))};
                const double frequency_i {frequency * double(harmonic) / double(sample_rate)};

                for (std::size_t i {}; i < size / 2; i++) {
                    const double harmonic_profile {profile(double(i) / double(size) - frequency_i, bandwidth_i)};
                    frequency_amplitudes[i] += harmonic_profile * amplitude_harmonics[harmonic];
                }
            }

            for (std::size_t i {}; i < size / 2; i++) {
                frequency_phases[i] = math::w(random());
            }

            inverse_ft(size, frequency_amplitudes.get(), frequency_phases.get(), sample.get());
            normalize(sample.get(), size);

            return sample;
        }
    }
}
