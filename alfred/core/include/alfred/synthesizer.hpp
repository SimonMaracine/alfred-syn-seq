#pragma once

#include <vector>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

class Synthesizer : public Audio {
public:
    void note_on(syn::Note note, syn::Octave octave, syn::Voice voice);
    void note_off(syn::Note note, syn::Octave octave);
    void set_volume(double volume);
    void update();
private:
    std::vector<syn::Sound>::iterator find_sound(syn::Note note, syn::Octave octave);

    double sound(double time) const override;
    double volume() const override;

    syn::Voices m_voices;
    std::vector<syn::Sound> m_sounds;
    double m_volume {0.5};
};
