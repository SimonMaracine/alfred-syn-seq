#include "application.hpp"

#include <SDL3/SDL.h>

void Application::on_start() {
    m_synthesizer.resume();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsClassic();
}

void Application::on_stop() {

}

void Application::on_update() {
    m_keyboard = SDL_GetKeyboardState(nullptr);

    static constexpr SDL_Scancode KEYBOARD[] {
        SDL_SCANCODE_Z,
        SDL_SCANCODE_S,
        SDL_SCANCODE_X,
        SDL_SCANCODE_C,
        SDL_SCANCODE_F,
        SDL_SCANCODE_V,
        SDL_SCANCODE_G,
        SDL_SCANCODE_B,
        SDL_SCANCODE_N,
        SDL_SCANCODE_J,
        SDL_SCANCODE_M,
        SDL_SCANCODE_K,
        SDL_SCANCODE_COMMA,
        SDL_SCANCODE_L,
        SDL_SCANCODE_PERIOD,
        SDL_SCANCODE_SLASH
    };

    for (unsigned int note {}; const auto key : KEYBOARD) {
        if (m_keyboard[key]) {
            m_synthesizer.note_on(syn::Note(note), m_octave, m_voice);
        } else {
            m_synthesizer.note_off(syn::Note(note), m_octave);
        }

        note++;
    }

    m_synthesizer.update();
}

void Application::on_render() {
    ImGui::DockSpaceOverViewport();

    main_menu_bar();
    keyboard();

    ImGui::ShowDemoWindow();
}

void Application::on_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.key) {
                case SDLK_Q:
                    m_voice = 0;
                    break;
                case SDLK_W:
                    m_voice = 1;
                    break;
                case SDLK_1:
                    m_octave = syn::Octave0;
                    break;
                case SDLK_2:
                    m_octave = syn::Octave1;
                    break;
                case SDLK_3:
                    m_octave = syn::Octave2;
                    break;
            }

            break;
    }
}

void Application::main_menu_bar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            main_menu_bar_file();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            main_menu_bar_edit();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")) {
            main_menu_bar_options();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            main_menu_bar_help();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Application::main_menu_bar_file() {
    if (ImGui::MenuItem("New")) {

    }

    if (ImGui::MenuItem("Quit")) {
        m_running = false;
    }
}

void Application::main_menu_bar_edit() {
    if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
    if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {} // Disabled item
    ImGui::Separator();
    if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
    if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
    if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
}

void Application::main_menu_bar_help() {

}

void Application::main_menu_bar_options() {
    if (ImGui::BeginMenu("Color Scheme")) {
        const char* SCHEMES[] { "Dark", "Light", "Classic" };

        if (ImGui::BeginCombo("##", SCHEMES[m_color_scheme])) {
            for (std::size_t i {}; i < std::size(SCHEMES); i++) {
                if (ImGui::Selectable(SCHEMES[i], m_color_scheme == i)) {
                    m_color_scheme = ColorScheme(i);

                    switch (m_color_scheme) {
                        case ColorSchemeDark:
                            ImGui::StyleColorsDark();
                            break;
                        case ColorSchemeLight:
                            ImGui::StyleColorsLight();
                            break;
                        case ColorSchemeClassic:
                            ImGui::StyleColorsClassic();
                            break;
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::EndMenu();
    }
}

void Application::keyboard() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("Keyboard")) {
        ImDrawList* list {ImGui::GetWindowDrawList()};
        const ImVec2 origin {ImGui::GetCursorScreenPos()};

        keyboard_key(list, origin, 'S', 1.0f, 0.0f, SDL_SCANCODE_S);
        keyboard_key(list, origin, 'F', 5.0f, 0.0f, SDL_SCANCODE_F);
        keyboard_key(list, origin, 'G', 7.0f, 0.0f, SDL_SCANCODE_G);
        keyboard_key(list, origin, 'J', 11.0f, 0.0f, SDL_SCANCODE_J);
        keyboard_key(list, origin, 'K', 13.0f, 0.0f, SDL_SCANCODE_K);
        keyboard_key(list, origin, 'L', 15.0f, 0.0f, SDL_SCANCODE_L);

        keyboard_key(list, origin, 'Z', 0.0f, 2.0f, SDL_SCANCODE_Z);
        keyboard_key(list, origin, 'X', 2.0f, 2.0f, SDL_SCANCODE_X);
        keyboard_key(list, origin, 'C', 4.0f, 2.0f, SDL_SCANCODE_C);
        keyboard_key(list, origin, 'V', 6.0f, 2.0f, SDL_SCANCODE_V);
        keyboard_key(list, origin, 'B', 8.0f, 2.0f, SDL_SCANCODE_B);
        keyboard_key(list, origin, 'N', 10.0f, 2.0f, SDL_SCANCODE_N);
        keyboard_key(list, origin, 'M', 12.0f, 2.0f, SDL_SCANCODE_M);
        keyboard_key(list, origin, ',', 14.0f, 2.0f, SDL_SCANCODE_COMMA);
        keyboard_key(list, origin, '.', 16.0f, 2.0f, SDL_SCANCODE_PERIOD);
        keyboard_key(list, origin, '/', 18.0f, 2.0f, SDL_SCANCODE_SLASH);
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

void Application::keyboard_key(ImDrawList* list, ImVec2 origin, char key, float x, float y, int scancode) {
    static constexpr float CELL {34.0f};
    static constexpr float PADDING {2.0f};
    static constexpr float TEXT_OFFSET {(2.0f * CELL - 13.0f) / 2.0f};
    static constexpr float WIDTH {2.0f * 10.0f * CELL};
    static constexpr float HEIGHT {2.0f * 2.0f * CELL};

    const ImGuiStyle& style {ImGui::GetStyle()};

    const ImColor COLOR_INACTIVE {style.Colors[ImGuiCol_TableBorderLight]};
    const ImColor COLOR_ACTIVE {style.Colors[ImGuiCol_PlotHistogramHovered]};

    const ImVec2 space_available {ImGui::GetContentRegionAvail()};

    const ImVec2 base {(space_available.x - WIDTH) / 2.0f, (space_available.y - HEIGHT) / 2.0f};
    const ImVec2 position {x * CELL, y * CELL};
    const char label[2] { key, '\0' };
    const ImColor color {m_keyboard[scancode] ? COLOR_ACTIVE : COLOR_INACTIVE};

    list->AddRectFilled(
        base + origin + position + ImVec2(PADDING, PADDING),
        base + origin + position + ImVec2(2.0f * CELL, 2.0f * CELL) - ImVec2(PADDING, PADDING),
        color,
        12.0f
    );

    list->AddText(base + origin + position + ImVec2(TEXT_OFFSET, TEXT_OFFSET), IM_COL32_WHITE, label);
}
