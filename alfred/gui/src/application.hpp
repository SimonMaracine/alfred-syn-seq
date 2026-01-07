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
    void on_render() override;
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

    void update_internals();
    void update_keyboard_input(unsigned int key, bool down);
    static double get_time();

    syn::Voice m_voice {};
    syn::Octave m_octave {syn::Octave1};

    Composition m_composition;

    synthesizer::Synthesizer m_synthesizer;
    Player m_player;

    const bool* m_keyboard {};

    double m_previous_time {};
    double m_delta_time {};

    enum ColorScheme {
        ColorSchemeDark,
        ColorSchemeLight,
        ColorSchemeClassic
    } m_color_scheme {ColorSchemeClassic};

    bool m_metronome {};
};
