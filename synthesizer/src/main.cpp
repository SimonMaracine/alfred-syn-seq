#include <iostream>
#include <cmath>

#include "application.hpp"
#include "audio.hpp"
#include "math.hpp"

struct Envelope {
    virtual ~Envelope() = default;

    virtual double get_amplitude(double time) const = 0;
    virtual void trigger_on(double time) = 0;
    virtual void trigger_off(double time) = 0;
};

class EnvelopeAdsr : public Envelope {
public:
    double get_amplitude(double time) const override {
        double result {};

        const double life_time {time - m_trigger_on_time};

        if (m_trigger_on) {
            // Attack
            if (life_time <= m_attack_time) {
                result = life_time / m_attack_time * m_start_amplitude;
            }

            // Decay
            if (life_time > m_attack_time && life_time <= m_attack_time + m_decay_time) {
                result = ((life_time - m_attack_time) / m_decay_time) * (m_sustain_amplitude - m_start_amplitude) + m_start_amplitude;
            }

            // Sustain
            if (life_time > m_attack_time + m_decay_time) {
                result = m_sustain_amplitude;
            }
        } else {
            // Release
            result = ((time - m_trigger_off_time) / m_release_time) * -m_sustain_amplitude + m_sustain_amplitude;
        }

        if (result <= 0.0001) {
            result = 0.0;
        }

        return result;
    }

    void trigger_on(double time) override {
        m_trigger_on_time = time;
        m_trigger_on = true;
    }

    void trigger_off(double time) override {
        m_trigger_off_time = time;
        m_trigger_on = false;
    }
private:
    double m_attack_time {0.02};
    double m_decay_time {0.01};
    double m_release_time {0.03};

    double m_sustain_amplitude {0.8};
    double m_start_amplitude {1.0};

    double m_trigger_on_time {};
    double m_trigger_off_time {};

    bool m_trigger_on {};
};

class SynthesizerStream : public AudioStream {
public:
    enum class Wave {
        Sine,
        Square,
        Triangle,
        Saw1,
        Saw2
    };

    Wave wave {Wave::Square};
    double frequency {};
    EnvelopeAdsr envelope_adsr;
private:
    double oscillator(double time) const override {
        switch (wave) {
            case Wave::Sine:
                return sine_wave(time, frequency);
            case Wave::Square:
                return square_wave(time, frequency);
            case Wave::Triangle:
                return triangle_wave(time, frequency);
            case Wave::Saw1:
                return saw1_wave(time, frequency);
            case Wave::Saw2:
                return saw2_wave(time, frequency);
        }

        return 0.0;
    }

    double envelope(double time) const override {
        return envelope_adsr.get_amplitude(time);
    }

    double master_volume() const override {
        return 0.1;
    }

    static double sine_wave(double time, double frequency) {
        return std::sin(w(frequency) * time);
    }

    static double square_wave(double time, double frequency) {
        const double value {std::sin(w(frequency) * time)};

        if (value >= 0.0) {
            return 1.0;
        } else {
            return -1.0;
        }
    }

    static double triangle_wave(double time, double frequency) {
        return std::asin(std::sin(w(frequency) * time)) * (2.0 / PI);
    }

    static double saw1_wave(double time, double frequency) {
        double result {};

        for (double n {1.0}; n < 10.0; n++) {
            result += std::sin(n * w(frequency) * time) / n;
        }

        return result * (2.0 / PI);
    }

    static double saw2_wave(double time, double frequency) {
        return (frequency * PI * std::fmod(time, 1.0 / frequency) - PI / 2.0) * (2.0 / PI);
    }
};

class Synthesizer : public Application {
public:
    void on_start() override {
        m_audio_stream.resume();
    }

    void on_event(const SDL_Event& event) override {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    m_running = false;
                } else if (event.key.key == SDLK_I) {
                    m_audio_stream.wave = SynthesizerStream::Wave::Sine;
                } else if (event.key.key == SDLK_Q) {
                    m_audio_stream.wave = SynthesizerStream::Wave::Square;
                } else if (event.key.key == SDLK_R) {
                    m_audio_stream.wave = SynthesizerStream::Wave::Triangle;
                } else if (event.key.key == SDLK_1) {
                    m_audio_stream.wave = SynthesizerStream::Wave::Saw1;
                } else if (event.key.key == SDLK_2) {
                    m_audio_stream.wave = SynthesizerStream::Wave::Saw2;
                }

                break;
        }
    }

    void on_update() override {
        static constexpr double base_frequency {110.0};  // La
        const double step_frequency {std::pow(2.0, 1.0 / 12.0)};

        const bool* keyboard {SDL_GetKeyboardState(nullptr)};

        static constexpr SDL_Scancode KEYBOARD[] {
            SDL_SCANCODE_Z,
            SDL_SCANCODE_S,
            SDL_SCANCODE_X,
            SDL_SCANCODE_C,
            SDL_SCANCODE_F,
            SDL_SCANCODE_V,
            SDL_SCANCODE_G,
            SDL_SCANCODE_B,
            SDL_SCANCODE_N,
            SDL_SCANCODE_J,
            SDL_SCANCODE_M,
            SDL_SCANCODE_K,
            SDL_SCANCODE_COMMA
        };

        m_key_pressed = false;

        for (double step {}; const auto key : KEYBOARD) {
            if (keyboard[key]) {
                if (m_current_key != key) {
                    m_audio_stream.frequency = base_frequency * std::pow(step_frequency, step);
                    m_audio_stream.envelope_adsr.trigger_on(m_audio_stream.get_time());
                    m_current_key = key;
                }

                m_key_pressed = true;
            }

            step += 1.0;
        }

        if (!m_key_pressed) {
            if (m_current_key != SDL_SCANCODE_UNKNOWN) {
                m_audio_stream.envelope_adsr.trigger_off(m_audio_stream.get_time());
                m_current_key = SDL_SCANCODE_UNKNOWN;
            }
        }
    }
private:
    SynthesizerStream m_audio_stream;
    SDL_Scancode m_current_key {SDL_SCANCODE_UNKNOWN};
    bool m_key_pressed {};
};

int main() {
    try {
        Synthesizer synthesizer;
        synthesizer.run();
    } catch (const ApplicationError& e) {
        std::cerr << "Fatal application error: " << e.what() << '\n';
        return 1;
    } catch (const AudioStreamError& e) {
        std::cerr << "Fatal audio error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
