#pragma once

#include <utility>

#include <imgui.h>
#include <alfred/synthesizer.hpp>

#include "video.hpp"
#include "sequencer.hpp"
#include "ui.hpp"

namespace application {
    class Application : public video::Video {
    public:
        void on_start() override;
        void on_stop() override;
        void on_update() override;
        void on_imgui() override;
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
        void playback();
        void tools();
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

        void update_keyboard_input(unsigned int key, bool down);
        void add_metronome();
        void add_metronome(MeasureIter begin, MeasureIter end);
        void remove_metronome();
        void select_measure(ImVec2 position);
        void append_measures();
        void insert_measure();
        void clear_measure();
        void delete_measure();
        void delete_notes(syn::Voice voice, unsigned int begin, unsigned int end);
        void delete_notes(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end);
        void shift_notes_left(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end, unsigned int steps);
        void shift_notes_right(std::vector<seq::Note>& notes, unsigned int begin, unsigned int end, unsigned int steps);

        static float note_height(const seq::Note& note);
        static const char* measure_label(char* buffer, long number);
        static std::pair<seq::Tempo, seq::TimeSignature> measure_type(MeasureIter iter, const std::vector<seq::Measure>& measures);
        static void set_tempo(seq::Measure& measure, const ui::Tempo& tempo);
        static void set_tempo(ui::Tempo& tempo, const seq::Measure& measure);
        static void set_time_signature(seq::Measure& measure, const ui::TimeSignature& time_signature);
        static void set_time_signature(ui::TimeSignature& time_signature, const seq::Measure& measure);

        syn::Voice m_voice {syn::VoiceBell};
        unsigned int m_octave {syn::Octave3};

        ImVec2 m_composition_camera;
        MeasureIter m_composition_selected_measure;
        seq::Composition m_composition;

        synthesizer::Synthesizer m_synthesizer;
        seq::Player m_player;

        ui::Tempo m_tempo {seq::Tempo()};
        ui::TimeSignature m_time_signature;
        ui::ColorScheme m_color_scheme {ui::ColorSchemeClassic};

        bool m_metronome {};
        bool m_composition_modified {};
    };
}
