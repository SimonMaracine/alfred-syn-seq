#include "video.hpp"

#include <format>

#include <SDL3/SDL.h>

#include "imgui.hpp"

Video::Video() {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw VideoError(std::format("SDL_InitSubSystem: {}", SDL_GetError()));
    }

    if (!SDL_CreateWindowAndRenderer("Alfred", 1280, 720, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer)) {
        throw VideoError(std::format("SDL_CreateWindowAndRenderer: {}", SDL_GetError()));
    }

    if (!SDL_SetRenderVSync(m_renderer, 1)) {
        throw VideoError(std::format("SDL_SetRenderVSync: {}", SDL_GetError()));
    }

    if (!SDL_SetWindowMinimumSize(m_window, 320, 180)) {
        throw VideoError(std::format("SDL_SetWindowMinimumSize: {}", SDL_GetError()));
    }

    imgui_initialize(m_window, m_renderer);
}

Video::~Video() {
    imgui_uninitialize();

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
    }

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Video::run() {
    on_start();

    while (m_running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            imgui_process_event(&event);

            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_running = false;
                    break;
            }

            on_event(event);
        }

        on_update();

        if (!SDL_RenderClear(m_renderer)) {
            throw VideoError(std::format("SDL_RenderClear: {}", SDL_GetError()));
        }

        imgui_begin();
        on_render();
        imgui_end(m_renderer);

        if (!SDL_RenderPresent(m_renderer)) {
            throw VideoError(std::format("SDL_RenderPresent: {}", SDL_GetError()));
        }
    }

    on_stop();
}
