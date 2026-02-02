#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <tuple>
#include <ranges>

namespace synthesizer {
    static constexpr std::size_t MAX_NOTES {8};

    Synthesizer::~Synthesizer() noexcept {
        if (!*this) {
            return;
        }

        // Prevent audio from being processed in the base class destructor, causing pure virtual function calls
        halt();
    }

    void Synthesizer::note_on(syn::Name name, syn::Octave octave, syn::Voice voice) {
        note_on(syn::Note::get_id(name, octave), voice);
    }

    void Synthesizer::note_off(syn::Name name, syn::Octave octave, syn::Voice voice) {
        note_off(syn::Note::get_id(name, octave), voice);
    }

    void Synthesizer::note_on(syn::Id id, syn::Voice voice) {
        if (const auto [begin, end] {m_voices[voice].range()}; id < begin || id > end) {
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

    void Synthesizer::note_off(syn::Id id, syn::Voice voice) {
        if (const auto [begin, end] {m_voices[voice].range()}; id < begin || id > end) {
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
            return m_voices[note.voice].overall_envelope().is_done(time(), note.time_on, note.time_off);
        });

        while (m_notes.size() > MAX_NOTES) {
            // Erase the oldest sound policy
            m_notes.erase(std::ranges::min_element(m_notes, [](const auto& lhs, const auto& rhs) {
                return lhs.time_on < rhs.time_on;
            }));
        }
    }

    void Synthesizer::for_each_instrument(std::function<void(const syn::Instrument&)> function) const {
        std::apply([function = std::move(function)](auto&&... args) {
            (function(args), ...);
        }, m_voices.get());
    }

    const char* Synthesizer::instrument_name(syn::Voice voice) const {
        return m_voices[voice].name();
    }

    double Synthesizer::sound(double time) const {
        double output {};

        for (const auto& [i, note] : m_notes | std::views::enumerate) {
            output += m_voices[note.voice].sound(time, note);

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
