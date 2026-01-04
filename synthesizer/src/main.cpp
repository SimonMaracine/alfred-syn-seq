#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include <cmath>

#include "application.hpp"
#include "audio.hpp"
#include "synthesis.hpp"

class Synthesizer : public AudioStream {
public:
    void note_on(syn::Note note, syn::Octave octave, syn::Voice voice) {
        StreamLockGuard guard {this};

        const auto iter {find_sound(note, octave)};

        if (iter == m_sounds.end()) {
            syn::Sound& sound {m_sounds.emplace_back()};
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

    void note_off(syn::Note note, syn::Octave octave) {
        StreamLockGuard guard {this};

        const auto iter {find_sound(note, octave)};

        if (iter != m_sounds.end()) {
            if (iter->time_on > iter->time_off) {
                iter->time_off = get_time();
            }
        }
    }

    void update() {
        StreamLockGuard guard {this};

        m_sounds.erase(std::remove_if(m_sounds.begin(), m_sounds.end(), [this](const syn::Sound& sound) {
            return m_voices[sound.voice].get_envelope().is_done(get_time(), sound.time_on, sound.time_off);
        }), m_sounds.end());
    }
private:
    std::vector<syn::Sound>::iterator find_sound(syn::Note note, syn::Octave octave) {
        return std::find_if(m_sounds.begin(), m_sounds.end(),
            [note, octave](const syn::Sound& sound) {
                return sound.note == note && sound.octave == octave;
            }
        );
    }

    double sound(double time) const override {
        double result {};

        for (const syn::Sound& sound : m_sounds) {
            result += m_voices[sound.voice].sound(time, sound);
        }

        return result;
    }

    double volume() const override {
        return 0.1;
    }

    syn::Voices m_voices;
    std::vector<syn::Sound> m_sounds;
};

class SynthesizerApplication : public Application {
public:
    void on_start() override {
        m_synthesizer.resume();
    }

    void on_event(const SDL_Event& event) override {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        m_running = false;
                        break;
                    case SDLK_Q:
                        m_voice = 0;
                        break;
                    case SDLK_W:
                        m_voice = 1;
                        break;
                    case SDLK_1:
                        m_octave = syn::Octave0;
                        break;
                    case SDLK_2:
                        m_octave = syn::Octave1;
                        break;
                    case SDLK_3:
                        m_octave = syn::Octave2;
                        break;
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
                m_synthesizer.note_on(syn::Note(note), m_octave, m_voice);
            } else {
                m_synthesizer.note_off(syn::Note(note), m_octave);
            }

            note++;
        }

        m_synthesizer.update();
    }
private:
    syn::Voice m_voice {};
    syn::Octave m_octave {syn::Octave1};
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
