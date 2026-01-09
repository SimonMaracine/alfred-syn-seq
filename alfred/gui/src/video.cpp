#include "video.hpp"

#include <format>

#include <SDL3/SDL.h>

#include "imgui.hpp"

static constexpr unsigned long long IMGUI_UPDATE_INTERVAL {16 * SDL_NS_PER_MS};

Video::Video() {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw VideoError(std::format("SDL_InitSubSystem: {}", SDL_GetError()));
    }

    if (!SDL_CreateWindowAndRenderer("Alfred", 1280, 720, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer)) {
        throw VideoError(std::format("SDL_CreateWindowAndRenderer: {}", SDL_GetError()));
    }

    // We really don't want VSync
    if (!SDL_SetRenderVSync(m_renderer, 0)) {
        throw VideoError(std::format("SDL_SetRenderVSync: {}", SDL_GetError()));
    }

    if (!SDL_SetWindowMinimumSize(m_window, 320, 180)) {
        throw VideoError(std::format("SDL_SetWindowMinimumSize: {}", SDL_GetError()));
    }

    imgui_initialize(m_window, m_renderer);
    set_desired_frame_time(16);
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

    m_previous_time = SDL_GetTicksNS();

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

        m_keyboard_state = SDL_GetKeyboardState(nullptr);

        on_update();

        if (m_imgui_accumulator_time > IMGUI_UPDATE_INTERVAL) {
            render();
            m_imgui_accumulator_time = 0;
        }

        unsigned long long current_time {};

        current_time = SDL_GetTicksNS();
        m_frame_time = current_time - m_previous_time;

        if (m_frame_time < m_desired_frame_time) {
            SDL_DelayNS(m_desired_frame_time - m_frame_time);
        }

        current_time = SDL_GetTicksNS();
        m_frame_time = current_time - m_previous_time;
        m_previous_time = current_time;

        m_imgui_accumulator_time += m_frame_time;

        m_frame_timef = static_cast<double>(m_frame_time) / static_cast<double>(SDL_NS_PER_SECOND);
    }

    on_stop();
}

void Video::set_desired_frame_time(unsigned long long milliseconds) {
    m_desired_frame_time = milliseconds * SDL_NS_PER_MS;
}

void Video::render() {
    if (!SDL_RenderClear(m_renderer)) {
        throw VideoError(std::format("SDL_RenderClear: {}", SDL_GetError()));
    }

    imgui_begin();
    on_imgui();
    imgui_end(m_renderer);

    if (!SDL_RenderPresent(m_renderer)) {
        throw VideoError(std::format("SDL_RenderPresent: {}", SDL_GetError()));
    }
}
