#pragma once

#include <stdexcept>

union SDL_Event;
struct SDL_Window;
struct SDL_Renderer;

class Video {
public:
    Video();
    virtual ~Video();

    virtual void on_start() {}
    virtual void on_stop() {}
    virtual void on_update() {}
    virtual void on_render() {}
    virtual void on_event(const SDL_Event&) {}

    void run();
protected:
    SDL_Window* m_window {};
    SDL_Renderer* m_renderer {};
    bool m_running {true};
};

struct VideoError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
