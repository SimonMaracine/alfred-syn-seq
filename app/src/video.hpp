#pragma once

#include <stdexcept>
#include <span>
#include <initializer_list>

union SDL_Event;
struct SDL_Window;
struct SDL_Renderer;

namespace video {
    class Video {
    public:
        Video();
        virtual ~Video();

        Video(const Video&) = delete;
        Video& operator=(const Video&) = delete;
        Video(Video&&) = delete;
        Video& operator=(Video&&) = delete;

        virtual void on_start() {}
        virtual void on_stop() {}
        virtual void on_update() {}
        virtual void on_imgui() {}
        virtual void on_late_update() {}
        virtual void on_event(const SDL_Event&) {}

        void run();
    protected:
        void set_desired_frame_time(unsigned long long milliseconds);
        void set_icons(std::initializer_list<std::span<const unsigned char>> images) const;
        static const char* get_property(const char* property);

        const bool* get_keyboard_state() const { return m_keyboard_state; }
        double get_frame_time() const { return m_frame_timef; }

        SDL_Window* m_window {};
        SDL_Renderer* m_renderer {};
        bool m_running {true};
    private:
        void render();

        const bool* m_keyboard_state {};
        double m_frame_timef {};

        unsigned long long m_previous_time {};
        unsigned long long m_frame_time {};
        unsigned long long m_desired_frame_time {};
        unsigned long long m_imgui_accumulator_time {};
    };

    struct VideoError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
