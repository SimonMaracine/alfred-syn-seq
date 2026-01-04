#pragma once

#include <alfred/synthesizer.hpp>

#include "video.hpp"

class Application : public Video {
public:
    void on_start() override;
    void on_event(const SDL_Event& event) override;
    void on_update() override;
private:
    syn::Voice m_voice {};
    syn::Octave m_octave {syn::Octave1};
    Synthesizer m_synthesizer;
};
