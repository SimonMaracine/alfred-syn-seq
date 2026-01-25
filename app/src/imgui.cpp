#include "imgui.hpp"

#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <backends/imgui_impl_sdl3.h>

namespace imgui {
    void initialize(SDL_Window* window, SDL_Renderer* renderer) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
    }

    void uninitialize() {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void process_event(const SDL_Event* event) {
        ImGui_ImplSDL3_ProcessEvent(event);
    }

    void begin() {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void end(SDL_Renderer* renderer) {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

    bool want_capture_mouse() {
        return ImGui::GetIO().WantCaptureMouse;
    }

    bool want_capture_keyboard() {
        return ImGui::GetIO().WantCaptureKeyboard;
    }
}
