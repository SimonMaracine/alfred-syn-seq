#include "application.hpp"

#include <algorithm>
#include <ranges>
#include <charconv>
#include <iterator>
#include <numeric>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cassert>

#include <SDL3/SDL.h>
#include <alfred/instrument.hpp>
#include <alfred/math.hpp>

#include "imgui.hpp"
#include "logging.hpp"
#include "imgui.ini.hpp"
#include "icon64.png.hpp"
#include "icon128.png.hpp"
#include "play.png.hpp"
#include "pause.png.hpp"
#include "rewind.png.hpp"

namespace application {
    static constexpr ImVec2 STEP_SIZE {1.0f / ui::FONT_SIZE, 20.0f / ui::FONT_SIZE};
    static constexpr float COMPOSITION_LEFT {40.0f / ui::FONT_SIZE};
    static constexpr float COMPOSITION_HEIGHT {STEP_SIZE.y * 12.0f * float(syn::keyboard::OCTAVES) + STEP_SIZE.y * float(syn::keyboard::EXTRA)};
    static constexpr float COMPOSITION_SCROLL_SPEED {40.0f / ui::FONT_SIZE};
    static constexpr int ADD_MEASURES {4};
    static constexpr unsigned long long FRAME_TIME_DEFAULT {16};
    static constexpr unsigned long long FRAME_TIME_PLAYBACK {4};

    void Application::on_start() {
        set_desired_frame_time(FRAME_TIME_DEFAULT);

        try {
            set_icons({ ICON64, ICON128 });
        } catch (const video::VideoError& e) {
            logging::error("Could not set icon: {}", e.what());
        }

        try {
            utility::Buffer buffer;
            utility::read_file(utility::data_file_path() / "alfred.dat", buffer);
            data::import_data(m_data, buffer);
        } catch (const data::DataError& e) {
            logging::warning("Could not import data: {}", e.what());
        } catch (const utility::FilerError& e) {
            logging::warning("Could not import data: {}", e.what());
        }

        m_synthesizer.open();
        m_synthesizer.resume();
        m_synthesizer.volume(0.75);

        ImGui::LoadIniSettingsFromMemory(SETTINGS.data(), SETTINGS.size());

        set_color_scheme(m_data.color_scheme);
        set_scale(m_data.scale);

        if (m_data.scale == ui::Scale2X) {
            set_window_size(1920, 1080);
            LOG_INFORMATION("Double scale");
        }

        auto& io {ImGui::GetIO()};
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigWindowsResizeFromEdges = false;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = nullptr;

        m_instrument = instrument::Piano::static_id();
        m_player = seq::Player(m_synthesizer, m_composition, [this] { set_desired_frame_time(FRAME_TIME_DEFAULT); });
        m_composition_selected_measure = m_composition.measures.end();

        m_ui.volume = m_synthesizer.volume();
        m_ui.device = m_synthesizer.device().second;
        m_ui.texture_play = image::Texture(m_renderer, image::Surface(PLAY));
        m_ui.texture_pause = image::Texture(m_renderer, image::Surface(PAUSE));
        m_ui.texture_rewind = image::Texture(m_renderer, image::Surface(REWIND));

        m_task_manager.add_repeatable_task([this] {
            m_ui.device = m_synthesizer.device().second;
            return false;
        }, 5000);

        m_synthesizer.for_each_instrument([this, index = std::size_t()](const auto& instrument) mutable {
            m_ui.colors[instrument.id()] = ui::ColorIndex(index);
            index = (index + 1) % std::size(ui::COLORS);
        });
    }

    void Application::on_stop() {
        try {
            utility::Buffer buffer;
            data::export_data(m_data, buffer);
            utility::write_file(utility::data_file_path() / "alfred.dat", buffer);
        } catch (const data::DataError& e) {
            logging::warning("Could not export data: {}", e.what());
        } catch (const utility::FilerError& e) {
            logging::warning("Could not export data: {}", e.what());
        }
    }

    void Application::on_update() {
        m_player.update(get_frame_time());
        m_synthesizer.update_voices();
    }

    void Application::on_imgui() {
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoUndocking);
        ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Tab, ImGuiInputFlags_RouteGlobal);

        main_menu_bar();
        keyboard();
        instruments();
        output();
        playback();
        tools();
        composition();
        shortcuts();

#ifndef ALFRED_DISTRIBUTION
        debug();
        ImGui::ShowDemoWindow();
