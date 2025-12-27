#pragma once

#include <stdexcept>

#include <SDL3/SDL.h>

class Application {
public:
    Application();
    virtual ~Application();

    virtual void on_start() {}
    virtual void on_stop() {}
    virtual void on_update() {}
    virtual void on_render() const {}
    virtual void on_event(const SDL_Event&) {}

    void run();
private:
    SDL_Window* m_window {};
    SDL_Renderer* m_renderer {};
    bool m_running {true};
};

struct ApplicationError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
