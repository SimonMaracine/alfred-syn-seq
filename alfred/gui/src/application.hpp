#pragma once

#include <imgui.h>
#include <alfred/synthesizer.hpp>

#include "video.hpp"

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
    void main_menu_bar_options();
    void main_menu_bar_help();
    void keyboard();
    void keyboard_key(ImDrawList* list, ImVec2 origin, char key, float x, float y, int scancode);

    syn::Voice m_voice {};
    syn::Octave m_octave {syn::Octave1};
    synthesizer::Synthesizer m_synthesizer;
    const bool* m_keyboard {};

    enum ColorScheme {
        ColorSchemeDark,
        ColorSchemeLight,
        ColorSchemeClassic
    } m_color_scheme {ColorSchemeClassic};
};