#endif
    }

    void Application::on_late_update() {
        m_task_manager.update();
    }

    void Application::on_event(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (keyboard_active() && !event.key.repeat) {
                    keyboard_input(event.key.key, true);
                }
                break;
            case SDL_EVENT_KEY_UP:
                keyboard_input(event.key.key, false);  // Don't block note offs
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

            if (ImGui::BeginMenu("Composition")) {
                main_menu_bar_composition();
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
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            file_new();
        }

        if (ImGui::BeginMenu("Open Recent")) {
            for (const auto& [i, file_path] : m_data.recent_compositions | std::views::enumerate) {
                ImGui::PushID(int(i));

                if (ImGui::MenuItem(file_path.c_str())) {
                    if (!composition_open(file_path)) {
                        m_task_manager.add_immediate_task([this, &file_path] {
                            m_data.recent_compositions.erase(file_path);
                            logging::information("Erased invalid composition from the list");
                        });
                    }
                }

                ImGui::PopID();
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            file_open();
        }

        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            file_save();
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

    void Application::main_menu_bar_composition() {
        if (ImGui::BeginMenu("Title")) {
            if (ImGui::InputText("##title", m_ui.composition.title, sizeof(m_ui.composition.title))) {
                m_composition.title = m_ui.composition.title;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Author")) {
            if (ImGui::InputText("##author", m_ui.composition.author, sizeof(m_ui.composition.author))) {
                m_composition.author = m_ui.composition.author;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Year")) {
            if (ImGui::InputScalar("##year", ImGuiDataType_S16, &m_ui.composition.year)) {
                m_composition.year = std::chrono::year(m_ui.composition.year);
            }

            ImGui::EndMenu();
        }
    }

    void Application::main_menu_bar_help() {
        if (ImGui::BeginMenu("About")) {
            const char* link {get_property(SDL_PROP_APP_METADATA_URL_STRING)};
            ImGui::TextLinkOpenURL(link, link);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Build")) {
#ifdef ALFRED_DISTRIBUTION
            constexpr const char* DEV_TAG {""};
#else
            constexpr const char* DEV_TAG {" dev"};
#endif
            ImGui::Text("Version: %s%s", get_property(SDL_PROP_APP_METADATA_VERSION_STRING), DEV_TAG);
#ifdef ALFRED_LINUX
            ImGui::Text("Compiler: GCC %d.%d", __GNUC__, __GNUC_MINOR__);
#elifdef ALFRED_WINDOWS
            ImGui::Text("Compiler: MSVC %d", _MSC_VER);
#endif
            ImGui::Text("Date: %s %s", __DATE__, __TIME__);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Driver")) {
            ImGui::Text("%s", m_synthesizer.driver());

            ImGui::EndMenu();
        }
    }

    void Application::main_menu_bar_options() {
        if (ImGui::BeginMenu("Color Scheme")) {
            constexpr const char* SCHEME[] { "Dark", "Light", "Classic" };

            if (ImGui::BeginCombo("##", SCHEME[m_data.color_scheme], ImGuiComboFlags_WidthFitPreview)) {
                for (std::size_t i {}; i < std::size(SCHEME); i++) {
                    if (ImGui::Selectable(SCHEME[i], m_data.color_scheme == i)) {
                        m_data.color_scheme = ui::ColorScheme(i);
                        set_color_scheme(m_data.color_scheme);
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scale")) {
            constexpr const char* SCALE[] { "1X", "2X" };

            if (ImGui::BeginCombo("##", SCALE[m_data.scale], ImGuiComboFlags_WidthFitPreview)) {
                for (std::size_t i {}; i < std::size(SCALE); i++) {
                    if (ImGui::Selectable(SCALE[i], m_data.scale == i)) {
                        m_data.scale = ui::Scale(i);
                        m_task_manager.add_immediate_task([this] {
                            set_scale(m_data.scale);
                        });
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::EndMenu();
        }
    }

    void Application::keyboard() const {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        if (ImGui::Begin("Keyboard", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            const Draw draw {ImGui::GetWindowDrawList(), ImGui::GetCursorScreenPos(), {}, {}};

            keyboard_key(draw, 'S', 1.0f, 0.0f, SDL_SCANCODE_S);
            keyboard_key(draw, 'F', 5.0f, 0.0f, SDL_SCANCODE_F);
            keyboard_key(draw, 'G', 7.0f, 0.0f, SDL_SCANCODE_G);
            keyboard_key(draw, 'J', 11.0f, 0.0f, SDL_SCANCODE_J);
            keyboard_key(draw, 'K', 13.0f, 0.0f, SDL_SCANCODE_K);
            keyboard_key(draw, 'L', 15.0f, 0.0f, SDL_SCANCODE_L);

            keyboard_key(draw, 'Z', 0.0f, 2.0f, SDL_SCANCODE_Z);
            keyboard_key(draw, 'X', 2.0f, 2.0f, SDL_SCANCODE_X);
            keyboard_key(draw, 'C', 4.0f, 2.0f, SDL_SCANCODE_C);
            keyboard_key(draw, 'V', 6.0f, 2.0f, SDL_SCANCODE_V);
            keyboard_key(draw, 'B', 8.0f, 2.0f, SDL_SCANCODE_B);
            keyboard_key(draw, 'N', 10.0f, 2.0f, SDL_SCANCODE_N);
            keyboard_key(draw, 'M', 12.0f, 2.0f, SDL_SCANCODE_M);
            keyboard_key(draw, ',', 14.0f, 2.0f, SDL_SCANCODE_COMMA);
            keyboard_key(draw, '.', 16.0f, 2.0f, SDL_SCANCODE_PERIOD);
            keyboard_key(draw, '/', 18.0f, 2.0f, SDL_SCANCODE_SLASH);
        }

        ImGui::End();

        ImGui::PopStyleVar();
    }

    void Application::keyboard_key(const Draw& draw, char key, float x, float y, int scancode) const {
        static constexpr float CELL {30.0f / ui::FONT_SIZE};
        static constexpr float PADDING {2.0f / ui::FONT_SIZE};
        static constexpr float TEXT_OFFSET {(2.0f * CELL - 1.0f) / 2.0f};
        static constexpr float WIDTH {2.0f * 10.0f * CELL};
        static constexpr float HEIGHT {2.0f * 2.0f * CELL};

        const ImColor& COLOR_TEXT {color(ImGuiCol_Text)};
        const ImColor& COLOR_INACTIVE {color(ImGuiCol_TableBorderLight)};
        const ImColor& COLOR_ACTIVE {color(ImGuiCol_PlotHistogramHovered)};

        const ImVec2 space {ImGui::GetContentRegionAvail()};

        const ImVec2 base {(space.x - ui::rem(WIDTH)) / 2.0f, (space.y - ui::rem(HEIGHT)) / 2.0f};
        const ImVec2 position {x * ui::rem(CELL), y * ui::rem(CELL)};
        const char label[2] { key, '\0' };
        ImColor color {get_keyboard_state()[scancode] ? COLOR_ACTIVE : COLOR_INACTIVE};

        // Just override when keyboard is disabled
        if (!keyboard_active()) {
            color = COLOR_INACTIVE;
        }

        draw.list->AddRectFilled(
            base + draw.origin + position + ui::rem(ImVec2(PADDING, PADDING)),
            base + draw.origin + position + ImVec2(2.0f * ui::rem(CELL), 2.0f * ui::rem(CELL)) - ui::rem(ImVec2(PADDING, PADDING)),
            color,
            12.0f
        );

        draw.list->AddText(base + draw.origin + position + ui::rem(ImVec2(TEXT_OFFSET, TEXT_OFFSET)), COLOR_TEXT, label);
    }

    void Application::instruments() {
        if (ImGui::Begin("Instruments", nullptr, ImGuiWindowFlags_NoResize)) {
            ImGui::SeparatorText("Instrument");

            if (ImGui::BeginCombo("##instrument", m_synthesizer.get_instrument(m_instrument).name(), ImGuiComboFlags_NoArrowButton)) {
                m_synthesizer.for_each_instrument([this](const syn::Instrument& instrument) {
                    if (instrument.id() == instrument::Metronome::static_id()) {
                        return;
                    }

                    if (ImGui::Selectable(instrument.name(), instrument.id() == m_instrument)) {
                        m_instrument = instrument.id();
                        m_composition_selected_notes.clear();
                        m_synthesizer.silence();
                    }
                });

                ImGui::EndCombo();
            }

            ImGui::Dummy(ui::rem(ImVec2(0.0f, 1.0f)));

            ImGui::SeparatorText("Octave");

            if (ImGui::SliderInt("##octave", &m_ui.octave, ui::Octave1, ui::Octave5)) {
                m_octave = syn::keyboard::Octave(m_ui.octave - 1);
                m_synthesizer.silence();
            }

            ImGui::Dummy(ui::rem(ImVec2(0.0f, 1.0f)));

            ImGui::SeparatorText("In Project");

            if (ImGui::BeginListBox("##in_project")) {
                for (const auto instruments {instruments_in_project()}; const syn::InstrumentId instrument : instruments) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ui::COLORS[m_ui.colors.at(instrument)].second));

                    if (ImGui::Selectable(m_synthesizer.get_instrument(instrument).name(), instrument == m_instrument)) {
                        m_instrument = instrument;
                        m_composition_selected_notes.clear();
                        m_synthesizer.silence();
                    }

                    ImGui::PopStyleColor();
                }

                ImGui::EndListBox();
            }

            ImGui::Dummy(ui::rem(ImVec2(0.0f, 1.0f)));

            ImGui::SeparatorText("Color");

            if (ImGui::BeginCombo("##color", ui::COLORS[m_ui.colors.at(m_instrument)].first, ImGuiComboFlags_NoArrowButton)) {
                for (std::size_t i {}; i < std::size(ui::COLORS); i++) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ui::COLORS[i].second));

                    if (ImGui::Selectable(ui::COLORS[i].first, m_ui.colors.at(m_instrument) == i)) {
                        m_ui.colors.at(m_instrument) = ui::ColorIndex(i);
                    }

                    ImGui::PopStyleColor();
                }

                ImGui::EndCombo();
            }
        }

        ImGui::End();
    }

    void Application::output() {
        constexpr double zero {0.0};
        constexpr double one {1.0};

        if (ImGui::Begin("Output", nullptr, ImGuiWindowFlags_NoResize)) {
            ImGui::SeparatorText("Volume");

            if (ImGui::SliderScalar("##volume", ImGuiDataType_Double, &m_ui.volume, &zero, &one, "%.2f")) {
                m_synthesizer.volume(m_ui.volume);
            }

            ImGui::Dummy(ui::rem(ImVec2(0.0f, 1.0f)));

            ImGui::SeparatorText("Device");

            ImGui::TextWrapped("%s", m_ui.device);
        }

        ImGui::End();
    }

    void Application::playback() {
        const ImVec2 SIZE {ui::rem(ImVec2(2.461538f, 2.461538f))};
        static constexpr ImVec2 UV0 {0.0f, 0.0f};
        static constexpr ImVec2 UV1 {1.0f, 1.0f};
        static constexpr ImVec4 COLOR_BACKGROUND {0.0f, 0.0f, 0.0f, 0.0f};
        const ImVec4& COLOR_FOREGROUND {color(ImGuiCol_Text)};

        if (ImGui::Begin("Playback", nullptr, ImGuiWindowFlags_NoResize)) {
            if (ImGui::ImageButton("Rewind", reinterpret_cast<ImTextureID>(m_ui.texture_rewind.get()), SIZE, UV0, UV1, COLOR_BACKGROUND, COLOR_FOREGROUND)) {
                m_player.seek(0);
            }

            ImGui::SetItemTooltip("Rewind the player to the beginning (R)");

            ImGui::SameLine();

            if (m_player.is_playing()) {
                if (ImGui::ImageButton("Pause", reinterpret_cast<ImTextureID>(m_ui.texture_pause.get()), SIZE, UV0, UV1, COLOR_BACKGROUND, COLOR_FOREGROUND)) {
                    stop_player();
                }
            } else {
                if (ImGui::ImageButton("Play", reinterpret_cast<ImTextureID>(m_ui.texture_play.get()), SIZE, UV0, UV1, COLOR_BACKGROUND, COLOR_FOREGROUND)) {
                    start_player();
                }
            }

            ImGui::SetItemTooltip("Play/Pause the player (Space)");

            ImGui::SameLine();

            ImGui::BeginDisabled(m_player.is_playing());

            if (ImGui::Checkbox("Metronome", &m_metronome)) {
                if (const bool allow_edit {!m_player.is_playing()}; allow_edit) {
                    if (m_metronome) {
                        add_metronome();
                    } else {
                        remove_metronome();
                    }
                } else {
                    m_metronome = !m_metronome;
                }
            }

            ImGui::SetItemTooltip("The metronome is a pseudo-instrument not part of the composition");

            ImGui::EndDisabled();

            ImGui::SameLine();

            const Time time {elapsed_seconds_to_time(m_player.get_elapsed_time())};

            if (m_player.is_in_time()) {
                ImGui::Text("%02d:%02d.%d", time.minutes, time.seconds, time.deciseconds);
            } else {
                const ImColor& COLOR {color(ImGuiCol_PlotLinesHovered)};

                ImGui::TextColored(COLOR, "%02d:%02d.%d", time.minutes, time.seconds, time.deciseconds);
            }
        }

        ImGui::End();
    }

    void Application::tools() {
        if (ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoResize)) {
            ImGui::BeginGroup();

            if (ImGui::RadioButton("Measure", &m_ui.tool, ui::ToolMeasure)) {
                m_composition_selected_notes.clear();
            }

            if (ImGui::RadioButton("Note", &m_ui.tool, ui::ToolNote)) {
                m_composition_selected_measure = m_composition.measures.end();
            }

            ImGui::EndGroup();

            ImGui::SetItemTooltip("Change the editing tool (Tab)");

            ImGui::SameLine();

            ImGui::Dummy(ImVec2(ui::rem(1.0f), 0.0f));

            ImGui::SameLine();

            switch (m_ui.tool) {
                case ui::ToolMeasure:
                    tools_measure();
                    break;
                case ui::ToolNote:
                    tools_note();
                    break;
            }
        }

        ImGui::End();
    }

    void Application::tools_measure() {
        ImGui::BeginDisabled(m_player.is_playing());

        ImGui::BeginGroup();

        if (ImGui::Button("Append")) {
            append_measures();
        }

        ImGui::SetItemTooltip("Append a couple of measures to the end of the composition (Alt+A)");

        ImGui::BeginDisabled(m_composition_selected_measure == m_composition.measures.end());

        if (ImGui::Button("Insert")) {
            insert_measure();
        }

        ImGui::SetItemTooltip("Insert a measure before the selected measure (Alt+I)");

        ImGui::EndDisabled();

        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();

        ImGui::BeginDisabled(m_composition_selected_measure == m_composition.measures.end());

        if (ImGui::Button("Clear")) {
            clear_measure();
        }

        ImGui::SetItemTooltip("Clear the selected measure (Backspace)");

        if (ImGui::Button("Delete")) {
            delete_measure();
        }

        ImGui::SetItemTooltip("Completely delete the selected measure (Delete)");

        ImGui::EndDisabled();

        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();

        ImGui::BeginDisabled(m_composition_selected_measure == m_composition.measures.end());

        if (time_signature()) {
            set_measure_time_signature();
        }

        ImGui::EndDisabled();

        ImGui::SameLine();

        if (tempo()) {
            set_measures_tempo();
        }

        ImGui::EndGroup();

        ImGui::EndDisabled();
    }

    void Application::tools_note() {
        ImGui::BeginDisabled(m_player.is_playing() || m_composition_selected_notes.empty());

        ImGui::BeginGroup();

        ImGui::BeginGroup();

        ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);

        if (ImGui::ArrowButton("Shift Up", ImGuiDir_Up)) {
            shift_notes_up();
        }

        ImGui::SetItemTooltip("Shift the selected notes up (Alt+W)");

        ImGui::SameLine();

        if (ImGui::ArrowButton("Shift Down", ImGuiDir_Down)) {
            shift_notes_down();
        }

        ImGui::SetItemTooltip("Shift the selected notes down (Alt+S)");

        if (ImGui::ArrowButton("Shift Left", ImGuiDir_Left)) {
            shift_notes_left();
        }

        ImGui::SetItemTooltip("Shift the selected notes left (Alt+A)");

        ImGui::SameLine();

        if (ImGui::ArrowButton("Shift Right", ImGuiDir_Right)) {
            shift_notes_right();
        }

        ImGui::SetItemTooltip("Shift the selected notes right (Alt+D)");

        ImGui::PopItemFlag();

        ImGui::EndGroup();

        ImGui::SameLine();

        if (ImGui::Button("Delete")) {
            delete_notes();
        }

        ImGui::SetItemTooltip("Completely delete the selected notes (Delete)");

        ImGui::SameLine();

        if (ImGui::Button("Legato")) {
            legato_notes();
        }

        ImGui::SetItemTooltip("Toggle the selected notes' legato");

        ImGui::EndGroup();

        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginGroup();

        ImGui::RadioButton("Whole", &m_ui.value, ui::ValueWhole);

        ImGui::SameLine();

        ImGui::RadioButton("Half", &m_ui.value, ui::ValueHalf);

        ImGui::SameLine();

        ImGui::RadioButton("Quarter", &m_ui.value, ui::ValueQuarter);

        ImGui::RadioButton("Eighth", &m_ui.value, ui::ValueEighth);

        ImGui::SameLine();

        ImGui::RadioButton("Sixteenth", &m_ui.value, ui::ValueSixteenth);

        ImGui::EndGroup();

        ImGui::SetItemTooltip("Change the note value (1-5)");
    }

    void Application::composition() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        if (ImGui::Begin("Composition", nullptr, ImGuiWindowFlags_NoResize)) {
            const ImVec2 origin {ImGui::GetCursorScreenPos()};
            const ImVec2 space {ImGui::GetContentRegionAvail()};
            const ImVec2 clamped_space {composition_space(space)};
            const Draw draw {ImGui::GetWindowDrawList(), origin, space, clamped_space};

            (void) ImGui::InvisibleButton("Canvas", space, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

            const bool item_active {ImGui::IsItemActive()};
            const bool item_hovered {ImGui::IsItemHovered()};
            const bool allow_edit {!m_player.is_playing()};

            if (!ImGui::IsKeyDown(ImGuiMod_Alt)) {
                if (const auto hovered_note {hover_note(composition_mouse_position(origin))}; item_hovered && allow_edit && hovered_note) {
                    composition_hover(draw, *hovered_note);
                }
            }

            composition_camera(item_active, item_hovered, space);
            composition_measures(draw);
            composition_octaves(draw);
            composition_cursor(draw);
            composition_notes(draw);
            composition_measures_labels(draw);
            composition_left(draw);

            if (item_hovered && allow_edit) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    composition_mouse_pressed(origin);
                }
            }
        }

        ImGui::End();

        ImGui::PopStyleVar();
    }

    void Application::composition_left(const Draw& draw) const {
        static constexpr ImVec2 CELL {COMPOSITION_LEFT, STEP_SIZE.y};
        static constexpr ImVec2 TEXT_OFFSET {(CELL.x - 2.0f) / 2.0f, (CELL.y - 1.0f) / 2.0f};

        const ImColor& COLOR_FOREGROUND {color(ImGuiCol_Text)};
        const ImColor COLOR_BACKGROUND {color_opacity(ImGuiCol_WindowBg, 1.0f)};

        draw.list->AddRectFilled(
            draw.origin + ImVec2(0.0f, 0.0f),
            draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), draw.clamped_space.y),
            COLOR_BACKGROUND
        );

        draw.list->AddLine(
            draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), 0.0f),
            draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), draw.clamped_space.y),
            COLOR_FOREGROUND
        );

        constexpr const char* NOTES_OCTAVES[] { "C \0", "B \0", "A #", "A \0", "G #", "G \0", "F #", "F \0", "E \0", "D #", "D \0", "C #" };
        constexpr const char* NOTES_EXTRA[] { "C \0", "B \0", "A #", "A \0" };

        float position_y {};
        int octave {7};

        for (int j {}; j < syn::keyboard::OCTAVES; j++) {
            for (std::size_t i {}; i < std::size(NOTES_OCTAVES); i++) {
                if (point_y_in_camera_view(position_y + ui::rem(STEP_SIZE.y), draw.space.y + ui::rem(STEP_SIZE.y))) {
                    char buffer[4] {};
                    std::strcpy(buffer, NOTES_OCTAVES[i]);
                    buffer[1] = char(octave + 48);

                    draw.list->AddText(
                        draw.origin + ImVec2(0.0f, position_y) + ui::rem(TEXT_OFFSET) - ImVec2(0.0f, m_composition_camera.y),
                        COLOR_FOREGROUND,
                        buffer
                    );

                    draw.list->AddLine(
                        draw.origin + ImVec2(0.0f, position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                        draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                        COLOR_FOREGROUND
                    );
                }

                position_y += ui::rem(STEP_SIZE.y);
                octave -= i == 0 ? 1 : 0;
            }
        }

        for (std::size_t i {}; i < std::size(NOTES_EXTRA); i++) {
            if (point_y_in_camera_view(position_y + ui::rem(STEP_SIZE.y), draw.space.y + ui::rem(STEP_SIZE.y))) {
                char buffer[4] {};
                std::strcpy(buffer, NOTES_EXTRA[i]);
                buffer[1] = char(octave + 48);

                draw.list->AddText(
                    draw.origin + ImVec2(0.0f, position_y) + ui::rem(TEXT_OFFSET) - ImVec2(0.0f, m_composition_camera.y),
                    COLOR_FOREGROUND,
                    buffer
                );

                draw.list->AddLine(
                    draw.origin + ImVec2(0.0f, position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                    draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                    COLOR_FOREGROUND
                );
            }

            position_y += ui::rem(STEP_SIZE.y);
            octave -= i == 0 ? 1 : 0;
        }
    }

    void Application::composition_octaves(const Draw& draw) const {
        const ImColor COLOR {color_opacity(ImGuiCol_TextDisabled, 0.7f)};

        float position_y {float(syn::keyboard::EXTRA) * ui::rem(STEP_SIZE.y)};

        if (point_y_in_camera_view(position_y, draw.space.y)) {
            draw.list->AddLine(
                draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y) - ImVec2(0.0f, m_composition_camera.y),
                draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT) + draw.clamped_space.x, position_y) - ImVec2(0.0f, m_composition_camera.y),
                COLOR
            );
        }

        // Draw one additional line
        for (int i {1}; i <= syn::keyboard::OCTAVES; i++) {
            position_y += 12.0f * ui::rem(STEP_SIZE.y);

            if (!point_y_in_camera_view(position_y, draw.space.y)) {
                continue;
            }

            draw.list->AddLine(
                draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y) - ImVec2(0.0f, m_composition_camera.y),
                draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT) + draw.clamped_space.x, position_y) - ImVec2(0.0f, m_composition_camera.y),
                COLOR
            );
        }
    }

    void Application::composition_measures(const Draw& draw) const {
        const ImColor& COLOR_FOREGROUND {color(ImGuiCol_Text)};
        const ImColor COLOR_FOREGROUND2 {color_opacity(ImGuiCol_TextDisabled, 0.7f)};
        const ImColor COLOR_SELECTION {color_opacity(ImGuiCol_TableHeaderBg, 0.3f)};

        float position_x {ui::rem(COMPOSITION_LEFT)};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float width {float(measure->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (measure == m_composition_selected_measure) {
                draw.list->AddRectFilled(
                    draw.origin + ImVec2(position_x + 1.0f, 0.0f) - ImVec2(m_composition_camera.x, 0.0f),
                    draw.origin + ImVec2(position_x + width, draw.clamped_space.y) - ImVec2(m_composition_camera.x, 0.0f),
                    COLOR_SELECTION
                );
            }

            const bool in_view {point_x_in_camera_view(position_x + width, draw.space.x + width)};

            if (in_view) {
                for (seq::Beats beat {1}; beat < measure->time_signature.beats(); beat++) {
                    const float position_x_beat {
                        position_x +
                        float(beat) *
                        float(seq::steps(measure->time_signature.value())) * ui::rem(STEP_SIZE.x)
                    };

                    draw.list->AddLine(
                        draw.origin + ImVec2(position_x_beat, 0.0f) - ImVec2(m_composition_camera.x, 0.0f),
                        draw.origin + ImVec2(position_x_beat, draw.clamped_space.y) - ImVec2(m_composition_camera.x, 0.0f),
                        COLOR_FOREGROUND2
                    );
                }
            }

            position_x += width;

            if (in_view) {
                draw.list->AddLine(
                    draw.origin + ImVec2(position_x, 0.0f) - ImVec2(m_composition_camera.x, 0.0f),
                    draw.origin + ImVec2(position_x, draw.clamped_space.y) - ImVec2(m_composition_camera.x, 0.0f),
                    COLOR_FOREGROUND
                );
            }
        }
    }

    void Application::composition_measures_labels(const Draw& draw) const {
        static constexpr ImVec2 TEXT_OFFSET {5.0f / ui::FONT_SIZE, 5.0f / ui::FONT_SIZE};

        const ImColor& COLOR {color(ImGuiCol_Text)};

        float position_x {ui::rem(COMPOSITION_LEFT)};

        for (const auto& [i, measure] : m_composition.measures | std::views::enumerate) {
            const float width {float(measure.time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (point_x_in_camera_view(position_x + width, draw.space.x + width)) {
                char buffer[32] {};

                draw.list->AddText(
                    draw.origin + ImVec2(position_x, 0.0f) + ui::rem(TEXT_OFFSET) - ImVec2(m_composition_camera.x, 0.0f),
                    COLOR,
                    measure_label(buffer, i + 1)
                );
            }

            position_x += width;
        }
    }

    void Application::composition_notes(const Draw& draw) const {
        static constexpr float ROUNDING {6.0f};

        const ImColor& COLOR {color(ImGuiCol_Text)};

        float global_position_x {ui::rem(COMPOSITION_LEFT)};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float width {float(measure->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (point_x_in_camera_view(global_position_x + width, draw.space.x + width)) {
                for (const auto& [instrument, notes] : measure->instruments) {
                    if (instrument == m_instrument) {
                        continue;
                    }

                    composition_notes(draw, instrument, notes, global_position_x, ROUNDING);
                }

                // Always draw the selected instrument last
                if (auto instrument {measure->instruments.find(m_instrument)}; instrument != measure->instruments.end()) {
                    composition_notes(draw, instrument->first, instrument->second, global_position_x, ROUNDING);
                }

                for (const ProvenanceNote& provenance_note : m_composition_selected_notes) {
                    if (provenance_note.measure() == measure) {
                        const ImVec4 rect {note_rectangle(*provenance_note.note())};

                        draw.list->AddRect(
                            draw.origin + ImVec2(global_position_x + rect.x, rect.y) - m_composition_camera,
                            draw.origin + ImVec2(global_position_x + rect.x + rect.z, rect.y + rect.w) - m_composition_camera,
                            COLOR,
                            ROUNDING
                        );
                    }
                }
            }

            global_position_x += width;
        }
    }

    void Application::composition_notes(const Draw& draw, syn::InstrumentId instrument, const std::set<seq::Note>& notes, float global_position_x, float rounding) const {
        for (auto note {notes.begin()}; note != notes.end(); note++) {
            const ImVec4 rect {note_rectangle(*note)};

            draw.list->AddRectFilled(
                draw.origin + ImVec2(global_position_x + rect.x, rect.y) - m_composition_camera,
                draw.origin + ImVec2(global_position_x + rect.x + rect.z, rect.y + rect.w) - m_composition_camera,
                ui::COLORS[m_ui.colors.at(instrument)].second,
                rounding
            );

            if (note->legato) {
                const float x {global_position_x + float(note->position + seq::steps(note->value)) * ui::rem(STEP_SIZE.x)};

                draw.list->AddBezierCubic(
                    draw.origin + ImVec2(x - ui::rem(STEP_SIZE.x) * 6.0f, rect.y + ui::rem(STEP_SIZE.y)) - m_composition_camera,
                    draw.origin + ImVec2(x - ui::rem(STEP_SIZE.x) * 3.0f, rect.y + ui::rem(STEP_SIZE.y) * 1.5f) - m_composition_camera,
                    draw.origin + ImVec2(x + ui::rem(STEP_SIZE.x) * 3.0f, rect.y + ui::rem(STEP_SIZE.y) * 1.5f) - m_composition_camera,
                    draw.origin + ImVec2(x + ui::rem(STEP_SIZE.x) * 6.0f, rect.y + ui::rem(STEP_SIZE.y)) - m_composition_camera,
                    ui::COLORS[m_ui.colors.at(instrument)].second,
                    1.0f
                );
            }
        }
    }

    void Application::composition_cursor(const Draw& draw) const {
        const ImColor& COLOR {color(ImGuiCol_PlotHistogramHovered)};

        const float position_x {ui::rem(COMPOSITION_LEFT) + float(m_player.get_position()) * ui::rem(STEP_SIZE.x)};

        draw.list->AddLine(
            draw.origin + ImVec2(position_x, 0.0f) - ImVec2(m_composition_camera.x, 0.0f),
            draw.origin + ImVec2(position_x, draw.clamped_space.y) - ImVec2(m_composition_camera.x, 0.0f),
            COLOR
        );
    }

    void Application::composition_hover(const Draw& draw, const HoveredNote& hovered_note) const {
        const ImColor COLOR {color_opacity(ImGuiCol_PopupBg, 0.4f)};
        const ImColor COLOR2 {color_opacity(ImGuiCol_PopupBg, 0.6f)};

        switch (m_ui.tool) {
            case ui::ToolMeasure: {
                const float position_x {ui::rem(COMPOSITION_LEFT) + float(hovered_note.measure_position()) * ui::rem(STEP_SIZE.x)};
                const float width {float(hovered_note.measure()->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

                draw.list->AddRectFilled(
                    draw.origin + ImVec2(position_x + 1.0f, 0.0f) - ImVec2(m_composition_camera.x, 0.0f),
                    draw.origin + ImVec2(position_x + width, draw.clamped_space.y) - ImVec2(m_composition_camera.x, 0.0f),
                    COLOR
                );

                break;
            }
            case ui::ToolNote: {
                const float position_y {float(syn::keyboard::NOTES - 1 - hovered_note.id()) * ui::rem(STEP_SIZE.y)};

                draw.list->AddRectFilled(
                    draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT), position_y) - ImVec2(0.0f, m_composition_camera.y),
                    draw.origin + ImVec2(ui::rem(COMPOSITION_LEFT) + draw.clamped_space.x, position_y + ui::rem(STEP_SIZE.y)) - ImVec2(0.0f, m_composition_camera.y),
                    COLOR
                );

                const float position_x {ui::rem(COMPOSITION_LEFT) + float(hovered_note.global_position() / seq::DIVISION * seq::DIVISION) * ui::rem(STEP_SIZE.x)};

                draw.list->AddRectFilled(
                    draw.origin + ImVec2(position_x, position_y) - m_composition_camera,
                    draw.origin + ImVec2(position_x + ui::rem(STEP_SIZE.x) * float(seq::DIVISION), position_y + ui::rem(STEP_SIZE.y)) - m_composition_camera,
                    COLOR2
                );

                break;
            }
        }
    }

    void Application::shortcuts() {
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, ImGuiInputFlags_RouteAlways)) {
            file_new();
        }

        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_RouteAlways)) {
            file_open();
        }

        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteAlways)) {
            file_save();
        }

        if (ImGui::Shortcut(ImGuiKey_Space, ImGuiInputFlags_RouteAlways)) {
            if (m_player.is_playing()) {
                stop_player();
            } else {
                start_player();
            }
        }

        if (ImGui::Shortcut(ImGuiKey_R, ImGuiInputFlags_RouteAlways)) {
            m_player.seek(0);
        }

        if (ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_RouteAlways)) {
            switch (m_ui.tool) {
                case ui::ToolMeasure:
                    m_ui.tool = ui::ToolNote;
                    m_composition_selected_measure = m_composition.measures.end();
                    break;
                case ui::ToolNote:
                    m_ui.tool = ui::ToolMeasure;
                    m_composition_selected_notes.clear();
                    break;
            }
        }

        switch (m_ui.tool) {
            case ui::ToolMeasure:
                if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_A, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
                    append_measures();
                }

                if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_I, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
                    insert_measure();
                }

                if (ImGui::Shortcut(ImGuiKey_Backspace, ImGuiInputFlags_RouteAlways)) {
                    clear_measure();
                }

                if (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_RouteAlways)) {
                    delete_measure();
                }

                if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D, ImGuiInputFlags_RouteAlways)) {
                    delete_measure();
                }

                break;
            case ui::ToolNote:
                if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_W, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
                    shift_notes_up();
                }

                if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_S, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
                    shift_notes_down();
                }

                if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_A, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
                    shift_notes_left();
                }

                if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_D, ImGuiInputFlags_RouteAlways | ImGuiInputFlags_Repeat)) {
                    shift_notes_right();
                }

                if (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_RouteAlways)) {
                    delete_notes();
                }

                if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_D, ImGuiInputFlags_RouteAlways)) {
                    delete_notes();
                }

                if (ImGui::Shortcut(ImGuiKey_1, ImGuiInputFlags_RouteAlways)) {
                    m_ui.value = ui::ValueWhole;
                }

                if (ImGui::Shortcut(ImGuiKey_2, ImGuiInputFlags_RouteAlways)) {
                    m_ui.value = ui::ValueHalf;
                }

                if (ImGui::Shortcut(ImGuiKey_3, ImGuiInputFlags_RouteAlways)) {
                    m_ui.value = ui::ValueQuarter;
                }

                if (ImGui::Shortcut(ImGuiKey_4, ImGuiInputFlags_RouteAlways)) {
                    m_ui.value = ui::ValueEighth;
                }

                if (ImGui::Shortcut(ImGuiKey_5, ImGuiInputFlags_RouteAlways)) {
                    m_ui.value = ui::ValueSixteenth;
                }

                break;
        }
    }

    bool Application::tempo() {
        constexpr unsigned int one {1};

        ImGui::SetNextItemWidth(ui::rem(6.0f));

        bool result {};

        if (ImGui::InputScalar("Tempo", ImGuiDataType_S32, &m_ui.tempo, &one)) {
            m_ui.tempo = std::min(std::max(m_ui.tempo, seq::Tempo::MIN), seq::Tempo::MAX);
            result = true;
        }

        ImGui::SetItemTooltip("Change the tempo of the composition");

        return result;
    }

    bool Application::time_signature() {
        constexpr const char* BEATS[] { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" };
        constexpr const char* VALUE[] { "1", "2", "4", "8", "16" };

        static constexpr auto flags {ImGuiComboFlags_HeightSmall | ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_NoArrowButton};

        bool result {};

        ImGui::BeginGroup();

        if (ImGui::BeginCombo("Beats", BEATS[m_ui.time_signature.beats], flags)) {
            for (std::size_t i {}; i < std::size(BEATS); i++) {
                if (ImGui::Selectable(BEATS[i], m_ui.time_signature.beats == i)) {
                    m_ui.time_signature.beats = ui::TimeSignature::Beats(i);
                    result = true;
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Value", VALUE[m_ui.time_signature.value], flags)) {
            for (std::size_t i {}; i < std::size(VALUE); i++) {
                if (ImGui::Selectable(VALUE[i], m_ui.time_signature.value == i)) {
                    m_ui.time_signature.value = ui::TimeSignature::Value(i);
                    result = true;
                }
            }

            ImGui::EndCombo();
        }

        ImGui::EndGroup();

        ImGui::SetItemTooltip("Change the time signature of the selected measure");

        return result;
    }

    void Application::debug() const {
#ifndef ALFRED_DISTRIBUTION
        if (ImGui::Begin("Debug")) {
            ImGui::Text("Frame time: %f", get_frame_time());

            if (ImGui::SmallButton("Write Settings")) {
                ImGui::SaveIniSettingsToDisk("imguid.ini");
            }

            ImGui::Text("m_player.get_position() %u", m_player.get_position());
            ImGui::Text("m_composition_not_compiled %d", m_composition_not_compiled);
            ImGui::Text("m_composition_not_saved %d", m_composition_not_saved);
        }

        ImGui::End();
#endif
    }

    void Application::keyboard_input(unsigned int key, bool down) {
        const auto update {[this, down](syn::NoteId id) {
            if (down) {
                m_synthesizer.note_on(id + m_octave * 12, m_instrument);
            } else {
                m_synthesizer.note_off(id + m_octave * 12, m_instrument);
            }
        }};

        switch (key) {
            case SDLK_Z: update(0); break;
            case SDLK_S: update(1); break;
            case SDLK_X: update(2); break;
            case SDLK_C: update(3); break;
            case SDLK_F: update(4); break;
            case SDLK_V: update(5); break;
            case SDLK_G: update(6); break;
            case SDLK_B: update(7); break;
            case SDLK_N: update(8); break;
            case SDLK_J: update(9); break;
            case SDLK_M: update(10); break;
            case SDLK_K: update(11); break;
            case SDLK_COMMA: update(12); break;
            case SDLK_L: update(13); break;
            case SDLK_PERIOD: update(14); break;
            case SDLK_SLASH: update(15); break;
        }
    }

    void Application::composition_mouse_pressed(ImVec2 origin) {
        switch (m_ui.tool) {
            case ui::ToolMeasure: {
                if (const auto hovered_measure {hover_measure(composition_mouse_position(origin))}; hovered_measure) {
                    select_measure(*hovered_measure);
                }
                break;
            }
            case ui::ToolNote: {
                if (const auto hovered_note {hover_note(composition_mouse_position(origin))}; hovered_note) {
                    do_with_note(*hovered_note);
                }
                break;
            }
        }
    }

    void Application::composition_camera(bool item_active, bool item_hovered, ImVec2 space) {
        const auto& io {ImGui::GetIO()};

        if (item_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            m_composition_camera -= io.MouseDelta;
        }

        if (item_hovered) {
            if (ImGui::IsKeyDown(ImGuiMod_Shift)) {
                m_composition_camera -= ImVec2(
                    ui::rem(COMPOSITION_SCROLL_SPEED * 2.0f) * io.MouseWheel,
                    -ui::rem(COMPOSITION_SCROLL_SPEED) * io.MouseWheelH
                );
            } else {
                m_composition_camera -= ImVec2(
                    -ui::rem(COMPOSITION_SCROLL_SPEED * 2.0f) * io.MouseWheelH,
                    ui::rem(COMPOSITION_SCROLL_SPEED) * io.MouseWheel
                );
            }
        }

        const float cursor_position {float(m_player.get_position()) * ui::rem(STEP_SIZE.x) - m_composition_camera.x};

        if (m_player.is_playing() && cursor_position > space.x / 2.0f) {
            m_composition_camera.x += std::floor(cursor_position - space.x / 2.0f);
        }

        m_composition_camera.x = std::max(m_composition_camera.x, 0.0f);
        m_composition_camera.x = std::min(m_composition_camera.x, composition_width());
        m_composition_camera.y = std::max(m_composition_camera.y, 0.0f);
        m_composition_camera.y = std::min(m_composition_camera.y, ui::rem(COMPOSITION_HEIGHT + STEP_SIZE.y) - space.y);
    }

    void Application::add_metronome() {
        add_metronome(m_composition.measures.begin(), m_composition.measures.end());
    }

    void Application::add_metronome(MeasureIter begin, MeasureIter end) {
        for (auto measure {begin}; measure != end; measure++) {
            for (unsigned int i {}; i < measure->time_signature.measure_steps(); i += seq::steps(measure->time_signature.value())) {
                measure->instruments[instrument::Metronome::static_id()].emplace(i == 0 ? 50 : 48, seq::Sixteenth, i);
            }
        }

        modify_composition();
    }

    void Application::remove_metronome() {
        remove_metronome(m_composition.measures.begin(), m_composition.measures.end());
    }

    void Application::remove_metronome(MeasureIter begin, MeasureIter end) {
        for (auto measure {begin}; measure != end; measure++) {
            measure->instruments.erase(instrument::Metronome::static_id());
        }

        modify_composition();
    }

    void Application::select_measure(MeasureIter hovered_measure) {
        if (m_composition_selected_measure == hovered_measure) {
            m_composition_selected_measure = m_composition.measures.end();
        } else {
            m_composition_selected_measure = hovered_measure;

            set_tempo(m_ui.tempo, *hovered_measure);
            set_time_signature(m_ui.time_signature, *hovered_measure);
        }
    }

    void Application::append_measures() {
        assert(!m_player.is_playing());

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

        modify_composition();
    }

    void Application::insert_measure() {
        assert(!m_player.is_playing());
        assert(m_composition_selected_measure != m_composition.measures.end());

        const auto [tempo, time_signature] {measure_type(
            m_composition_selected_measure,
            m_composition.measures
        )};

        reset_note_legato_previous_measure(m_composition_selected_measure);

        m_composition_selected_measure = m_composition.measures.emplace(m_composition_selected_measure, tempo, time_signature);

        if (m_metronome) {
            add_metronome(m_composition_selected_measure, std::next(m_composition_selected_measure));
        }

        modify_composition();
    }

    void Application::clear_measure() {
        assert(!m_player.is_playing());
        assert(m_composition_selected_measure != m_composition.measures.end());

        reset_note_legato_previous_measure(m_composition_selected_measure);

        std::erase_if(m_composition_selected_measure->instruments, [](const auto& instrument) {
            return instrument.first != instrument::Metronome::static_id();
        });

        modify_composition();
    }

    void Application::delete_measure() {
        assert(!m_player.is_playing());
        assert(m_composition_selected_measure != m_composition.measures.end());

        reset_note_legato_previous_measure(m_composition_selected_measure);

        m_composition_selected_measure = m_composition.measures.erase(m_composition_selected_measure);

        modify_composition();
    }

    void Application::set_measure_tempo(MeasureIter measure) {
        set_tempo(*measure, m_ui.tempo);

        modify_composition();
    }

    void Application::set_measure_tempo() {
        assert(!m_player.is_playing());
        assert(m_composition_selected_measure != m_composition.measures.end());

        set_measure_tempo(m_composition_selected_measure);
    }

    void Application::set_measures_tempo() {
        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            set_measure_tempo(measure);
        }
    }

    void Application::set_measure_time_signature() {
        assert(!m_player.is_playing());
        assert(m_composition_selected_measure != m_composition.measures.end());

        if (empty_except_metronome(*m_composition_selected_measure)) {
            set_time_signature(*m_composition_selected_measure, m_ui.time_signature);

            if (m_metronome) {
                remove_metronome(m_composition_selected_measure, std::next(m_composition_selected_measure));
                add_metronome(m_composition_selected_measure, std::next(m_composition_selected_measure));
            }

            modify_composition();
        } else {
            // Reset back
            set_time_signature(m_ui.time_signature, *m_composition_selected_measure);
            LOG_DEBUG("Cannot change time signature in this state");
        }
    }

    std::optional<MeasureIter> Application::hover_measure(ImVec2 position) {
        float position_x {};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float right {float(measure->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (position.x >= position_x && position.x < position_x + right) {
                return std::make_optional(measure);
            }

            position_x += right;
        }

        return std::nullopt;
    }

    std::optional<HoveredNote> Application::hover_note(ImVec2 position) {
        syn::NoteId result_id {};

        {
            const int index {int(position.y / ui::rem(STEP_SIZE.y))};
            const int id {syn::keyboard::NOTES - 1 - index};

            if (id < 0) {
                return std::nullopt;  // This can happen now when hovering the mouse below the lowest note
            }

            result_id = syn::NoteId(id);
        }

        float position_x {};
        unsigned int global_position {};

        for (auto measure {m_composition.measures.begin()}; measure != m_composition.measures.end(); measure++) {
            const float right {float(measure->time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (position.x >= position_x && position.x < position_x + right) {
                const float offset {position.x - position_x};
                const unsigned int result_position {static_cast<unsigned int>(offset / ui::rem(STEP_SIZE.x))};

                return std::make_optional<HoveredNote>(
                    result_id,
                    measure,
                    result_position,
                    global_position + result_position
                );
            }

            position_x += right;
            global_position += measure->time_signature.measure_steps();
        }

        return std::nullopt;
    }

    std::optional<NoteIter> Application::select_note(const HoveredNote& hovered_note) const {
        const auto instrument {hovered_note.measure()->instruments.find(m_instrument)};

        if (instrument == hovered_note.measure()->instruments.end()) {
            return std::nullopt;
        }

        for (auto n {instrument->second.begin()}; n != instrument->second.end(); n++) {
            if (hovered_note.id() == n->id) {
                if (hovered_note.position() >= n->position && hovered_note.position() < n->position + seq::steps(n->value)) {
                    return std::make_optional(n);
                }
            }
        }

        return std::nullopt;
    }

    std::optional<unsigned int> Application::hover_position(ImVec2 position) const {
        unsigned int result {};

        for (float position_x {}; const seq::Measure& measure : m_composition.measures) {
            const float right {float(measure.time_signature.measure_steps()) * ui::rem(STEP_SIZE.x)};

            if (position.x >= position_x && position.x < position_x + right) {
                const float offset {position.x - position_x};

                result += static_cast<unsigned int>(offset / ui::rem(STEP_SIZE.x));

                return std::make_optional(result);
            }

            position_x += right;
            result += measure.time_signature.measure_steps();
        }

        return std::nullopt;
    }


    void Application::do_with_note(const HoveredNote& hovered_note) {
        if (auto note {select_note(hovered_note)}; note) {
            const auto selected_note {
                std::ranges::find_if(m_composition_selected_notes, [&hovered_note, note](const auto& n) {
                    return n.measure() == hovered_note.measure() && n.note() == note;
                })
            };

            if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
                if (selected_note != m_composition_selected_notes.end()) {
                    m_composition_selected_notes.erase(selected_note);
                } else {
                    m_composition_selected_notes.emplace_back(hovered_note.measure(), *note);
                }
            } else {
                const bool exists {selected_note != m_composition_selected_notes.end()};

                m_composition_selected_notes.clear();

                if (!exists) {
                    m_composition_selected_notes.emplace_back(hovered_note.measure(), *note);
                }
            }

            return;
        }

        if (!m_composition_selected_notes.empty()) {
            m_composition_selected_notes.clear();
            return;
        }

        const seq::Note new_note {
            hovered_note.id(),
            get_value(ui::Value(m_ui.value)),
            hovered_note.position() / seq::DIVISION * seq::DIVISION,  // Always place on groups of steps
            false
        };

        if (
            ![this, &hovered_note, &new_note] {
                if (new_note.position + seq::steps(new_note.value) > hovered_note.measure()->time_signature.measure_steps()) {
                    return false;
                }

                for (const seq::Note& note : hovered_note.measure()->instruments[m_instrument]) {
                    if (notes_overlapping(note, new_note)) {
                        return false;
                    }
                }

                return true;
            }()
        ) {
            LOG_DEBUG("Cannot place note here");
            return;
        }

        hovered_note.measure()->instruments[m_instrument].insert(new_note);
        play_note(new_note);

        modify_composition();
    }

    void Application::delete_notes() {
        assert(!m_player.is_playing());
        assert(!m_composition_selected_notes.empty());

        for (const ProvenanceNote& selected_note : m_composition_selected_notes) {
            if (auto previous_note {check_note_has_previous(selected_note)}; previous_note) {
                reset_note_legato(*previous_note);
            }

            selected_note.measure()->instruments.at(m_instrument).erase(selected_note.note());
        }

        m_composition_selected_notes.clear();

        modify_composition();
    }

    void Application::legato_notes() {
        assert(!m_player.is_playing());
        assert(!m_composition_selected_notes.empty());

        for (ProvenanceNote& selected_note : m_composition_selected_notes) {
            seq::Note note {selected_note.copy()};
            note.legato = !note.legato;

            if (note.legato) {
                if (!check_note_has_next(selected_note)) {
                    note.legato = false;
                    LOG_DEBUG("Note cannot have legato");
                }
            }

            readd_note(selected_note, note);
        }

        modify_composition();
    }

    void Application::shift_notes_up() {  // FIXME don't reset legato if the tied notes are both shifted
        assert(!m_player.is_playing());
        assert(!m_composition_selected_notes.empty());

        if (
            ![this] {
                for (const ProvenanceNote& selected_note : m_composition_selected_notes) {
                    if (!check_note_up_limit(*selected_note.note())) {
                        return false;
                    }

                    const auto& notes {selected_note.measure()->instruments.at(m_instrument)};

                    for (auto note {notes.begin()}; note != notes.end(); note++) {
                        if (note_in_selection(note, selected_note.measure(), m_composition_selected_notes)) {
                            continue;
                        }

                        seq::Note shifted_note {selected_note.copy()};
                        shifted_note.id++;

                        if (notes_overlapping(*note, shifted_note)) {
                            return false;
                        }
                    }
                }

                return true;
            }()
        ) {
            LOG_DEBUG("Cannot shift notes here");
            return;
        }

        for (ProvenanceNote& selected_note : m_composition_selected_notes) {
            seq::Note note {selected_note.copy()};
            note.id++;
            note.legato = false;

            if (const auto previous_note {check_note_has_previous(selected_note)}; previous_note) {
                reset_note_legato(*previous_note);
            }

            readd_note(selected_note, note);
            play_note(note);
        }

        modify_composition();
    }

    void Application::shift_notes_down() {  // FIXME don't reset legato if the tied notes are both shifted
        assert(!m_player.is_playing());
        assert(!m_composition_selected_notes.empty());

        if (
            ![this] {
                for (const ProvenanceNote& selected_note : m_composition_selected_notes) {
                    if (!check_note_down_limit(*selected_note.note())) {
                        return false;
                    }

                    const auto& notes {selected_note.measure()->instruments.at(m_instrument)};

                    for (auto note {notes.begin()}; note != notes.end(); note++) {
                        if (note_in_selection(note, selected_note.measure(), m_composition_selected_notes)) {
                            continue;
                        }

                        seq::Note shifted_note {selected_note.copy()};
                        shifted_note.id--;

                        if (notes_overlapping(*note, shifted_note)) {
                            return false;
                        }
                    }
                }

                return true;
            }()
        ) {
            LOG_DEBUG("Cannot shift notes here");
            return;
        }

        for (ProvenanceNote& selected_note : m_composition_selected_notes) {
            seq::Note note {selected_note.copy()};
            note.id--;
            note.legato = false;

            if (const auto previous_note {check_note_has_previous(selected_note)}; previous_note) {
                reset_note_legato(*previous_note);
            }

            readd_note(selected_note, note);
            play_note(note);
        }

        modify_composition();
    }

    void Application::shift_notes_left() {
        assert(!m_player.is_playing());
        assert(!m_composition_selected_notes.empty());

        if (
            ![this] {
                for (const ProvenanceNote& selected_note : m_composition_selected_notes) {
                    if (!check_note_left_limit(*selected_note.note())) {
                        return false;
                    }

                    const auto& notes {selected_note.measure()->instruments.at(m_instrument)};

                    for (auto note {notes.begin()}; note != notes.end(); note++) {
                        if (note_in_selection(note, selected_note.measure(), m_composition_selected_notes)) {
                            continue;
                        }

                        seq::Note shifted_note {selected_note.copy()};
                        shifted_note.position--;

                        if (notes_overlapping(*note, shifted_note)) {
                            return false;
                        }
                    }
                }

                return true;
            }()
        ) {
            LOG_DEBUG("Cannot shift notes here");
            return;
        }

        for (ProvenanceNote& selected_note : m_composition_selected_notes) {
            seq::Note note {selected_note.copy()};
            note.position -= seq::DIVISION;
            note.legato = false;  // No need to check previous

            readd_note(selected_note, note);
        }

        modify_composition();
    }

    void Application::shift_notes_right() {
        assert(!m_player.is_playing());
        assert(!m_composition_selected_notes.empty());

        if (
            ![this] {
                for (const ProvenanceNote& selected_note : m_composition_selected_notes) {
                    if (!check_note_right_limit(*selected_note.note(), *selected_note.measure())) {
                        return false;
                    }

                    const auto& notes {selected_note.measure()->instruments.at(m_instrument)};

                    for (auto note {notes.begin()}; note != notes.end(); note++) {
                        if (note_in_selection(note, selected_note.measure(), m_composition_selected_notes)) {
                            continue;
                        }

                        seq::Note shifted_note {selected_note.copy()};
                        shifted_note.position++;

                        if (notes_overlapping(*note, shifted_note)) {
                            return false;
                        }
                    }
                }

                return true;
            }()
        ) {
            LOG_DEBUG("Cannot shift notes here");
            return;
        }

        for (ProvenanceNote& selected_note : m_composition_selected_notes) {
            seq::Note note {selected_note.copy()};
            note.position += seq::DIVISION;
            note.legato = false;

            if (const auto previous_note {check_note_has_previous(selected_note)}; previous_note) {
                reset_note_legato(*previous_note);
            }

            readd_note(selected_note, note);
        }

        modify_composition();
    }

    void Application::start_player() {
        if (m_composition_not_compiled) {
            LOG_DEBUG("Compiling composition");
            m_player.prepare();
            m_composition_not_compiled = false;
        }

        m_composition_selected_measure = m_composition.measures.end();
        m_composition_selected_notes.clear();

        m_player.start();
        set_desired_frame_time(FRAME_TIME_PLAYBACK);
    }

    void Application::stop_player() {
        m_player.stop();
        set_desired_frame_time(FRAME_TIME_DEFAULT);
    }

    void Application::modify_composition() {
        if (!m_composition_not_saved) {
            set_title_composition_not_saved();
        }

        m_composition_not_compiled = true;
        m_composition_not_saved = true;
    }

    void Application::invalidate_composition() {
        m_composition_not_compiled = true;
    }

    void Application::reset_composition_flags() {
        m_composition_not_compiled = false;
        m_composition_not_saved = false;
    }

    void Application::set_title_composition_not_saved() const {
        if (m_composition_path.empty()) {
            set_title("Alfred - Unsaved Composition");
        } else {
            set_title(std::format("Alfred - {}*", m_composition_path.filename().c_str()));
        }

        LOG_DEBUG("Changed title");
    }

    void Application::set_title_composition_saved() const {
        if (m_composition_path.empty()) {
            set_title("Alfred");
        } else {
            set_title(std::format("Alfred - {}", m_composition_path.filename().c_str()));
        }

        LOG_DEBUG("Changed title");
    }

    void Application::set_color_scheme(ui::ColorScheme color_scheme) {
        switch (color_scheme) {
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

        LOG_DEBUG("Changed color scheme");
    }

    void Application::set_scale(ui::Scale scale) {
        switch (scale) {
            case ui::Scale1X:
                ui::set_scale(1);
                break;
            case ui::Scale2X:
                ui::set_scale(2);
                break;
        }

        LOG_DEBUG("Changed scale");
    }

    float Application::composition_width() const {
        return std::accumulate(m_composition.measures.begin(), m_composition.measures.end(), 0.0f, [](const float& total, const auto& measure) {
            return total + float(measure.time_signature.measure_steps()) * ui::rem(STEP_SIZE.x);
        });
    }

    ImVec2 Application::composition_space(ImVec2 space) const {
        const float max_width {composition_width() - m_composition_camera.x};
        const float max_height {ui::rem(COMPOSITION_HEIGHT) - m_composition_camera.y};

        return { std::min(space.x, max_width), std::min(space.y, max_height) };
    }

    ImVec2 Application::composition_mouse_position(ImVec2 origin) const {
        const ImVec2 mouse_position {ImGui::GetIO().MousePos};

        if (!ImGui::IsMousePosValid(&mouse_position)) {
            return ImVec2(0.0f, 0.0f);
        }

        return mouse_position - origin - ImVec2(ui::rem(COMPOSITION_LEFT), 0.0f) + m_composition_camera;
    }

    std::flat_set<syn::InstrumentId> Application::instruments_in_project() const {
        std::flat_set<syn::InstrumentId> instruments;

        for (const seq::Measure& measure : m_composition.measures) {
            for (const auto& instrument : measure.instruments) {
                if (!instrument.second.empty()) {
                    instruments.insert(instrument.first);
                }
            }
        }

        instruments.erase(instrument::Metronome::static_id());

        return instruments;
    }

    bool Application::point_x_in_camera_view(float point_x, float space_x) const {
        return point_x >= m_composition_camera.x && point_x < m_composition_camera.x + space_x;
    }

    bool Application::point_y_in_camera_view(float point_y, float space_y) const {
        return point_y >= m_composition_camera.y && point_y < m_composition_camera.y + space_y;
    }

    void Application::readd_note(ProvenanceNote& provenance_note, const seq::Note& note) const {
        provenance_note.measure()->instruments.at(m_instrument).erase(provenance_note.note());
        const auto [iter, inserted] {provenance_note.measure()->instruments.at(m_instrument).insert(note)};

        assert(inserted);

        provenance_note.note(iter);
    }

    void Application::readd_note(NoteIter note_iter, MeasureIter measure, const seq::Note& note) const {
        measure->instruments.at(m_instrument).erase(note_iter);
        measure->instruments.at(m_instrument).insert(note);
    }

    void Application::reset_note_legato(const ProvenanceNote& provenance_note) const {
        seq::Note note {provenance_note.copy()};
        note.legato = false;

        readd_note(provenance_note.note(), provenance_note.measure(), note);
    }

    void Application::reset_note_legato_previous_measure(MeasureIter measure) const {
        for (const auto& [instrument, notes] : measure->instruments) {
            for (auto note {notes.begin()}; note != notes.end(); note++) {
                if (seq::Composition::note_first_in_measure(*measure, *note)) {
                    if (auto previous_note {m_composition.check_note_has_previous(instrument, measure, note)}; previous_note) {
                        reset_note_legato(*previous_note);
                    }
                }
            }
        }
    }

    void Application::play_note(const seq::Note& note) {
        m_synthesizer.note_on(note.id, m_instrument);

        m_task_manager.add_delayed_task([this, id = note.id] {
            m_synthesizer.note_off(id, m_instrument);
        }, math::seconds_to_milliseconds(m_synthesizer.get_instrument(m_instrument).attack_time()) + 100);
    }

    bool Application::keyboard_active() {
        return !imgui::want_capture_keyboard() && !ImGui::IsKeyDown(ImGuiMod_Ctrl) && !ImGui::IsKeyDown(ImGuiMod_Alt) && !ImGui::IsKeyDown(ImGuiMod_Shift);
    }

    float Application::note_height(const seq::Note& note) {
        return ui::rem(COMPOSITION_HEIGHT) - ui::rem(STEP_SIZE.y) - float(note.id) * ui::rem(STEP_SIZE.y);
    }

    ImVec4 Application::note_rectangle(const seq::Note& note) {
        const float x {float(note.position) * ui::rem(STEP_SIZE.x)};
        const float y {note_height(note)};
        const float width {float(seq::steps(note.value)) * ui::rem(STEP_SIZE.x)};
        const float height {ui::rem(STEP_SIZE.y)};

        return ImVec4(x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f);
    }

    const char* Application::measure_label(char* buffer, long number) {
        const std::to_chars_result result {std::to_chars(buffer, buffer + sizeof(buffer), number)};

        if (result.ec != std::errc()) {
            std::strcpy(buffer, "?");
        } else {
            *result.ptr = '\0';
        }

        return buffer;
    }

    std::pair<seq::Tempo, seq::TimeSignature> Application::measure_type(MeasureIter measure, const std::vector<seq::Measure>& measures) {
        seq::Tempo tempo;
        seq::TimeSignature time_signature;

        if (measure != measures.end()) {
            tempo = measure->tempo;
            time_signature = measure->time_signature;
        }

        return { tempo, time_signature };
    }

    void Application::set_tempo(seq::Measure& measure, const ui::Tempo& tempo) {
        measure.tempo = seq::Tempo(tempo);
    }

    void Application::set_tempo(ui::Tempo& tempo, const seq::Measure& measure) {
        tempo = measure.tempo;
    }

    void Application::set_time_signature(seq::Measure& measure, const ui::TimeSignature& time_signature) {
        seq::Beats beats {};
        seq::Value value {};

        switch (time_signature.beats) {
            case ui::TimeSignature::Beats1: beats = 1; break;
            case ui::TimeSignature::Beats2: beats = 2; break;
            case ui::TimeSignature::Beats3: beats = 3; break;
            case ui::TimeSignature::Beats4: beats = 4; break;
            case ui::TimeSignature::Beats5: beats = 5; break;
            case ui::TimeSignature::Beats6: beats = 6; break;
            case ui::TimeSignature::Beats7: beats = 7; break;
            case ui::TimeSignature::Beats8: beats = 8; break;
            case ui::TimeSignature::Beats9: beats = 9; break;
            case ui::TimeSignature::Beats10: beats = 10; break;
            case ui::TimeSignature::Beats11: beats = 11; break;
            case ui::TimeSignature::Beats12: beats = 12; break;
            case ui::TimeSignature::Beats13: beats = 13; break;
            case ui::TimeSignature::Beats14: beats = 14; break;
            case ui::TimeSignature::Beats15: beats = 15; break;
            case ui::TimeSignature::Beats16: beats = 16; break;
        }

        switch (time_signature.value) {
            case ui::TimeSignature::Value1: value = seq::Whole; break;
            case ui::TimeSignature::Value2: value = seq::Half; break;
            case ui::TimeSignature::Value4: value = seq::Quarter; break;
            case ui::TimeSignature::Value8: value = seq::Eighth; break;
            case ui::TimeSignature::Value16: value = seq::Sixteenth; break;
        }

        measure.time_signature = seq::TimeSignature(beats, value);
    }

    void Application::set_time_signature(ui::TimeSignature& time_signature, const seq::Measure& measure) {
        switch (measure.time_signature.beats()) {
            case 1: time_signature.beats = ui::TimeSignature::Beats1; break;
            case 2: time_signature.beats = ui::TimeSignature::Beats2; break;
            case 3: time_signature.beats = ui::TimeSignature::Beats3; break;
            case 4: time_signature.beats = ui::TimeSignature::Beats4; break;
            case 5: time_signature.beats = ui::TimeSignature::Beats5; break;
            case 6: time_signature.beats = ui::TimeSignature::Beats6; break;
            case 7: time_signature.beats = ui::TimeSignature::Beats7; break;
            case 8: time_signature.beats = ui::TimeSignature::Beats8; break;
            case 9: time_signature.beats = ui::TimeSignature::Beats9; break;
            case 10: time_signature.beats = ui::TimeSignature::Beats10; break;
            case 11: time_signature.beats = ui::TimeSignature::Beats11; break;
            case 12: time_signature.beats = ui::TimeSignature::Beats12; break;
            case 13: time_signature.beats = ui::TimeSignature::Beats13; break;
            case 14: time_signature.beats = ui::TimeSignature::Beats14; break;
            case 15: time_signature.beats = ui::TimeSignature::Beats15; break;
            case 16: time_signature.beats = ui::TimeSignature::Beats16; break;
        }

        switch (measure.time_signature.value()) {
            case seq::Whole: time_signature.value = ui::TimeSignature::Value1; break;
            case seq::Half: time_signature.value = ui::TimeSignature::Value2; break;
            case seq::Quarter: time_signature.value = ui::TimeSignature::Value4; break;
            case seq::Eighth: time_signature.value = ui::TimeSignature::Value8; break;
            case seq::Sixteenth: time_signature.value = ui::TimeSignature::Value16; break;
        }
    }

    bool Application::empty_except_metronome(const seq::Measure& measure) {
        return std::ranges::all_of(measure.instruments, [](const auto& instrument) {
            return instrument.second.empty() || instrument.first == instrument::Metronome::static_id();
        });
    }

    bool Application::check_note_up_limit(const seq::Note& note) {
        return note.id < static_cast<unsigned int>(syn::keyboard::NOTES - 1);
    }

    bool Application::check_note_down_limit(const seq::Note& note) {
        return note.id > 0;
    }

    bool Application::check_note_left_limit(const seq::Note& note) {
        return note.position > 0;
    }

    bool Application::check_note_right_limit(const seq::Note& note, const seq::Measure& measure) {
        return note.position < measure.time_signature.measure_steps() - seq::steps(note.value);
    }

    bool Application::notes_overlapping(const seq::Note& note1, const seq::Note& note2) {
        if (note1.id != note2.id) {
            return false;
        }

        const auto note1_left {note1.position};
        const auto note1_right {note1.position + seq::steps(note1.value)};
        const auto note2_left {note2.position};
        const auto note2_right {note2.position + seq::steps(note2.value)};

        return note1_left > note2_left && note1_left < note2_right ||
            note1_right > note2_left && note1_right < note2_right ||
            note2_left > note1_left && note2_left < note1_right ||
            note2_right > note1_left && note2_right < note1_right;
    }

    bool Application::note_in_selection(NoteIter note, MeasureIter measure, const std::vector<ProvenanceNote>& selected_notes) {
        return std::ranges::find_if(selected_notes, [note, measure](const auto& n) {
            return measure == n.measure() && note == n.note();
        }) != selected_notes.end();
    }

    std::optional<ProvenanceNote> Application::check_note_has_next(const ProvenanceNote& provenance_note) const {
        return m_composition.check_note_has_next(m_instrument, provenance_note);
    }

    std::optional<ProvenanceNote> Application::check_note_has_previous(const ProvenanceNote& provenance_note) const {
        return m_composition.check_note_has_previous(m_instrument, provenance_note);
    }

    Time Application::elapsed_seconds_to_time(double elapsed_seconds) {
        Time time;

        double total_seconds {};
        const double fraction_seconds {std::modf(elapsed_seconds, &total_seconds)};

        const std::div_t division {std::div(int(total_seconds), 60)};

        time.minutes = division.quot;
        time.seconds = division.rem;
        time.deciseconds = int(fraction_seconds * 10.0);

        return time;
    }

    seq::Value Application::get_value(ui::Value value) {
        switch (value) {
            case ui::ValueWhole:
                return seq::Whole;
            case ui::ValueHalf:
                return seq::Half;
            case ui::ValueQuarter:
                return seq::Quarter;
            case ui::ValueEighth:
                return seq::Eighth;
            case ui::ValueSixteenth:
                return seq::Sixteenth;
        }

        std::unreachable();
    }

    const ImVec4& Application::color(ImGuiCol color) {
        return ImGui::GetStyle().Colors[color];
    }

    ImColor Application::color_opacity(ImGuiCol color, float opacity) {
        ImColor color_ {ImGui::GetStyle().Colors[color]};
        color_.Value.w = opacity;
        return color_;
    }

    void Application::composition_save_file_dialog(void* userdata, const char* const* filelist, int) {
        Application& self {*static_cast<Application*>(userdata)};

        if (!filelist) {
            self.m_task_manager.add_immediate_thread_safe_task([error = std::string(SDL_GetError())] {
                logging::error("An error occurred while handling the file dialog: {}", error);
            });

            return;
        }

        if (const char* file {filelist[0]}; file) {
            self.m_task_manager.add_immediate_thread_safe_task([&self, file = std::string(file)] mutable {
                self.composition_save(std::move(file));
            });
        }
    }

    void Application::composition_open_file_dialog(void* userdata, const char* const* filelist, int) {
        Application& self {*static_cast<Application*>(userdata)};

        if (!filelist) {
            self.m_task_manager.add_immediate_thread_safe_task([error = std::string(SDL_GetError())] {
                logging::error("An error occurred while handling the file dialog: {}", error);
            });

            return;
        }

        if (const char* file {filelist[0]}; file) {
            self.m_task_manager.add_immediate_thread_safe_task([&self, file = std::string(file)] mutable {
                (void) self.composition_open(std::move(file));
            });
        }
    }

    void Application::composition_save(const std::filesystem::path& path, const composition::Composition& composition) {
        utility::Buffer buffer;
        composition::export_composition(composition, buffer);
        utility::write_file(path, buffer);

        logging::information("Saved composition to `{}`", path.c_str());
    }

    void Application::composition_open(const std::filesystem::path& path, composition::Composition& composition) {
        utility::Buffer buffer;
        utility::read_file(path, buffer);
        composition::import_composition(composition, buffer);

        logging::information("Opened composition from `{}`", path.c_str());
    }

    bool Application::composition_save(std::filesystem::path path) {
        path.replace_extension("alfred");

        try {
            composition_save(path, m_composition);
        } catch (const composition::CompositionError& e) {
            logging::error("Could not save composition: {}", e.what());
            return false;
        } catch (const utility::FilerError& e) {
            logging::error("Could not save composition: {}", e.what());
            return false;
        }

        m_composition_path = std::move(path);
        m_composition_not_saved = false;
        m_data.recent_compositions.insert(m_composition_path);

        set_title_composition_saved();

        return true;
    }

    bool Application::composition_save() {
        assert(!m_composition_path.empty());

        if (m_composition_path.extension() != ".alfred") {
            LOG_WARNING("Composition file has the wrong extension");
        }

        try {
            composition_save(m_composition_path, m_composition);
        } catch (const composition::CompositionError& e) {
            logging::error("Could not save composition: {}", e.what());
            return false;
        } catch (const utility::FilerError& e) {
            logging::error("Could not save composition: {}", e.what());
            return false;
        }

        m_composition_not_saved = false;
        m_data.recent_compositions.insert(m_composition_path);

        set_title_composition_saved();

        return true;
    }

    bool Application::composition_open(std::filesystem::path path) {
        if (path.extension() != ".alfred") {
            LOG_WARNING("Composition file has the wrong extension");
        }

        try {
            composition_open(path, m_composition);
        } catch (const composition::CompositionError& e) {
            m_composition = {};
            logging::error("Could not open composition: {}", e.what());
            return false;
        } catch (const utility::FilerError& e) {
            m_composition = {};
            logging::error("Could not open composition: {}", e.what());
            return false;
        }

        m_composition_path = std::move(path);
        m_data.recent_compositions.insert(m_composition_path);

        std::strncpy(m_ui.composition.title, m_composition.title.c_str(), sizeof(m_ui.composition.title));
        std::strncpy(m_ui.composition.author, m_composition.author.c_str(), sizeof(m_ui.composition.author));
        m_ui.composition.year = short(int(m_composition.year));

        m_player.seek(0);

        reset_composition_flags();
        invalidate_composition();
        set_title_composition_saved();

        return true;
    }

    void Application::composition_new() {
        m_composition = {};
        m_composition_path.clear();
        m_ui.composition = {};
        m_player.seek(0);

        reset_composition_flags();
        set_title_composition_saved();
    }

    void Application::file_new() {
        composition_new();
    }

    void Application::file_open() {
        constexpr SDL_DialogFileFilter filters[] {
            { "Alfred files", "alfred" },
            { "All files", "*" }
        };

        SDL_ShowOpenFileDialog(
            &Application::composition_open_file_dialog,
            this,
            m_window,
            filters,
            std::size(filters),
            nullptr,
            false
        );
    }

    void Application::file_save() {
        constexpr SDL_DialogFileFilter filters[] {
            { "Alfred files", "alfred" },
            { "All files", "*" }
        };

        if (m_composition_path.empty()) {
            SDL_ShowSaveFileDialog(
                &Application::composition_save_file_dialog,
                this,
                m_window,
                filters,
                std::size(filters),
                nullptr
            );
        } else {
            composition_save();
        }
    }
}
