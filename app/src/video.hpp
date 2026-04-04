#pragma once

#include <stdexcept>
#include <span>
#include <initializer_list>
#include <string_view>
#include <utility>

union SDL_Event;
struct SDL_Window;
struct SDL_Renderer;

namespace video {
    inline constexpr int DEFAULT_WIDTH = 1280;
    inline constexpr int DEFAULT_HEIGHT = 720;
    inline constexpr auto MAX_DELTA = 40ull;  // Milliseconds (20 FPS)

    // Represents an application window and a main loop
    class Video {
    public:
        Video();
        virtual ~Video();

        Video(const Video&) = delete;
        Video& operator=(const Video&) = delete;
        Video(Video&&) = delete;
        Video& operator=(Video&&) = delete;

        // Called before the main loop
        virtual void on_start() {}

        // Called after the main loop
        virtual void on_stop() {}

        // Called for each iteration of the main loop
        virtual void on_update() {}

        // Called at a steady rate; call Dear ImGui render functions only here
        virtual void on_imgui() {}

        // Called later for each iteration of the main loop
        virtual void on_late_update() {}

        // Called for every event
        virtual void on_event(const SDL_Event&) {}

        // Start the main loop
        void run();
    protected:
        // Set the desired frame rate of the application
        // VSync is turned off explicitly
        void desired_frame_time(unsigned long long milliseconds);

        // Set the window icons
        void icons(std::initializer_list<std::span<const unsigned char>> icons) const;

        // Set the window title
        void title(std::string_view title) const;

        // Get/set the window size
        void window_size(int width, int height) const;
        std::pair<int, int> window_size() const;

        const bool* keyboard_state() const { return m_keyboard_state; }
        double frame_time() const { return m_frame_timef; }

        SDL_Window* m_window {};
        SDL_Renderer* m_renderer {};
        char* m_working_directory {};
        bool m_running = true;
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
