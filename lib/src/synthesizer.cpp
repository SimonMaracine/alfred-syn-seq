#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <tuple>
#include <ranges>

#include "alfred/voice.hpp"

namespace synthesizer {
    static constexpr std::size_t MAX_NOTES {8};

    Synthesizer::Synthesizer() {
        m_voices[voice::Metronome::static_id()] = std::make_unique<voice::Metronome>();
        m_voices[voice::Bell::static_id()] = std::make_unique<voice::Bell>();
        m_voices[voice::Harmonica::static_id()] = std::make_unique<voice::Harmonica>();
        m_voices[voice::DrumBass::static_id()] = std::make_unique<voice::DrumBass>();
        m_voices[voice::DrumSnare::static_id()] = std::make_unique<voice::DrumSnare>();
        m_voices[voice::DrumHiHat::static_id()] = std::make_unique<voice::DrumHiHat>();
        m_voices[voice::Piano::static_id()] = std::make_unique<voice::Piano>();
        m_voices[voice::Guitar::static_id()] = std::make_unique<voice::Guitar>();
        m_voices[voice::Strings::static_id()] = std::make_unique<voice::Strings>();
    }

    Synthesizer::~Synthesizer() noexcept {
        if (!*this) {
            return;
        }

        // Prevent audio from being processed in the base class destructor, causing pure virtual function calls
        halt();
    }

    void Synthesizer::note_on(syn::Name name, syn::Octave octave, syn::VoiceId voice) {
        note_on(syn::Note::get_id(name, octave), voice);
    }

    void Synthesizer::note_off(syn::Name name, syn::Octave octave, syn::VoiceId voice) {
        note_off(syn::Note::get_id(name, octave), voice);
    }

    void Synthesizer::note_on(syn::Id id, syn::VoiceId voice) {
        if (const auto [begin, end] {m_voices.at(voice)->range()}; id < begin || id > end) {
            return;
        }

        audio::AudioLockGuard guard {this};

        if (const auto iter {find_note(id)}; iter == m_notes.end()) {
            syn::Note& note {m_notes.emplace_back()};
            note.id = id;
            note.voice = voice;
            note.time_on = time();
        } else {
            if (iter->time_off > iter->time_on) {
                iter->time_on = time();
            }
        }
    }

    void Synthesizer::note_off(syn::Id id, syn::VoiceId voice) {
        if (const auto [begin, end] {m_voices.at(voice)->range()}; id < begin || id > end) {
            return;
        }

        audio::AudioLockGuard guard {this};

        if (const auto iter {find_note(id)}; iter != m_notes.end()) {
            if (iter->time_on > iter->time_off) {
                iter->time_off = time();
            }
        }
    }

    void Synthesizer::silence() {
        audio::AudioLockGuard guard {this};

        m_notes.clear();
    }

    void Synthesizer::update() {
        audio::AudioLockGuard guard {this};

        std::erase_if(m_notes, [this](const syn::Note& note) {
            return m_voices.at(note.voice)->overall_envelope().is_done(time(), note.time_on, note.time_off);
        });

        while (m_notes.size() > MAX_NOTES) {
            // Erase the oldest sound policy
            m_notes.erase(std::ranges::min_element(m_notes, [](const auto& lhs, const auto& rhs) {
                return lhs.time_on < rhs.time_on;
            }));
        }
    }

    void Synthesizer::for_each_voice(const std::function<void(const syn::Voice&)>& function) const {
        for (const auto& voice : m_voices | std::views::values) {
            function(*voice);
        }
    }

    const char* Synthesizer::voice_name(syn::VoiceId voice) const {
        return m_voices.at(voice)->name();
    }

    double Synthesizer::sound(double time) const {
        double output {};

        for (const auto& [i, note] : m_notes | std::views::enumerate) {
            output += m_voices.at(note.voice)->sound(time, note);

            // The update function should take care of removing sounds, if there are too many
            if (i == MAX_NOTES) {
                break;
            }
        }

        return output / double(MAX_NOTES);
    }

    std::vector<syn::Note>::iterator Synthesizer::find_note(syn::Id id) {
        return std::ranges::find_if(m_notes, [id](const syn::Note& note) {
            return note.id == id;
        });
    }
}
