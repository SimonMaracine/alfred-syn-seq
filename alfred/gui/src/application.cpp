#include "application.hpp"

#include <SDL3/SDL.h>

void Application::on_start() {
    m_synthesizer.resume();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsClassic();

    for (int i {}; i < 8; i++) {
        m_composition.measures.emplace_back();
    }

    m_player = Player(m_synthesizer, m_composition);
}

void Application::on_stop() {

}

void Application::on_update() {
    m_player.update(get_frame_time());
    m_synthesizer.update();
}

void Application::on_imgui() {
    ImGui::DockSpaceOverViewport();

    main_menu_bar();
    keyboard();
    playback();
    debug();

    ImGui::ShowDemoWindow();
}

void Application::on_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event.key.key) {
                case SDLK_Q:
                    m_voice = syn::VoiceBell;
                    break;
                case SDLK_W:
                    m_voice = syn::VoiceHarmonica;
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

            update_keyboard_input(event.key.key, true);

            break;
        case SDL_EVENT_KEY_UP:
            update_keyboard_input(event.key.key, false);

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

        if (ImGui::BeginMenu("Sequencer")) {
            main_menu_bar_sequencer();
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

void Application::main_menu_bar_sequencer() {

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
    const ImColor color {get_keyboard_state()[scancode] ? COLOR_ACTIVE : COLOR_INACTIVE};

    list->AddRectFilled(
        base + origin + position + ImVec2(PADDING, PADDING),
        base + origin + position + ImVec2(2.0f * CELL, 2.0f * CELL) - ImVec2(PADDING, PADDING),
        color,
        12.0f
    );

    list->AddText(base + origin + position + ImVec2(TEXT_OFFSET, TEXT_OFFSET), IM_COL32_WHITE, label);
}

void Application::playback() {
    if (ImGui::Begin("Playback")) {
        if (ImGui::Button("Rewind")) {
            m_player.seek(0);
        }

        ImGui::SameLine();

        if (m_player.is_playing()) {
            if (ImGui::Button("Stop")) {
                m_player.stop();
            }
        } else {
            if (ImGui::Button("Start")) {
                m_player.start();
            }
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Metronome", &m_metronome)) {
            if (m_metronome) {
                for (unsigned int i {}; i < 8 * 16; i += 4) {
                    const syn::Name name {i % 16 == 0 ? syn::C : syn::D};
                    m_composition.voices[syn::VoiceBell].emplace_back(name, syn::Octave1, Eighth, i);
                }
                m_player.reload();  // FIXME
            } else {
                m_composition.voices.erase(syn::VoiceBell);
                m_player.reload();
            }
        }

        ImGui::SameLine();

        ImGui::Text("%f", m_player.get_elapsed_time());

        ImGui::SameLine();

        ImGui::Text("%u", m_player.get_position());
    }

    ImGui::End();
}

void Application::debug() {
    if (ImGui::Begin("Debug")) {
        ImGui::Text("Frame time: %f", get_frame_time());
    }

    ImGui::End();
}

void Application::update_keyboard_input(unsigned int key, bool down) {
    const auto update {[this, down](syn::Name name) {
        if (down) {
            m_synthesizer.note_on(name, m_octave, m_voice);
        } else {
            m_synthesizer.note_off(name, m_octave);
        }
    }};

    switch (key) {
        case SDLK_Z: update(syn::Name::A); break;
        case SDLK_S: update(syn::Name::As); break;
        case SDLK_X: update(syn::Name::B); break;
        case SDLK_C: update(syn::Name::C); break;
        case SDLK_F: update(syn::Name::Cs); break;
        case SDLK_V: update(syn::Name::D); break;
        case SDLK_G: update(syn::Name::Ds); break;
        case SDLK_B: update(syn::Name::E); break;
        case SDLK_N: update(syn::Name::F); break;
        case SDLK_J: update(syn::Name::Fs); break;
        case SDLK_M: update(syn::Name::G); break;
        case SDLK_K: update(syn::Name::Gs); break;
        case SDLK_COMMA: update(syn::Name::A2); break;
        case SDLK_L: update(syn::Name::As2); break;
        case SDLK_PERIOD: update(syn::Name::B2); break;
        case SDLK_SLASH: update(syn::Name::C2); break;
    }

    // static constexpr unsigned int KEYBOARD[] {
    //     SDLK_Z,
    //     SDLK_S,
    //     SDLK_X,
    //     SDLK_C,
    //     SDLK_F,
    //     SDLK_V,
    //     SDLK_G,
    //     SDLK_B,
    //     SDLK_N,
    //     SDLK_J,
    //     SDLK_M,
    //     SDLK_K,
    //     SDLK_COMMA,
    //     SDLK_L,
    //     SDLK_PERIOD,
    //     SDLK_SLASH
    // };

    // for (unsigned int name {}; const auto key : KEYBOARD) {
    //     if (key != event.key.key) {
    //         continue;
    //     }

    //     if (event.type == SDL_EVENT_KEY_DOWN) {
    //         m_synthesizer.note_on(syn::Name(name), m_octave, m_voice);
    //     } else if (event.type == SDL_EVENT_KEY_UP) {
    //         m_synthesizer.note_off(syn::Name(name), m_octave);
    //     }

    //     name++;
    // }
}
