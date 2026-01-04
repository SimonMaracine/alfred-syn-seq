#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <cmath>

void Synthesizer::note_on(syn::Note note, syn::Octave octave, syn::Voice voice) {
    AudioLockGuard guard {this};

    const auto iter {find_sound(note, octave)};

    if (iter == m_sounds.end()) {
        syn::Sound& sound {m_sounds.emplace_back()};
        sound.note = note;
        sound.octave = octave;
        sound.voice = voice;
        sound.time_on = get_time();
    } else {
        if (iter->time_off > iter->time_on) {
            iter->time_on = get_time();
        }
    }
}

void Synthesizer::note_off(syn::Note note, syn::Octave octave) {
    AudioLockGuard guard {this};

    const auto iter {find_sound(note, octave)};

    if (iter != m_sounds.end()) {
        if (iter->time_on > iter->time_off) {
            iter->time_off = get_time();
        }
    }
}

void Synthesizer::set_volume(double volume) {
    m_volume = std::min(std::max(volume, 0.0), 1.0);
}

void Synthesizer::update() {
    AudioLockGuard guard {this};

    m_sounds.erase(std::remove_if(m_sounds.begin(), m_sounds.end(), [this](const syn::Sound& sound) {
        return m_voices[sound.voice].get_envelope().is_done(get_time(), sound.time_on, sound.time_off);
    }), m_sounds.end());
}

std::vector<syn::Sound>::iterator Synthesizer::find_sound(syn::Note note, syn::Octave octave) {
    return std::find_if(m_sounds.begin(), m_sounds.end(),
        [note, octave](const syn::Sound& sound) {
            return sound.note == note && sound.octave == octave;
        }
    );
}

double Synthesizer::sound(double time) const {
    double result {};

    for (const syn::Sound& sound : m_sounds) {
        result += m_voices[sound.voice].sound(time, sound);
    }

    return result;
}

double Synthesizer::volume() const {
    return m_volume;
}
