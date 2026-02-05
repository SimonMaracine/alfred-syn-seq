#pragma once

#include <utility>
#include <vector>
#include <unordered_map>
#include <optional>
#include <filesystem>

#include <imgui.h>
#include <alfred/synthesizer.hpp>

#include "video.hpp"
#include "sequencer.hpp"
#include "composition.hpp"
#include "ui.hpp"
#include "data.hpp"
#include "task.hpp"
#include "image.hpp"

namespace application {
    using MeasureIter = std::vector<seq::Measure>::iterator;
    using NoteIter = std::multiset<seq::Note>::iterator;

    class SelectedNote {
    public:
        SelectedNote() = default;
        SelectedNote(MeasureIter measure, NoteIter note)
            : m_measure(measure), m_note(note) {}

        MeasureIter measure() const { return m_measure; }
        NoteIter note() const { return m_note; }
        void note(NoteIter note) { m_note = note; }
    private:
        MeasureIter m_measure;
        NoteIter m_note;
    };

    class HoveredNote {
    public:
        HoveredNote() = default;
        HoveredNote(syn::Id id, MeasureIter measure, unsigned int position, unsigned int global_position)
            : m_id(id), m_measure(measure), m_position(position), m_global_position(global_position) {}

        syn::Id id() const { return m_id; }
        MeasureIter measure() const { return m_measure; }
        unsigned int position() const { return m_position; }
        unsigned int global_position() const { return m_global_position; }

        unsigned int measure_position() const {
            return m_global_position - m_position;
        }

        bool operator==(const HoveredNote& other) const {
            return m_id == other.m_id && m_global_position == other.m_global_position;
        }
    private:
        syn::Id m_id {};
        MeasureIter m_measure;
        unsigned int m_position {};
        unsigned int m_global_position {};
    };

    struct Time {
        int minutes {};
        int seconds {};
        int deciseconds {};
    };

    class Application : public video::Video {
    public:
        void on_start() override;
        void on_stop() override;
        void on_update() override;
        void on_imgui() override;
        void on_late_update() override;
        void on_event(const SDL_Event& event) override;
    private:
        void main_menu_bar();
        void main_menu_bar_file();
        void main_menu_bar_edit();
        void main_menu_bar_options();
        void main_menu_bar_composition();
        void main_menu_bar_help();
        void keyboard() const;
        void keyboard_key(ImDrawList* list, ImVec2 origin, char key, float x, float y, int scancode) const;
        void instruments();
        void output();
        void playback();
        void tools();
        void tools_measure();
        void tools_note();
        void composition();
        void composition_left(ImDrawList* list, ImVec2 origin) const;
        void composition_octaves(ImDrawList* list, ImVec2 origin, ImVec2 space) const;
        void composition_measures(ImDrawList* list, ImVec2 origin, ImVec2 space) const;
        void composition_measures_labels(ImDrawList* list, ImVec2 origin) const;
        void composition_notes(ImDrawList* list, ImVec2 origin) const;
        void composition_cursor(ImDrawList* list, ImVec2 origin) const;
        void composition_hover(ImDrawList* list, ImVec2 origin, ImVec2 space, const HoveredNote& hovered_note) const;
        void shortcuts();
        bool tempo();
        bool time_signature();
        void debug() const;

        void keyboard_input(unsigned int key, bool down);
        void composition_mouse_pressed(ImVec2 origin);
        void composition_mouse_released(ImVec2 origin);
        void composition_camera(bool item_active, bool item_hovered, ImVec2 space);
        void add_metronome();
        void add_metronome(MeasureIter begin, MeasureIter end);
        void remove_metronome();
        void remove_metronome(MeasureIter begin, MeasureIter end);
        bool hover_measure(ImVec2 position, MeasureIter& hovered_measure);
        void select_measure(MeasureIter hovered_measure);
        void append_measures();
        void insert_measure();
        void clear_measure();
        void delete_measure();
        void set_measure_tempo();
        void set_measure_time_signature();
        bool hover_note(ImVec2 position, HoveredNote& hovered_note);
        bool select_note(const HoveredNote& hovered_note, NoteIter& note) const;
        void do_with_note(const HoveredNote& hovered_note);
        void delete_notes();
        void shift_notes_up();
        void shift_notes_down();
        void shift_notes_left();
        void shift_notes_right();
        bool hover_position(ImVec2 position, unsigned int& position_) const;

