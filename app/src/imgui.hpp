#pragma once

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

namespace imgui {
    void initialize(SDL_Window* window, SDL_Renderer* renderer);
    void uninitialize();
    void process_event(const SDL_Event* event);
    void begin();
    void end(SDL_Renderer* renderer);
    bool want_capture_mouse();
    bool want_capture_keyboard();
}
