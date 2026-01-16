#pragma once

#include <utility>
#include <vector>

#include <imgui.h>
#include <alfred/synthesizer.hpp>

#include "video.hpp"
#include "sequencer.hpp"
#include "ui.hpp"
#include "task.hpp"

namespace application {
    class Application : public video::Video {  // FIXME pure virtual method call sometimes
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
        void main_menu_bar_sequencer();
        void main_menu_bar_options();
        void main_menu_bar_help();
        void keyboard();
        void keyboard_key(ImDrawList* list, ImVec2 origin, char key, float x, float y, int scancode);
        void instruments();
        void output();
        void playback();
        void tools();
        void tools_measure();
        void tools_note();
        void composition();
        void composition_left(ImDrawList* list, ImVec2 origin) const;
        void composition_measures(ImDrawList* list, ImVec2 origin) const;
        void composition_measures_labels(ImDrawList* list, ImVec2 origin) const;
        void composition_notes(ImDrawList* list, ImVec2 origin) const;
        void composition_cursor(ImDrawList* list, ImVec2 origin) const;
        bool tempo();
        bool time_signature();
        void debug();

        using MeasureIter = std::vector<seq::Measure>::iterator;
        using NoteIter = std::flat_multiset<seq::Note>::iterator;

        struct SelectedNote {
            MeasureIter measure;
            NoteIter note;
        };

        struct CompositionNote {
            syn::Name name {};
            syn::Octave octave {};
            MeasureIter measure;
            unsigned int position {};
        };

        void update_keyboard_input(unsigned int key, bool down);
        void add_metronome();
        void add_metronome(MeasureIter begin, MeasureIter end);
        void remove_metronome();
        void remove_metronome(MeasureIter begin, MeasureIter end);
        void select_measure(ImVec2 position);
        void append_measures();
        void insert_measure();
        void clear_measure();
        void delete_measure();
        void set_measure_tempo();
        void set_measure_time_signature();
        bool click_note(ImVec2 position, CompositionNote& composition_note);
        bool select_note(const CompositionNote& composition_note, NoteIter& note);
        void do_with_clicked_note(const CompositionNote& composition_note);
        void delete_notes();
        void shift_notes_left(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end, unsigned int steps);
        void shift_notes_right(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end, unsigned int steps);

        ImVec2 composition_mouse_position(ImVec2 origin) const;

        static float note_height(const seq::Note& note);
        static ImVec4 note_rectangle(const seq::Note& note);
        static const char* measure_label(char* buffer, long number);
        static std::pair<seq::Tempo, seq::TimeSignature> measure_type(MeasureIter iter, const std::vector<seq::Measure>& measures);
        static void set_tempo(seq::Measure& measure, const ui::Tempo& tempo);
        static void set_tempo(ui::Tempo& tempo, const seq::Measure& measure);
        static void set_time_signature(seq::Measure& measure, const ui::TimeSignature& time_signature);
        static void set_time_signature(ui::TimeSignature& time_signature, const seq::Measure& measure);
        static bool empty_except_metronome(const seq::Measure& measure);
        static seq::Value get_value(ui::Value value);

        task::TaskManager m_task_manager;

        syn::Voice m_voice {syn::VoiceBell};
        unsigned int m_octave {syn::Octave3};

        ImVec2 m_composition_camera;
        MeasureIter m_composition_selected_measure;
        std::vector<SelectedNote> m_composition_selected_notes;
        seq::Composition m_composition;

        synthesizer::Synthesizer m_synthesizer;
        seq::Player m_player;

        struct {
            int tool {ui::ToolMeasure};
            int value {ui::ValueQuarter};
            ui::Voice voice {ui::VoiceBell};
            ui::Tempo tempo {seq::Tempo()};
            ui::TimeSignature time_signature;
            ui::ColorScheme color_scheme {ui::ColorSchemeClassic};
            ui::Scale scale {ui::Scale1X};
            int octave {};
            unsigned int device {};
            double volume {};
        } m_ui;

        bool m_metronome {};
        bool m_composition_modified {};
    };
}
