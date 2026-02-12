#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <tuple>
#include <ranges>

#include "alfred/instrument.hpp"

namespace synthesizer {
    static constexpr std::size_t MAX_NOTES {8};  // Maximum 8 voices

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

    void Synthesizer::note_on(syn::Name name, syn::Octave octave, syn::InstrumentId instrument) {
        note_on(syn::Note::get_id(name, octave), instrument);
    }

    void Synthesizer::note_off(syn::Name name, syn::Octave octave, syn::InstrumentId instrument) {
        note_off(syn::Note::get_id(name, octave), instrument);
    }

    void Synthesizer::note_on(syn::Id id, syn::InstrumentId instrument) {
        if (const auto [begin, end] {m_instruments.at(instrument)->range()}; id < begin || id > end) {
            return;
        }

        audio::AudioLockGuard guard {this};

        if (const auto iter {find_note(id)}; iter == m_notes.end()) {
            syn::Note& note {m_notes.emplace_back()};
            note.id = id;
            note.instrument = instrument;
            note.envelope = m_instruments.at(instrument)->new_envelope();
            note.time_on = time();
        } else {
            if (iter->time_off > iter->time_on) {
                iter->time_on = time();
            }
        }
    }

    void Synthesizer::note_off(syn::Id id, syn::InstrumentId instrument) {
        if (const auto [begin, end] {m_instruments.at(instrument)->range()}; id < begin || id > end) {
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
            return note.envelope->is_done(time(), note.time_on, note.time_off);
        });

        while (m_notes.size() > MAX_NOTES) {
            // Erase the oldest sound policy
            m_notes.erase(std::ranges::min_element(m_notes, [](const auto& lhs, const auto& rhs) {
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

        for (const auto& [i, note] : m_notes | std::views::enumerate) {
            output += m_instruments.at(note.instrument)->sound(time, note);

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
