#include <iostream>
#include <memory>
#include <cmath>

#include "application.hpp"
#include "audio.hpp"
#include "synthesis.hpp"

class SynthesizerStream : public AudioStream {
public:
    double frequency {};
    std::unique_ptr<Instrument> instrument {std::make_unique<instruments::Bell>()};
private:
    double sound(double time) const override {
        return instrument->sound(time, frequency);
    }

    double envelope(double time) const override {
        return instrument->get_envelope().get_amplitude(time);
    }

    double volume() const override {
        return 0.1;
    }
};

class SynthesizerApplication : public Application {
public:
    void on_start() override {
        m_audio_stream.resume();
    }

    void on_event(const SDL_Event& event) override {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    m_running = false;
                }

                break;
        }
    }

    void on_update() override {
        static constexpr double base_frequency {110.0};  // La
        static constexpr double step_frequency {1.059463094};  // 2.0 ** (1.0 / 12.0)

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
                    m_audio_stream.instrument->get_envelope().trigger_on(m_audio_stream.get_time());
                    m_current_key = key;
                }

                m_key_pressed = true;
            }

            step += 1.0;
        }

        if (!m_key_pressed) {
            if (m_current_key != SDL_SCANCODE_UNKNOWN) {
                m_audio_stream.instrument->get_envelope().trigger_off(m_audio_stream.get_time());
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
        SynthesizerApplication application;
        application.run();
    } catch (const ApplicationError& e) {
        std::cerr << "Fatal application error: " << e.what() << '\n';
        return 1;
    } catch (const AudioStreamError& e) {
        std::cerr << "Fatal audio error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
