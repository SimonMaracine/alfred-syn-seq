#include "video.hpp"

#include <format>
#include <vector>

#include <SDL3/SDL.h>

#include "imgui.hpp"
#include "image.hpp"

namespace video {
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

        imgui::initialize(m_window, m_renderer);
        set_desired_frame_time(16);
    }

    Video::~Video() {
        imgui::uninitialize();

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
                imgui::process_event(&event);

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

            on_late_update();

            unsigned long long current_time {};

            current_time = SDL_GetTicksNS();
            m_frame_time = current_time - m_previous_time;

            if (m_frame_time < m_desired_frame_time) {
                SDL_DelayNS(m_desired_frame_time - m_frame_time);
            }

            current_time = SDL_GetTicksNS();
            m_frame_time = std::min(current_time - m_previous_time, 50ull * SDL_NS_PER_MS);  // Cap the frame time at 20 frames per second
            m_previous_time = current_time;

            m_imgui_accumulator_time += m_frame_time;

            m_frame_timef = double(m_frame_time) / double(SDL_NS_PER_SECOND);
        }

        on_stop();
    }

    void Video::set_desired_frame_time(unsigned long long milliseconds) {
        m_desired_frame_time = milliseconds * SDL_NS_PER_MS;
    }

    void Video::set_icons(std::initializer_list<std::span<const unsigned char>> icons) const {
        std::vector<image::Surface> surfaces;

        for (const auto icon : icons) {
            surfaces.emplace_back(icon);
        }

        const image::SurfaceRef surface_first {surfaces.front()};

        for (auto surface {std::next(surfaces.begin())}; surface != surfaces.end(); surface++) {
            surface_first.add_alternate(*surface);
        }

        if (!SDL_SetWindowIcon(m_window, surface_first.get())) {
            throw VideoError(std::format("SDL_SetWindowIcon: {}", SDL_GetError()));
        }
    }

    void Video::set_title(std::string_view title) const {
        if (!SDL_SetWindowTitle(m_window, title.data())) {
            throw VideoError(std::format("SDL_SetWindowTitle: {}", SDL_GetError()));
        }
    }

    const char* Video::get_property(const char* property) {
        const char* value {SDL_GetAppMetadataProperty(property)};

        if (!value) {
            return "";
        }

        return value;
    }

    void Video::render() {
        if (!SDL_RenderClear(m_renderer)) {
            throw VideoError(std::format("SDL_RenderClear: {}", SDL_GetError()));
        }

        imgui::begin();
        on_imgui();
        imgui::end(m_renderer);

        if (!SDL_RenderPresent(m_renderer)) {
            throw VideoError(std::format("SDL_RenderPresent: {}", SDL_GetError()));
        }
    }
}
