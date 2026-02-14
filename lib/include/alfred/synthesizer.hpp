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

        void note_on(syn::NoteId note, syn::InstrumentId instrument);
        void note_off(syn::NoteId note, syn::InstrumentId instrument);
        void silence();
        void update_voices();
        void for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const;
        const char* instrument_name(syn::InstrumentId instrument) const;
    private:
        void update() override;
        double sound() const override;

        std::vector<syn::Voice>::iterator find_voice(syn::NoteId note);

        std::unordered_map<syn::InstrumentId, std::unique_ptr<syn::Instrument>> m_instruments;
        std::vector<syn::Voice> m_voices;
    };
}
