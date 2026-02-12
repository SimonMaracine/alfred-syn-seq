#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

namespace synthesizer {
    class Synthesizer : public audio::Audio {
    public:
        Synthesizer();
        ~Synthesizer() noexcept override;

        void note_on(syn::Name name, syn::Octave octave, syn::InstrumentId instrument);
        void note_off(syn::Name name, syn::Octave octave, syn::InstrumentId instrument);
        void note_on(syn::Id id, syn::InstrumentId instrument);
        void note_off(syn::Id id, syn::InstrumentId instrument);
        void silence();
        void update();
        void for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const;
        const char* instrument_name(syn::InstrumentId instrument) const;
    private:
        double sound(double time) const override;

        std::vector<syn::Note>::iterator find_note(syn::Id id);

        std::unordered_map<syn::InstrumentId, std::unique_ptr<syn::Instrument>> m_instruments;
        std::vector<syn::Note> m_notes;
    };
}
