#include "synthesis.hpp"

#include <cmath>
#include <random>

#include "math.hpp"

namespace oscillators {
    enum class Wave {
        Sine,
        Square,
        Triangle,
        Saw
    };

    struct LowFrequencyOscillator {
        double frequency {};
        double amplitude {};
    };

    static double frequency_modulation(double time, double frequency, LowFrequencyOscillator lfo) {
        return lfo.amplitude * frequency * std::sin(w(lfo.frequency) * time);
    }

    static double wave_sine(double time, double frequency, LowFrequencyOscillator lfo) {
        return std::sin(w(frequency) * time + frequency_modulation(time, frequency, lfo));
    }

    static double wave_square(double time, double frequency, LowFrequencyOscillator lfo) {
        const double value {std::sin(w(frequency) * time + frequency_modulation(time, frequency, lfo))};

        if (value >= 0.0) {
            return 1.0;
        } else {
            return -1.0;
        }
    }

    static double wave_triangle(double time, double frequency, LowFrequencyOscillator lfo) {
        return std::asin(std::sin(w(frequency) * time + frequency_modulation(time, frequency, lfo))) * (2.0 / PI);
    }

    static double wave_saw(double time, double frequency, LowFrequencyOscillator lfo) {
        double result {};

        for (double n {1.0}; n < 10.0; n++) {
            result += std::sin(n * w(frequency) * time + frequency_modulation(time, frequency, lfo)) / n;
        }

        return result * (2.0 / PI);
    }

    static double noise() {
        static thread_local std::mt19937_64 s_random;

        std::uniform_real_distribution distribution {-1.0, 1.0};

        return distribution(s_random);
    }
}

double EnvelopeAdsr::get_amplitude(double time) const {
    double result {};

    const double life_time {time - m_trigger_on_time};

    if (m_trigger_on) {
        // Attack
        if (life_time <= m_description.attack_time) {
            result = life_time / m_description.attack_time * m_description.start_amplitude;
        }

        // Decay
        if (life_time > m_description.attack_time && life_time <= m_description.attack_time + m_description.decay_time) {
            result = ((life_time - m_description.attack_time) / m_description.decay_time) * (m_description.sustain_amplitude - m_description.start_amplitude) + m_description.start_amplitude;
        }

        // Sustain
        if (life_time > m_description.attack_time + m_description.decay_time) {
            result = m_description.sustain_amplitude;
        }
    } else {
        // Release
        result = ((time - m_trigger_off_time) / m_description.release_time) * -m_description.sustain_amplitude + m_description.sustain_amplitude;
    }

    if (result <= 0.0001) {
        result = 0.0;
    }

    return result;
}

void EnvelopeAdsr::trigger_on(double time) {
    m_trigger_on_time = time;
    m_trigger_on = true;
}

void EnvelopeAdsr::trigger_off(double time) {
    m_trigger_off_time = time;
    m_trigger_on = false;
}

double EnvelopeAd::get_amplitude(double time) const {
    double result {};

    const double life_time {time - m_trigger_on_time};

    // Attack
    if (life_time <= m_description.attack_time) {
        result = life_time / m_description.attack_time;
    }

    // Decay
    if (life_time > m_description.attack_time && life_time <= m_description.attack_time + m_description.decay_time) {
        result = (-(life_time - m_description.attack_time) / m_description.decay_time) + 1.0;
    }

    if (result <= 0.0001) {
        result = 0.0;
    }

    return result;
}

void EnvelopeAd::trigger_on(double time) {
    m_trigger_on_time = time;
}

namespace instruments {
    double Bell::sound(double time, double frequency) const {
        return (
            1.0 * oscillators::wave_sine(time, frequency * 2.0, { 5.0, 0.001 }) +
            0.5 * oscillators::wave_sine(time, frequency * 3.0, {}) +
            0.25 * oscillators::wave_sine(time, frequency * 4.0, {})
        );
    }

    double Harmonica::sound(double time, double frequency) const {
        return (
            1.0 * oscillators::wave_square(time, frequency, { 5.0, 0.001 }) +
            0.5 * oscillators::wave_square(time, frequency * 1.5, {}) +
            0.25 * oscillators::wave_square(time, frequency * 2.0, {}) +
            0.05 * oscillators::noise()
        );
    }
}
