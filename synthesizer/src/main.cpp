#include <iostream>
#include <cmath>

#include "application.hpp"
#include "audio.hpp"
#include "math.hpp"

class SynthesizerStream : public AudioStream {
public:
    enum class Wave {
        Sine,
        Square,
        Triangle,
        Saw1,
        Saw2
    };

    volatile Wave wave {Wave::Square};
    volatile double frequency {};
private:
    double make_noise(double time) const override {
        switch (wave) {
            case Wave::Sine:
                return sine_wave(time, frequency, 0.1);
            case Wave::Square:
                return square_wave(time, frequency, 0.06);
            case Wave::Triangle:
                return triangle_wave(time, frequency, 0.08);
            case Wave::Saw1:
                return saw1_wave(time, frequency, 0.06);
            case Wave::Saw2:
                return saw2_wave(time, frequency, 0.06);
        }

        return 0.0;
    }

    static double sine_wave(double time, double frequency, double amplitude) {
        return amplitude * std::sin(w(frequency) * time);
    }

    static double square_wave(double time, double frequency, double amplitude) {
        const double value {std::sin(w(frequency) * time)};

        if (value >= 0.0) {
            return amplitude;
        } else {
            return -amplitude;
        }
    }

    static double triangle_wave(double time, double frequency, double amplitude) {
        return amplitude * std::asin(std::sin(w(frequency) * time)) * (2.0 / PI);
    }

    static double saw1_wave(double time, double frequency, double amplitude) {
        double result {};

        for (double n {1.0}; n < 10.0; n++) {
            result += std::sin(n * w(frequency) * time) / n;
        }

        return amplitude * result * (2.0 / PI);
    }

    static double saw2_wave(double time, double frequency, double amplitude) {
        return amplitude * (frequency * PI * std::fmod(time, 1.0 / frequency) - PI / 2.0) * (2.0 / PI);
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

        bool key_pressed {};

        for (double step {}; const auto key : KEYBOARD) {
            if (keyboard[key]) {
                m_audio_stream.frequency = base_frequency * std::pow(step_frequency, step);
                key_pressed = true;
            }
            step += 1.0;
        }

        if (!key_pressed) {
            m_audio_stream.frequency = 0.0;
        }
    }
private:
    SynthesizerStream m_audio_stream;
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
