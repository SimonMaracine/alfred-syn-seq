#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <cmath>

namespace synthesizer {
    void Synthesizer::note_on(syn::Name name, syn::Octave octave, syn::Voice voice) {
        audio::AudioLockGuard guard {this};

        const auto iter {find_note(name, octave)};

        if (iter == m_notes.end()) {
            syn::Note& note {m_notes.emplace_back()};
            note.name = name;
            note.octave = octave;
            note.voice = voice;
            note.time_on = m_time;
        } else {
            if (iter->time_off > iter->time_on) {
                iter->time_on = m_time;
            }
        }
    }

    void Synthesizer::note_off(syn::Name name, syn::Octave octave) {
        audio::AudioLockGuard guard {this};

        const auto iter {find_note(name, octave)};

        if (iter != m_notes.end()) {
            if (iter->time_on > iter->time_off) {
                iter->time_off = m_time;
            }
        }
    }

    void Synthesizer::silence() {
        audio::AudioLockGuard guard {this};

        m_notes.clear();
    }

    void Synthesizer::set_volume(double volume) {
        m_volume = std::min(std::max(volume, 0.0), 1.0);
    }

    void Synthesizer::update() {
        audio::AudioLockGuard guard {this};

        m_notes.erase(std::remove_if(m_notes.begin(), m_notes.end(), [this](const syn::Note& note) {
            return m_voices[note.voice].get_envelope().is_done(m_time, note.time_on, note.time_off);
        }), m_notes.end());
    }

    std::vector<syn::Note>::iterator Synthesizer::find_note(syn::Name name, syn::Octave octave) {
        return std::find_if(m_notes.begin(), m_notes.end(),
            [name, octave](const syn::Note& note) {
                return note.name == name && note.octave == octave;
            }
        );
    }

    double Synthesizer::sound(double time) const {
        double result {};

        for (const syn::Note& note : m_notes) {
            result += m_voices[note.voice].sound(time, note);
        }

        return result;
    }

    double Synthesizer::volume() const {
        return m_volume;
    }
}
