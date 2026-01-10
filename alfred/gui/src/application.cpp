#include "application.hpp"

#include <algorithm>

#include <SDL3/SDL.h>

static constexpr ImVec2 STEP_SIZE {10.0f, 20.0f};
static constexpr float COMPOSITION_HEIGHT {STEP_SIZE.y * 12.0f * 3.0f + STEP_SIZE.y * 4.0f};

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
    instruments();
    playback();
    tools();
    composition();
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
                case SDLK_E:
                    m_voice = syn::VoiceDrumKick;
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

void Application::instruments() {
    if (ImGui::Begin("Instruments")) {

    }

    ImGui::End();
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
                for (unsigned int i {}; i < 8 * 4 * (STEP / Quarter); i += STEP / Quarter) {
                    const syn::Name name {i % (4 * (STEP / Quarter)) == 0 ? syn::C : syn::D};
                    m_composition.voices[syn::VoiceBell].emplace_back(name, syn::Octave1, Quarter, i);
                }
                m_player.prepare();
            } else {
                m_composition.voices.erase(syn::VoiceBell);
                m_player.prepare();
            }
        }

        ImGui::SameLine();

        ImGui::Text("%f", m_player.get_elapsed_time());

        ImGui::SameLine();

        ImGui::Text("%u", m_player.get_position());
    }

    ImGui::End();
}

void Application::tools() {
    if (ImGui::Begin("Tools")) {

    }

    ImGui::End();
}

void Application::composition() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("Composition")) {
        ImDrawList* list {ImGui::GetWindowDrawList()};
        const ImVec2 origin {ImGui::GetCursorScreenPos()};
        const ImVec2 space_available {ImGui::GetContentRegionAvail()};

        composition_notes(list, origin);

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            m_composition_camera -= ImGui::GetIO().MouseDelta;
        }

        m_composition_camera.x = std::max(m_composition_camera.x, 0.0f);
        m_composition_camera.y = std::max(m_composition_camera.y, 0.0f);
        m_composition_camera.y = std::min(m_composition_camera.y, COMPOSITION_HEIGHT - space_available.y);
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

void Application::composition_notes(ImDrawList* list, ImVec2 origin) {
    list->AddLine(
        origin + ImVec2(40.0f, 0.0f) - ImVec2(0.0f, m_composition_camera.y),
        origin + ImVec2(40.0f, COMPOSITION_HEIGHT) - ImVec2(0.0f, m_composition_camera.y),
        IM_COL32_WHITE
    );

    static const char* NOTES_SCALES[] { "C", "B", "A#", "A", "G#", "G", "F#", "F", "E", "D#", "D", "C#" };
    static const char* NOTES_REMAINING[] { "C", "B", "A#", "A" };

    float position_y {};

    for (int j {}; j < 3; j++) {
        for (std::size_t i {}; i < std::size(NOTES_SCALES); i++) {
            list->AddText(
                origin + ImVec2(0.0f, position_y) - ImVec2(0.0f, m_composition_camera.y),
                IM_COL32_WHITE,
                NOTES_SCALES[i]
            );
            list->AddLine(
                origin + ImVec2(0.0f, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
                origin + ImVec2(40.0f, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
                IM_COL32_WHITE
            );

            position_y += STEP_SIZE.y;
        }
    }

    for (std::size_t i {}; i < std::size(NOTES_REMAINING); i++) {
        list->AddText(
            origin + ImVec2(0.0f, position_y) - ImVec2(0.0f, m_composition_camera.y),
            IM_COL32_WHITE,
            NOTES_REMAINING[i]
        );
        list->AddLine(
            origin + ImVec2(0.0f, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
            origin + ImVec2(40.0f, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
            IM_COL32_WHITE
        );

        position_y += STEP_SIZE.y;
    }
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
}
