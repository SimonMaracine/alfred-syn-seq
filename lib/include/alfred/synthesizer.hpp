#pragma once

#include <vector>
#include <functional>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

namespace synthesizer {
    class Synthesizer : public audio::Audio {
    public:
        ~Synthesizer() noexcept override;

        void note_on(syn::Name name, syn::Octave octave, syn::Voice voice);
        void note_off(syn::Name name, syn::Octave octave, syn::Voice voice);
        void note_on(syn::Id id, syn::Voice voice);
        void note_off(syn::Id id, syn::Voice voice);
        void silence();
        void update();
        void for_each_instrument(std::function<void(const syn::Instrument&)> function) const;
        const char* instrument_name(syn::Voice voice) const;
    private:
        double sound(double time) const override;

        std::vector<syn::Note>::iterator find_note(syn::Id id);

        syn::Voices m_voices;
        std::vector<syn::Note> m_notes;
    };
}
