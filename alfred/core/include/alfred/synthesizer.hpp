#pragma once

#include <vector>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

namespace synthesizer {
    class Synthesizer : public audio::Audio {
    public:
        void note_on(syn::Name name, syn::Octave octave, syn::Voice voice);
        void note_off(syn::Name name, syn::Octave octave);
        void silence();
        void set_volume(double volume);
        void update();
    private:
        std::vector<syn::Note>::iterator find_note(syn::Id id);

        double sound(double time) const override;
        double volume() const override;

        syn::Voices m_voices;
        std::vector<syn::Note> m_notes;
        double m_volume {0.5};
    };
}
