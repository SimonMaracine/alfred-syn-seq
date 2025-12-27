#include "application.hpp"

#include <format>

Application::Application() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        throw ApplicationError(std::format("SDL_Init: {}", SDL_GetError()));
    }

    if (!SDL_CreateWindowAndRenderer("synthesizer", 640, 360, 0, &m_window, &m_renderer)) {
        throw ApplicationError(std::format("SDL_CreateWindowAndRenderer: {}", SDL_GetError()));
    }

    if (!SDL_SetRenderVSync(m_renderer, 1)) {
        throw ApplicationError(std::format("SDL_SetRenderVSync: {}", SDL_GetError()));
    }
}

Application::~Application() {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
    }

    SDL_Quit();
}

void Application::run() {
    on_start();

    while (m_running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_running = false;
                    break;
            }

            on_event(event);
        }

        on_update();

        if (!SDL_RenderClear(m_renderer)) {
            throw ApplicationError(std::format("SDL_RenderClear: {}", SDL_GetError()));
        }

        on_render();

        if (!SDL_RenderPresent(m_renderer)) {
            throw ApplicationError(std::format("SDL_RenderPresent: {}", SDL_GetError()));
        }
    }

    on_stop();
}
