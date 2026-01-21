#pragma once

#include <vector>
#include <functional>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

namespace synthesizer {
    class Synthesizer : public audio::Audio {
    public:
        void note_on(syn::Name name, syn::Octave octave, syn::Voice voice);
        void note_off(syn::Name name, syn::Octave octave);
        void note_on(syn::Id id, syn::Voice voice);
        void note_off(syn::Id id);
        void silence();
        void volume(double volume);
        void update();
        void for_each_instrument(std::function<void(const syn::Instrument&)> function) const;
        const char* instrument_name(syn::Voice voice) const;

        double volume() const override;
    private:
        double sound(double time) const override;

        std::vector<syn::Note>::iterator find_note(syn::Id id);

        syn::Voices m_voices;
        std::vector<syn::Note> m_notes;
        double m_volume {0.5};
    };
}
