#include "application.hpp"

#include <algorithm>
#include <ranges>
#include <charconv>
#include <iterator>
#include <cstring>

#include <SDL3/SDL.h>

namespace application {
    static constexpr ImVec2 STEP_SIZE {4.0f / ui::FONT_SIZE, 20.0f / ui::FONT_SIZE};
    static constexpr float COMPOSITION_LEFT {40.0f / ui::FONT_SIZE};
    static constexpr float COMPOSITION_HEIGHT {STEP_SIZE.y * 12.0f * float(syn::NOTE_OCTAVES) + STEP_SIZE.y * float(syn::NOTE_EXTRA)};
    static constexpr float COMPOSITION_SCROLL_SPEED {26.0f / ui::FONT_SIZE};
    static constexpr int ADD_MEASURES {4};

    void Application::on_start() {
        m_synthesizer.resume();

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsClassic();

        ui::set_scale(1);

        m_player = seq::Player(m_synthesizer, m_composition);

        m_composition_selected_measure = m_composition.measures.end();
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

    void Application::on_late_update() {
        m_task_manager.update();
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
            constexpr const char* SCHEME[] { "Dark", "Light", "Classic" };

            if (ImGui::BeginCombo("##", SCHEME[m_color_scheme], ImGuiComboFlags_WidthFitPreview)) {
                for (std::size_t i {}; i < std::size(SCHEME); i++) {
                    if (ImGui::Selectable(SCHEME[i], m_color_scheme == i)) {
                        m_color_scheme = ui::ColorScheme(i);

                        switch (m_color_scheme) {
                            case ui::ColorSchemeDark:
                                ImGui::StyleColorsDark();
                                break;
                            case ui::ColorSchemeLight:
                                ImGui::StyleColorsLight();
                                break;
                            case ui::ColorSchemeClassic:
                                ImGui::StyleColorsClassic();
                                break;
                        }
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scale")) {
            constexpr const char* SCALE[] { "1X", "2X" };

            if (ImGui::BeginCombo("##", SCALE[m_scale], ImGuiComboFlags_WidthFitPreview)) {
                for (std::size_t i {}; i < std::size(SCALE); i++) {
                    if (ImGui::Selectable(SCALE[i], m_scale == i)) {
                        m_scale = ui::Scale(i);

                        switch (m_scale) {
                            case ui::Scale1X:
                                m_task_manager.add_immediate_task([]() {
                                    ui::set_scale(1);
                                });
                                break;
                            case ui::Scale2X:
                                m_task_manager.add_immediate_task([]() {
                                    ui::set_scale(2);
                                });
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
        static constexpr float CELL {34.0f / ui::FONT_SIZE};
        static constexpr float PADDING {2.0f / ui::FONT_SIZE};
        static constexpr float TEXT_OFFSET {(2.0f * CELL - ui::FONT_SIZE / ui::FONT_SIZE) / 2.0f};
        static constexpr float WIDTH {2.0f * 10.0f * CELL};
        static constexpr float HEIGHT {2.0f * 2.0f * CELL};

        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR_INACTIVE {style.Colors[ImGuiCol_TableBorderLight]};
        const ImColor COLOR_ACTIVE {style.Colors[ImGuiCol_PlotHistogramHovered]};

        const ImVec2 space_available {ImGui::GetContentRegionAvail()};

        const ImVec2 base {(space_available.x - ui::rem(WIDTH)) / 2.0f, (space_available.y - ui::rem(HEIGHT)) / 2.0f};
        const ImVec2 position {x * ui::rem(CELL), y * ui::rem(CELL)};
        const char label[2] { key, '\0' };
        const ImColor color {get_keyboard_state()[scancode] ? COLOR_ACTIVE : COLOR_INACTIVE};

        list->AddRectFilled(
            base + origin + position + ui::rem(ImVec2(PADDING, PADDING)),
            base + origin + position + ImVec2(2.0f * ui::rem(CELL), 2.0f * ui::rem(CELL)) - ui::rem(ImVec2(PADDING, PADDING)),
            color,
            12.0f
        );

        list->AddText(base + origin + position + ui::rem(ImVec2(TEXT_OFFSET, TEXT_OFFSET)), IM_COL32_WHITE, label);
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
                    if (m_composition_modified) {
                        m_player.prepare();
                    }

                    m_player.start();
                }
            }

            ImGui::SameLine();

            if (ImGui::Checkbox("Metronome", &m_metronome)) {
                if (m_metronome) {
                    add_metronome();
                } else {
                    remove_metronome();
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
            if (ImGui::Button("Append")) {
                append_measures();
            }

            ImGui::SameLine();

            if (ImGui::Button("Insert")) {
                insert_measure();
            }

            ImGui::SameLine();

            if (ImGui::Button("Clear")) {
                clear_measure();
            }

            ImGui::SameLine();

            if (ImGui::Button("Delete")) {
                delete_measure();
            }

            ImGui::SameLine();

            if (time_signature()) {
                set_measure_time_signature();
            }

            ImGui::SameLine();

            if (tempo()) {
                set_measure_tempo();
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
            composition_measures_labels(list, origin);
            composition_left(list, origin);

            (void) ImGui::InvisibleButton("Canvas", space_available, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
                m_composition_camera -= ImGui::GetIO().MouseDelta;
            }

            m_composition_camera -= ImVec2(
                -ui::rem(COMPOSITION_SCROLL_SPEED) * ImGui::GetIO().MouseWheelH,
                ui::rem(COMPOSITION_SCROLL_SPEED) * ImGui::GetIO().MouseWheel
            );

            m_composition_camera.x = std::max(m_composition_camera.x, 0.0f);
            m_composition_camera.y = std::max(m_composition_camera.y, 0.0f);
            m_composition_camera.y = std::min(m_composition_camera.y, ui::rem(COMPOSITION_HEIGHT) - space_available.y);

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                const ImVec2 position {ImGui::GetIO().MousePos - origin - ImVec2(ui::rem(COMPOSITION_LEFT), 0.0f) + m_composition_camera};

                select_measure(position);
            }
        }

        ImGui::End();

        ImGui::PopStyleVar();
    }

    void Application::composition_left(ImDrawList* list, ImVec2 origin) const {
        static constexpr ImVec2 CELL {COMPOSITION_LEFT, STEP_SIZE.y};
        static constexpr ImVec2 TEXT_OFFSET {(CELL.x - ui::FONT_SIZE / ui::FONT_SIZE) / 2.0f, (CELL.y - ui::FONT_SIZE / ui::FONT_SIZE) / 2.0f};

        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR_FOREGROUND {style.Colors[ImGuiCol_Text]};
        const ImColor COLOR_BACKGROUND {style.Colors[ImGuiCol_TableHeaderBg]};

        list->AddRectFilled(
            origin + ImVec2(0.0f, 0.0f) - ImVec2(0.0f, m_composition_camera.y),
            origin + ImVec2(ui::rem(COMPOSITION_LEFT), ui::rem(COMPOSITION_HEIGHT)) - ImVec2(0.0f, m_composition_camera.y),
            COLOR_BACKGROUND
        );

        list->AddLine(
            origin + ImVec2(ui::rem(COMPOSITION_LEFT), 0.0f) - ImVec2(0.0f, m_composition_camera.y),
            origin + ImVec2(ui::rem(COMPOSITION_LEFT), ui::rem(COMPOSITION_HEIGHT)) - ImVec2(0.0f, m_composition_camera.y),
            COLOR_FOREGROUND
        );

        constexpr const char* NOTES_OCTAVES[] { "C", "B", "A#", "A", "G#", "G", "F#", "F", "E", "D#", "D", "C#" };
        constexpr const char* NOTES_EXTRA[] { "C", "B", "A#", "A" };

        float position_y {};

        for (int j {}; j < syn::NOTE_OCTAVES; j++) {
            for (std::size_t i {}; i < std::size(NOTES_OCTAVES); i++) {
                list->AddText(
                    origin + ImVec2(0.0f, position_y) + ui::rem(TEXT_OFFSET) - ImVec2(0.0f, m_composition_camera.y),
                    COLOR_FOREGROUND,
                    NOTES_OCTAVES[i]
                );

                list->AddLine(
                    origin + ImVec2(0.0f, position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                    origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                    COLOR_FOREGROUND
                );

                position_y += ui::rem(STEP_SIZE.y);
            }
        }

        for (std::size_t i {}; i < std::size(NOTES_EXTRA); i++) {
            list->AddText(
                origin + ImVec2(0.0f, position_y) + ui::rem(TEXT_OFFSET) - ImVec2(0.0f, m_composition_camera.y),
                COLOR_FOREGROUND,
                NOTES_EXTRA[i]
            );

            list->AddLine(
                origin + ImVec2(0.0f, position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                COLOR_FOREGROUND
            );

            position_y += ui::rem(STEP_SIZE.y);
        }
    }

    void Application::composition_measures(ImDrawList* list, ImVec2 origin) const {
        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR_FOREGROUND {style.Colors[ImGuiCol_Text]};
        const ImColor COLOR_SELECTION {style.Colors[ImGuiCol_TableHeaderBg]};

        float position_x {ui::rem(COMPOSITION_LEFT)};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float width {float(measure->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (measure == m_composition_selected_measure) {
                list->AddRectFilled(
                    origin + ImVec2(position_x + 1.0f, 0.0f) - m_composition_camera,
                    origin + ImVec2(position_x + width, ui::rem(COMPOSITION_HEIGHT)) - m_composition_camera,
                    COLOR_SELECTION
                );
            }

            position_x += width;

            list->AddLine(
                origin + ImVec2(position_x, 0.0f) - m_composition_camera,
                origin + ImVec2(position_x, ui::rem(COMPOSITION_HEIGHT)) - m_composition_camera,
                COLOR_FOREGROUND
            );
        }
    }

    void Application::composition_measures_labels(ImDrawList* list, ImVec2 origin) const {
        static constexpr ImVec2 TEXT_OFFSET {5.0f / ui::FONT_SIZE, 5.0f / ui::FONT_SIZE};

        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR_FOREGROUND {style.Colors[ImGuiCol_Text]};

        float position_x {ui::rem(COMPOSITION_LEFT)};

        for (const auto& [i, measure] : m_composition.measures | std::views::enumerate) {
            char buffer[32] {};

            list->AddText(
                origin + ImVec2(position_x, 0.0f) + ui::rem(TEXT_OFFSET) - ImVec2(m_composition_camera.x, 0.0f),
                COLOR_FOREGROUND,
                measure_label(buffer, i + 1)
            );

            position_x += float(measure.time_signature.measure_steps()) * ui::rem(STEP_SIZE.x);
        }
    }

    void Application::composition_notes(ImDrawList* list, ImVec2 origin) const {
        float global_position_x {ui::rem(COMPOSITION_LEFT)};

        for (const seq::Measure& measure : m_composition.measures) {
            for (const auto& [voice, notes] : measure.voices) {
                const ImColor color {IM_COL32_WHITE};

                for (const seq::Note& note : notes) {
                    const float position_x {float(note.position) * ui::rem(STEP_SIZE.x)};
                    const float position_y {note_height(note)};
                    const float width {float(seq::STEP / note.value) * ui::rem(STEP_SIZE.x)};
                    const float height {ui::rem(STEP_SIZE.y)};

                    list->AddRectFilled(
                        origin + ImVec2(global_position_x + position_x, position_y) - m_composition_camera,
                        origin + ImVec2(global_position_x + position_x + width, position_y + height) - m_composition_camera,
                        color,
                        4.0f
                    );
                }
            }

            global_position_x += float(measure.time_signature.measure_steps()) * ui::rem(STEP_SIZE.x);
        }
    }

    void Application::composition_cursor(ImDrawList* list, ImVec2 origin) const {
        const ImGuiStyle& style {ImGui::GetStyle()};

        const ImColor COLOR {style.Colors[ImGuiCol_PlotHistogramHovered]};

        const float position_x {ui::rem(COMPOSITION_LEFT) + float(m_player.get_position()) * ui::rem(STEP_SIZE.x)};

        list->AddLine(
            origin + ImVec2(position_x, 0.0f) - m_composition_camera,
            origin + ImVec2(position_x, ui::rem(COMPOSITION_HEIGHT)) - m_composition_camera,
            COLOR
        );
    }

    bool Application::tempo() {
        const unsigned int one {1};

        ImGui::SetNextItemWidth(ui::rem(6.0f));

        bool result {};

        if (ImGui::InputScalar("Tempo", ImGuiDataType_U32, &m_tempo, &one)) {
            m_tempo = std::min(std::max(m_tempo, seq::Tempo::MIN), seq::Tempo::MAX);
            result = true;
        }

        return result;
    }

    bool Application::time_signature() {
        constexpr const char* BEATS[] { "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" };
        constexpr const char* VALUE[] { "2", "4", "8", "16" };

        const auto flags {ImGuiComboFlags_HeightSmall | ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_NoArrowButton};

        bool result {};

        ImGui::BeginGroup();

        if (ImGui::BeginCombo("Beats", BEATS[m_time_signature.beats], flags)) {
            for (std::size_t i {}; i < std::size(BEATS); i++) {
                const bool selected {i == m_time_signature.beats};

                if (ImGui::Selectable(BEATS[i], selected)) {
                    m_time_signature.beats = ui::Beats(i);
                    result = true;
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Value", VALUE[m_time_signature.value], flags)) {
            for (std::size_t i {}; i < std::size(VALUE); i++) {
                const bool selected {i == m_time_signature.value};

                if (ImGui::Selectable(VALUE[i], selected)) {
                    m_time_signature.value = ui::Value(i);
                    result = true;
                }
            }

            ImGui::EndCombo();
        }

        ImGui::EndGroup();

        return result;
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
        add_metronome(m_composition.measures.begin(), m_composition.measures.end());
    }

    void Application::add_metronome(MeasureIter begin, MeasureIter end) {
        for (auto measure {begin}; measure != end; measure++) {
            for (unsigned int i {}; i < measure->time_signature.measure_steps(); i += seq::STEP / measure->time_signature.value()) {
                const syn::Name name {i == 0 ? syn::C : syn::D};
                measure->voices[syn::VoiceMetronome].emplace_back(name, syn::Octave2, seq::Eighth, i);
            }
        }

        m_composition_modified = true;
    }

    void Application::remove_metronome() {
        remove_metronome(m_composition.measures.begin(), m_composition.measures.end());
    }

    void Application::remove_metronome(MeasureIter begin, MeasureIter end) {
        for (auto measure {begin}; measure != end; measure++) {
            measure->voices.erase(syn::VoiceMetronome);
        }

        m_composition_modified = true;
    }

    void Application::select_measure(ImVec2 position) {
        float position_x {};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float right {float(measure->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (position.x > position_x && position.x < position_x + right) {
                if (m_composition_selected_measure == measure) {
                    m_composition_selected_measure = m_composition.measures.end();
                } else {
                    m_composition_selected_measure = measure;

                    set_tempo(m_tempo, *measure);
                    set_time_signature(m_time_signature, *measure);
                }

                break;
            }

            position_x += right;
        }
    }

    void Application::append_measures() {
        const auto [tempo, time_signature] {measure_type(
            !m_composition.measures.empty() ? std::prev(m_composition.measures.end()) : m_composition.measures.end(),
            m_composition.measures
        )};

        for (int i {}; i < ADD_MEASURES; i++) {
            m_composition.measures.emplace_back(tempo, time_signature);
            m_composition_selected_measure = m_composition.measures.end();
        }

        if (m_metronome) {
            const auto begin {std::prev(m_composition.measures.end(), ADD_MEASURES)};
            const auto end {m_composition.measures.end()};

            add_metronome(begin, end);
        }

        m_composition_modified = true;
    }

    void Application::insert_measure() {
        if (m_composition_selected_measure == m_composition.measures.end()) {
            return;
        }

        const auto [tempo, time_signature] {measure_type(
            m_composition_selected_measure,
            m_composition.measures
        )};

        m_composition_selected_measure = m_composition.measures.emplace(m_composition_selected_measure, tempo, time_signature);

        if (m_metronome) {
            add_metronome(m_composition_selected_measure, std::next(m_composition_selected_measure));
        }

        m_composition_modified = true;
    }

    void Application::clear_measure() {
        if (m_composition_selected_measure == m_composition.measures.end()) {
            return;
        }

        std::erase_if(m_composition_selected_measure->voices, [](const auto& voice) {
            return voice.first != syn::VoiceMetronome;
        });

        m_composition_modified = true;
    }

    void Application::delete_measure() {
        if (m_composition_selected_measure == m_composition.measures.end()) {
            return;
        }

        m_composition_selected_measure = m_composition.measures.erase(m_composition_selected_measure);

        m_composition_modified = true;
    }

    void Application::set_measure_tempo() {
        if (m_composition_selected_measure == m_composition.measures.end()) {
            return;
        }

        set_tempo(*m_composition_selected_measure, m_tempo);

        m_composition_modified = true;
    }

    void Application::set_measure_time_signature() {
        if (m_composition_selected_measure == m_composition.measures.end()) {
            return;
        }

        if (empty_except_metronome(*m_composition_selected_measure)) {
            set_time_signature(*m_composition_selected_measure, m_time_signature);

            if (m_metronome) {
                remove_metronome(m_composition_selected_measure, std::next(m_composition_selected_measure));
                add_metronome(m_composition_selected_measure, std::next(m_composition_selected_measure));
            }
        } else {
            // Reset back
            set_time_signature(m_time_signature, *m_composition_selected_measure);
        }

        m_composition_modified = true;
    }

    void Application::delete_notes(syn::Voice voice, unsigned int begin, unsigned int end) {

    }

    void Application::delete_notes(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end) {

    }

    void Application::shift_notes_left(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end, unsigned int steps) {

    }

    void Application::shift_notes_right(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end, unsigned int steps) {

    }

    float Application::note_height(const seq::Note& note) {
        const syn::Id id {syn::Note::get_id(note.name, note.octave)};

        return ui::rem(COMPOSITION_HEIGHT) - ui::rem(STEP_SIZE.y) - float(id) * ui::rem(STEP_SIZE.y);
    }

    const char* Application::measure_label(char* buffer, long number) {
        std::to_chars_result result {std::to_chars(buffer, buffer + sizeof(buffer), number)};

        if (result.ec != std::errc()) {
            std::strcpy(buffer, "?");
        } else {
            *result.ptr = '\0';
        }

        return buffer;
    }

    std::pair<seq::Tempo, seq::TimeSignature> Application::measure_type(MeasureIter iter, const std::vector<seq::Measure>& measures) {
        seq::Tempo tempo;
        seq::TimeSignature time_signature;

        if (iter != measures.end()) {
            tempo = iter->tempo;
            time_signature = iter->time_signature;
        }

        return { tempo, time_signature };
    }

    void Application::set_tempo(seq::Measure& measure, const ui::Tempo& tempo) {
        measure.tempo = tempo;
    }

    void Application::set_tempo(ui::Tempo& tempo, const seq::Measure& measure) {
        tempo = measure.tempo;
    }

    void Application::set_time_signature(seq::Measure& measure, const ui::TimeSignature& time_signature) {
        seq::Beats beats {};
        seq::Value value {};

        switch (time_signature.beats) {
            case ui::Beats2: beats = 2; break;
            case ui::Beats3: beats = 3; break;
            case ui::Beats4: beats = 4; break;
            case ui::Beats5: beats = 5; break;
            case ui::Beats6: beats = 6; break;
            case ui::Beats7: beats = 7; break;
            case ui::Beats8: beats = 8; break;
            case ui::Beats9: beats = 9; break;
            case ui::Beats10: beats = 10; break;
            case ui::Beats11: beats = 11; break;
            case ui::Beats12: beats = 12; break;
            case ui::Beats13: beats = 13; break;
            case ui::Beats14: beats = 14; break;
            case ui::Beats15: beats = 15; break;
            case ui::Beats16: beats = 16; break;
        }

        switch (time_signature.value) {
            case ui::Value2: value = seq::Half; break;
            case ui::Value4: value = seq::Quarter; break;
            case ui::Value8: value = seq::Eighth; break;
            case ui::Value16: value = seq::Sixteenth; break;
        }

        measure.time_signature = seq::TimeSignature(beats, value);
    }

    void Application::set_time_signature(ui::TimeSignature& time_signature, const seq::Measure& measure) {
        switch (measure.time_signature.beats()) {
            case 2: time_signature.beats = ui::Beats2; break;
            case 3: time_signature.beats = ui::Beats3; break;
            case 4: time_signature.beats = ui::Beats4; break;
            case 5: time_signature.beats = ui::Beats5; break;
            case 6: time_signature.beats = ui::Beats6; break;
            case 7: time_signature.beats = ui::Beats7; break;
            case 8: time_signature.beats = ui::Beats8; break;
            case 9: time_signature.beats = ui::Beats9; break;
            case 10: time_signature.beats = ui::Beats10; break;
            case 11: time_signature.beats = ui::Beats11; break;
            case 12: time_signature.beats = ui::Beats12; break;
            case 13: time_signature.beats = ui::Beats13; break;
            case 14: time_signature.beats = ui::Beats14; break;
            case 15: time_signature.beats = ui::Beats15; break;
            case 16: time_signature.beats = ui::Beats16; break;
        }

        switch (measure.time_signature.value()) {
            case seq::Whole: assert(false); break;
            case seq::Half: time_signature.value = ui::Value2; break;
            case seq::Quarter: time_signature.value = ui::Value4; break;
            case seq::Eighth: time_signature.value = ui::Value8; break;
            case seq::Sixteenth: time_signature.value = ui::Value16; break;
        }
    }

    bool Application::empty_except_metronome(const seq::Measure& measure) {
        return measure.voices.empty() || measure.voices.size() == 1 && measure.voices.count(syn::VoiceMetronome) == 1;
    }
}
