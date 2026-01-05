#pragma once

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

void imgui_initialize(SDL_Window* window, SDL_Renderer* renderer);
void imgui_uninitialize();
void imgui_process_event(SDL_Event* event);
void imgui_begin();
void imgui_end(SDL_Renderer* renderer);
bool imgui_want_capture_mouse();
bool imgui_want_capture_keyboard();
