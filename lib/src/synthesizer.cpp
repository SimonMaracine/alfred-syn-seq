#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <tuple>
#include <ranges>

#include "alfred/instrument.hpp"

namespace synthesizer {
    static constexpr std::size_t MAX_VOICES {8};

    Synthesizer::Synthesizer() {
        m_instruments[instrument::Metronome::static_id()] = std::make_unique<instrument::Metronome>();
        m_instruments[instrument::Bell::static_id()] = std::make_unique<instrument::Bell>();
        m_instruments[instrument::Harmonica::static_id()] = std::make_unique<instrument::Harmonica>();
        m_instruments[instrument::DrumBass::static_id()] = std::make_unique<instrument::DrumBass>();
        m_instruments[instrument::DrumSnare::static_id()] = std::make_unique<instrument::DrumSnare>();
        m_instruments[instrument::DrumHiHat::static_id()] = std::make_unique<instrument::DrumHiHat>();
        m_instruments[instrument::Piano::static_id()] = std::make_unique<instrument::Piano>();
        m_instruments[instrument::Guitar::static_id()] = std::make_unique<instrument::Guitar>();
        m_instruments[instrument::Strings::static_id()] = std::make_unique<instrument::Strings>();
    }

    Synthesizer::~Synthesizer() noexcept {
        if (!*this) {
            return;
        }

        // Prevent audio from being processed in the base class destructor, causing pure virtual function calls
        halt();
    }

    void Synthesizer::note_on(syn::NoteName name, syn::NoteOctave octave, syn::InstrumentId instrument) {
        note_on(syn::Voice::get_note(name, octave), instrument);
    }

    void Synthesizer::note_off(syn::NoteName name, syn::NoteOctave octave, syn::InstrumentId instrument) {
        note_off(syn::Voice::get_note(name, octave), instrument);
    }

    void Synthesizer::note_on(syn::NoteId note, syn::InstrumentId instrument) {
        if (const auto [begin, end] {m_instruments.at(instrument)->range()}; note < begin || note > end) {
            return;
        }

        audio::AudioLockGuard guard {this};

        if (const auto iter {find_voice(note)}; iter == m_voices.end()) {
            syn::Voice& voice {m_voices.emplace_back()};
            voice.note = note;
            voice.instrument = instrument;
            voice.envelope = m_instruments.at(instrument)->new_envelope();
            voice.time_on = time();
        } else {
            if (iter->time_off > iter->time_on) {
                iter->time_on = time();
            }
        }
    }

    void Synthesizer::note_off(syn::NoteId note, syn::InstrumentId instrument) {
        if (const auto [begin, end] {m_instruments.at(instrument)->range()}; note < begin || note > end) {
            return;
        }

        audio::AudioLockGuard guard {this};

        if (const auto voice {find_voice(note)}; voice != m_voices.end()) {
            if (voice->time_on > voice->time_off) {
                voice->time_off = time();
            }
        }
    }

    void Synthesizer::silence() {
        audio::AudioLockGuard guard {this};

        m_voices.clear();
    }

    void Synthesizer::update() {
        audio::AudioLockGuard guard {this};

        std::erase_if(m_voices, [this](const syn::Voice& voice) {
            return voice.envelope->is_done(time(), voice.time_on, voice.time_off);
        });

        while (m_voices.size() > MAX_VOICES) {
            // Replace the oldest voice policy
            m_voices.erase(std::ranges::min_element(m_voices, [](const auto& lhs, const auto& rhs) {
                return lhs.time_on < rhs.time_on;
            }));
        }
    }

    void Synthesizer::for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const {
        for (const auto& instrument : m_instruments | std::views::values) {
            function(*instrument);
        }
    }

    const char* Synthesizer::instrument_name(syn::InstrumentId instrument) const {
        return m_instruments.at(instrument)->name();
    }

    double Synthesizer::sound(double time) const {
        double output {};

        for (const auto& [i, voice] : m_voices | std::views::enumerate) {
            output += m_instruments.at(voice.instrument)->sound(time, voice);

            // The update function should take care of removing voices, if there are too many
            if (i == MAX_VOICES) {
                break;
            }
        }

        return output / double(MAX_VOICES);
    }

    std::vector<syn::Voice>::iterator Synthesizer::find_voice(syn::NoteId note) {
        return std::ranges::find_if(m_voices, [note](const syn::Voice& voice) {
            return voice.note == note;
        });
    }
}
