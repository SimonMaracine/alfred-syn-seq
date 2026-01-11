#include "application.hpp"

#include <algorithm>

#include <SDL3/SDL.h>

namespace application {
    static constexpr ImVec2 STEP_SIZE {4.0f, 20.0f};
    static constexpr float COMPOSITION_LEFT {40.0f};
    static constexpr float COMPOSITION_HEIGHT {STEP_SIZE.y * 12.0f * float(syn::NOTE_OCTAVES) + STEP_SIZE.y * float(syn::NOTE_EXTRA)};
    static constexpr int ADD_MEASURES {8};

    void Application::on_start() {
        m_synthesizer.resume();

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsClassic();

        m_player = seq::Player(m_synthesizer, m_composition);
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
                        m_octave = syn::Octave1;
                        break;
                    case SDLK_2:
                        m_octave = syn::Octave2;
                        break;
                    case SDLK_3:
                        m_octave = syn::Octave3;
                        break;
                    case SDLK_4:
                        m_octave = syn::Octave4;
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
                    if (m_modified) {
                        m_player.prepare();
                    }

                    m_player.start();
                }
            }

            ImGui::SameLine();

            if (ImGui::Checkbox("Metronome", &m_metronome)) {
                if (m_metronome) {
                    add_metronome();
                    m_modified = true;
                } else {
                    remove_metronome();
                    m_modified = true;
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
            if (ImGui::Button("Add Measures")) {
                if (m_composition.measures.empty()) {
                    for (int i {}; i < ADD_MEASURES; i++) {
                        m_composition.measures.emplace_back();
                    }
                } else {
                    const seq::Tempo tempo {m_composition.measures.back().tempo};
                    const seq::TimeSignature time_signature {m_composition.measures.back().time_signature};

                    for (int i {}; i < ADD_MEASURES; i++) {
                        m_composition.measures.emplace_back(tempo, time_signature);
                    }
                }
            }
        }

        ImGui::End();
    }

    void Application::composition() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        if (ImGui::Begin("Composition")) {
            ImDrawList* list {ImGui::GetWindowDrawList()};
            const ImVec2 origin {ImGui::GetCursorScreenPos()};
            const ImVec2 space_available {ImGui::GetContentRegionAvail()};

            composition_measures(list, origin);
            composition_notes(list, origin);
            composition_cursor(list, origin);
            composition_left(list, origin);

            (void) ImGui::InvisibleButton("Canvas", space_available, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
                m_composition_camera -= ImGui::GetIO().MouseDelta;
            }

            m_composition_camera.x = std::max(m_composition_camera.x, 0.0f);
            m_composition_camera.y = std::max(m_composition_camera.y, 0.0f);
            m_composition_camera.y = std::min(m_composition_camera.y, COMPOSITION_HEIGHT - space_available.y);

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                const ImVec2 position {ImGui::GetIO().MousePos - origin - ImVec2(COMPOSITION_LEFT, 0.0f) + m_composition_camera};

                select_measure(position);
            }
        }

        ImGui::End();

        ImGui::PopStyleVar();
    }

    void Application::composition_left(ImDrawList* list, ImVec2 origin) {
        static constexpr ImVec2 CELL {COMPOSITION_LEFT, STEP_SIZE.y};
        static constexpr ImVec2 TEXT_OFFSET {(CELL.x - 13.0f) / 2.0f, (CELL.y - 13.0f) / 2.0f};

        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR_FOREGROUND {style.Colors[ImGuiCol_Text]};
        const ImColor COLOR_BACKGROUND {style.Colors[ImGuiCol_TableHeaderBg]};

        list->AddRectFilled(
            origin + ImVec2(0.0f, 0.0f) - ImVec2(0.0f, m_composition_camera.y),
            origin + ImVec2(COMPOSITION_LEFT, COMPOSITION_HEIGHT) - ImVec2(0.0f, m_composition_camera.y),
            COLOR_BACKGROUND
        );

        list->AddLine(
            origin + ImVec2(COMPOSITION_LEFT, 0.0f) - ImVec2(0.0f, m_composition_camera.y),
            origin + ImVec2(COMPOSITION_LEFT, COMPOSITION_HEIGHT) - ImVec2(0.0f, m_composition_camera.y),
            COLOR_FOREGROUND
        );

        constexpr const char* NOTES_OCTAVES[] { "C", "B", "A#", "A", "G#", "G", "F#", "F", "E", "D#", "D", "C#" };
        constexpr const char* NOTES_EXTRA[] { "C", "B", "A#", "A" };

        float position_y {};

        for (int j {}; j < syn::NOTE_OCTAVES; j++) {
            for (std::size_t i {}; i < std::size(NOTES_OCTAVES); i++) {
                list->AddText(
                    origin + ImVec2(0.0f, position_y) + TEXT_OFFSET - ImVec2(0.0f, m_composition_camera.y),
                    COLOR_FOREGROUND,
                    NOTES_OCTAVES[i]
                );

                list->AddLine(
                    origin + ImVec2(0.0f, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
                    origin + ImVec2(COMPOSITION_LEFT, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
                    COLOR_FOREGROUND
                );

                position_y += STEP_SIZE.y;
            }
        }

        for (std::size_t i {}; i < std::size(NOTES_EXTRA); i++) {
            list->AddText(
                origin + ImVec2(0.0f, position_y) + TEXT_OFFSET - ImVec2(0.0f, m_composition_camera.y),
                COLOR_FOREGROUND,
                NOTES_EXTRA[i]
            );

            list->AddLine(
                origin + ImVec2(0.0f, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
                origin + ImVec2(COMPOSITION_LEFT, position_y + STEP_SIZE.y) - ImVec2(0.0f, m_composition_camera.y),
                COLOR_FOREGROUND
            );

            position_y += STEP_SIZE.y;
        }
    }

    void Application::composition_measures(ImDrawList* list, ImVec2 origin) {
        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR_FOREGROUND {style.Colors[ImGuiCol_Text]};
        const ImColor COLOR_SELECTION {style.Colors[ImGuiCol_TableHeaderBg]};

        float position_x {COMPOSITION_LEFT};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float width {float(measure->time_signature.measure_steps()) * STEP_SIZE.x};

            if (measure == m_composition_selected_measure) {
                list->AddRectFilled(
                    origin + ImVec2(position_x + 1.0f, 0.0f) - m_composition_camera,
                    origin + ImVec2(position_x + width, COMPOSITION_HEIGHT) - m_composition_camera,
                    COLOR_SELECTION
                );
            }

            position_x += width;

            list->AddLine(
                origin + ImVec2(position_x, 0.0f) - m_composition_camera,
                origin + ImVec2(position_x, COMPOSITION_HEIGHT) - m_composition_camera,
                COLOR_FOREGROUND
            );
        }
    }

    void Application::composition_notes(ImDrawList* list, ImVec2 origin) {
        for (const auto& [voice, notes] : m_composition.voices) {
            const ImColor color {IM_COL32_WHITE};

            for (const seq::Note& note : notes) {
                const float position_x {COMPOSITION_LEFT + float(note.position) * STEP_SIZE.x};
                const float position_y {note_height(note)};

                list->AddRectFilled(
                    origin + ImVec2(position_x, position_y) - m_composition_camera,
                    origin + ImVec2(position_x + float(seq::STEP / note.value) * STEP_SIZE.x, position_y + STEP_SIZE.y) - m_composition_camera,
                    color,
                    4.0f
                );
            }
        }
    }

    void Application::composition_cursor(ImDrawList* list, ImVec2 origin) {
        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR {style.Colors[ImGuiCol_PlotHistogramHovered]};

        const float position_x {COMPOSITION_LEFT + float(m_player.get_position()) * STEP_SIZE.x};

        list->AddLine(
            origin + ImVec2(position_x, 0.0f) - m_composition_camera,
            origin + ImVec2(position_x, COMPOSITION_HEIGHT) - m_composition_camera,
            COLOR
        );
    }

    void Application::debug() {
#ifndef ALFRED_DISTRIBUTION
        if (ImGui::Begin("Debug")) {
            ImGui::Text("Frame time: %f", get_frame_time());
        }

        ImGui::End();
#endif
    }

    void Application::update_keyboard_input(unsigned int key, bool down) {
        const auto update {[this, down](syn::Name name, unsigned int octave) {
            if (down) {
                m_synthesizer.note_on(name, syn::Octave(m_octave + octave), m_voice);
            } else {
                m_synthesizer.note_off(name, syn::Octave(m_octave + octave));
            }
        }};

        switch (key) {
            case SDLK_Z: update(syn::Name::A, 0); break;
            case SDLK_S: update(syn::Name::As, 0); break;
            case SDLK_X: update(syn::Name::B, 0); break;
            case SDLK_C: update(syn::Name::C, 1); break;
            case SDLK_F: update(syn::Name::Cs, 1); break;
            case SDLK_V: update(syn::Name::D, 1); break;
            case SDLK_G: update(syn::Name::Ds, 1); break;
            case SDLK_B: update(syn::Name::E, 1); break;
            case SDLK_N: update(syn::Name::F, 1); break;
            case SDLK_J: update(syn::Name::Fs, 1); break;
            case SDLK_M: update(syn::Name::G, 1); break;
            case SDLK_K: update(syn::Name::Gs, 1); break;
            case SDLK_COMMA: update(syn::Name::A, 1); break;
            case SDLK_L: update(syn::Name::As, 1); break;
            case SDLK_PERIOD: update(syn::Name::B, 1); break;
            case SDLK_SLASH: update(syn::Name::C, 2); break;
        }
    }

    void Application::add_metronome() {
        for (unsigned int position {}; const seq::Measure& measure : m_composition.measures) {
            unsigned int i {};

            for (; i < measure.time_signature.measure_steps(); i += seq::STEP / measure.time_signature.value()) {
                const syn::Name name {i == 0 ? syn::C : syn::D};
                m_composition.voices[syn::VoiceMetronome].emplace_back(name, syn::Octave2, seq::Eighth, position + i);
            }

            position += i;
        }
    }

    void Application::remove_metronome() {
        m_composition.voices.erase(syn::VoiceMetronome);
    }

    void Application::select_measure(ImVec2 position) {
        float position_x {};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float right {float(measure->time_signature.measure_steps()) * STEP_SIZE.x};

            if (position.x > position_x && position.x < position_x + right) {
                if (m_composition_selected_measure == measure) {
                    m_composition_selected_measure = m_composition.measures.end();
                } else {
                    m_composition_selected_measure = measure;
                }

                break;
            }

            position_x += right;
        }
    }

    float Application::note_height(const seq::Note& note) {
        const syn::Id id {syn::Note::get_id(note.name, note.octave)};

        return COMPOSITION_HEIGHT - STEP_SIZE.y - float(id) * STEP_SIZE.y;
    }
}
