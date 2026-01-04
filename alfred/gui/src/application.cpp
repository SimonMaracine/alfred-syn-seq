#include "application.hpp"

#include <SDL3/SDL.h>

void Application::on_start() {
    m_synthesizer.resume();
}

void Application::on_event(const SDL_Event& event) {
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

void Application::on_update() {
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
