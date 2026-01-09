#pragma once

#include <imgui.h>
#include <alfred/synthesizer.hpp>

#include "video.hpp"
#include "sequencer.hpp"

class Application : public Video {
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
    void playback();
    void debug();

    void update_keyboard_input(unsigned int key, bool down);

    syn::Voice m_voice {};
    syn::Octave m_octave {syn::Octave1};

    Composition m_composition;

    synthesizer::Synthesizer m_synthesizer;
    Player m_player;

    enum ColorScheme {
        ColorSchemeDark,
        ColorSchemeLight,
        ColorSchemeClassic
    } m_color_scheme {ColorSchemeClassic};

    bool m_metronome {};
};
