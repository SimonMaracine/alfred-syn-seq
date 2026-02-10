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

        void note_on(syn::Name name, syn::Octave octave, syn::VoiceId voice);
        void note_off(syn::Name name, syn::Octave octave, syn::VoiceId voice);
        void note_on(syn::Id id, syn::VoiceId voice);
        void note_off(syn::Id id, syn::VoiceId voice);
        void silence();
        void update();
        void for_each_voice(const std::function<void(const syn::Voice&)> &function) const;
        const char* voice_name(syn::VoiceId voice) const;
    private:
        double sound(double time) const override;

        std::vector<syn::Note>::iterator find_note(syn::Id id);

        std::unordered_map<syn::VoiceId, std::unique_ptr<syn::Voice>> m_voices;
        std::vector<syn::Note> m_notes;
    };
}
