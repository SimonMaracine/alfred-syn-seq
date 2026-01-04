#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <tuple>
#include <utility>
#include <cmath>

#include "application.hpp"
#include "audio.hpp"
#include "synthesis.hpp"

struct Voices : private std::tuple<
    instruments::Bell,
    instruments::Harmonica
> {
    const Instrument& operator[](std::size_t index) const {
        switch (index) {
            case 0: return std::get<0>(*this);
            case 1: return std::get<1>(*this);
        }

        std::unreachable();
    }
};

class Synthesizer : public AudioStream {
public:
    void note_on(Note note, unsigned int octave, unsigned int voice) {
        StreamLockGuard guard {this};

        const auto iter {std::find_if(m_sounds.begin(), m_sounds.end(), [note](const Sound& sound) { return sound.note == note; })};

        if (iter == m_sounds.end()) {
            Sound& sound {m_sounds.emplace_back()};
            sound.note = note;
            sound.octave = octave;
            sound.voice = voice;
            sound.time_on = get_time();
        } else {
            if (iter->time_off > iter->time_on) {
                iter->time_on = get_time();
            }
        }
    }

    void note_off(Note note, unsigned int octave) {
        StreamLockGuard guard {this};

        const auto iter {std::find_if(m_sounds.begin(), m_sounds.end(), [note](const Sound& sound) { return sound.note == note; })};

        if (iter != m_sounds.end()) {
            if (iter->time_on > iter->time_off) {
                iter->time_off = get_time();
            }
        }
    }

    void update() {
        StreamLockGuard guard {this};

        m_sounds.erase(std::remove_if(m_sounds.begin(), m_sounds.end(), [this](const Sound& sound) {
            return m_voices[sound.voice].get_envelope().is_done(get_time(), sound.time_on, sound.time_off);
        }), m_sounds.end());
    }
private:
    double sound(double time) const override {
        double result {};

        for (const Sound& sound : m_sounds) {
            result += m_voices[sound.voice].sound(time, sound);
        }

        return result;
    }

    double volume() const override {
        return 0.1;
    }

    Voices m_voices;
    std::vector<Sound> m_sounds;
};

class SynthesizerApplication : public Application {
public:
    void on_start() override {
        m_synthesizer.resume();
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
            SDL_SCANCODE_COMMA,
            SDL_SCANCODE_L,
            SDL_SCANCODE_PERIOD,
            SDL_SCANCODE_SLASH
        };

        for (unsigned int note {}; const auto key : KEYBOARD) {
            if (keyboard[key]) {
                m_synthesizer.note_on(Note(note), 0, 0);
            } else {
                m_synthesizer.note_off(Note(note), 0);
            }

            note++;
        }

        m_synthesizer.update();
    }
private:
    Synthesizer m_synthesizer;
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