        void start_player();
        void stop_player();
        void modify_composition();
        void invalidate_composition();
        void reset_composition_flags();
        void set_title_composition_not_saved() const;
        void set_title_composition_saved() const;
        static void set_color_scheme(ui::ColorScheme color_scheme);
        static void set_scale(ui::Scale scale);
        ImVec2 composition_mouse_position(ImVec2 origin) const;
        std::flat_set<syn::Voice> instruments_in_project() const;

        static bool keyboard_active();
        static float note_height(const seq::Note& note);
        static ImVec4 note_rectangle(const seq::Note& note);
        static const char* measure_label(char* buffer, long number);
        static std::pair<seq::Tempo, seq::TimeSignature> measure_type(MeasureIter iter, const std::vector<seq::Measure>& measures);
        static void set_tempo(seq::Measure& measure, const ui::Tempo& tempo);
        static void set_tempo(ui::Tempo& tempo, const seq::Measure& measure);
        static void set_time_signature(seq::Measure& measure, const ui::TimeSignature& time_signature);
        static void set_time_signature(ui::TimeSignature& time_signature, const seq::Measure& measure);
        static bool empty_except_metronome(const seq::Measure& measure);
        static bool check_note_up_limit(const seq::Note& note);
        static bool check_note_down_limit(const seq::Note& note);
        static bool check_note_left_limit(const seq::Note& note);
        static bool check_note_right_limit(const seq::Note& note, const seq::Measure& measure);
        static bool notes_overlapping(const seq::Note& note1, const seq::Note& note2);
        static bool note_in_selection(NoteIter note, MeasureIter measure, const std::vector<SelectedNote>& selected_notes);
        static Time elapsed_seconds_to_time(double elapsed_seconds);
        static seq::Value get_value(ui::Value value);
        static ImColor set_opacity(ImColor color, float opacity);
        static void composition_save_file_dialog(void* userdata, const char* const* filelist, int filter);
        static void composition_open_file_dialog(void* userdata, const char* const* filelist, int filter);
        static void composition_save(const std::filesystem::path& path, const composition::Composition& composition);
        static void composition_open(const std::filesystem::path& path, composition::Composition& composition);
        bool composition_save(std::filesystem::path path);
        bool composition_save();
        bool composition_open(std::filesystem::path path);
        void composition_new();
        void file_new();
        void file_open();
        void file_save();

        data::Data m_data;
        task::TaskManager m_task_manager;

        syn::Voice m_voice {syn::VoiceBell};
        syn::keyboard::Octave m_octave {syn::keyboard::Octave3};

        ImVec2 m_composition_camera;
        MeasureIter m_composition_selected_measure;
        std::vector<SelectedNote> m_composition_selected_notes;
        std::filesystem::path m_composition_path;
        composition::Composition m_composition;

        synthesizer::Synthesizer m_synthesizer;
        seq::Player m_player;

        struct {
            int tool {ui::ToolMeasure};
            int value {ui::ValueQuarter};
            ui::Tempo tempo {seq::Tempo()};
            ui::TimeSignature time_signature;
            ui::Composition composition;
            int octave {ui::Octave3};
            double volume {};
            const char* device {};
            image::Texture texture_play;
            image::Texture texture_pause;
            image::Texture texture_rewind;
            std::unordered_map<syn::Voice, ui::ColorIndex> colors;
            std::optional<MeasureIter> hovered_measure;
            std::optional<HoveredNote> hovered_note;
            bool hovered_composition {};
        } m_ui;

        bool m_metronome {};
        bool m_composition_not_compiled {};
        bool m_composition_not_saved {};
    };
}
