#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <ranges>
#include <cassert>

#include "alfred/instrument.hpp"
#include "alfred/math.hpp"

namespace synthesizer {
    Synthesizer::Synthesizer() {
        m_instruments[instrument::Metronome::static_id()] = std::make_unique<instrument::Metronome>();
        m_instruments[instrument::Bell::static_id()] = std::make_unique<instrument::Bell>();
        m_instruments[instrument::Harmonica::static_id()] = std::make_unique<instrument::Harmonica>();
        m_instruments[instrument::DrumBass::static_id()] = std::make_unique<instrument::DrumBass>();
        m_instruments[instrument::DrumSnare::static_id()] = std::make_unique<instrument::DrumSnare>();
        m_instruments[instrument::DrumHiHat::static_id()] = std::make_unique<instrument::DrumHiHat>();
        m_instruments[instrument::SynthPiano::static_id()] = std::make_unique<instrument::SynthPiano>();
        m_instruments[instrument::Guitar::static_id()] = std::make_unique<instrument::Guitar>();
        m_instruments[instrument::Strings::static_id()] = std::make_unique<instrument::Strings>();
        m_instruments[instrument::Cello::static_id()] = std::make_unique<instrument::Cello>();
    }

    void Synthesizer::for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const {
        for (const auto& instrument : m_instruments | std::views::values) {
            function(*instrument);
        }
    }

    void Synthesizer::for_each_instrument(const std::function<void(syn::Instrument&)>& function) {
        for (const auto& instrument : m_instruments | std::views::values) {
            function(*instrument);
        }
    }

    const syn::Instrument& Synthesizer::get_instrument(syn::InstrumentId instrument) const {
        return *m_instruments.at(instrument);
    }

    syn::Instrument& Synthesizer::get_instrument(syn::InstrumentId instrument) {
        return *m_instruments.at(instrument);
    }

    void Synthesizer::note_on(double time, syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) {
        assert(velocity >= 0.0 && velocity <= 1.0);

        if (const auto [begin, end] {m_instruments.at(instrument)->range()}; note < begin || note > end) {
            return;
        }

        if (const auto voice {find_voice(note, instrument)}; voice == m_voices.end()) {
            syn::Voice& new_voice {m_voices.emplace_back()};
            new_voice.note = note;
            new_voice.instrument = instrument;
            new_voice.envelope = m_instruments.at(instrument)->new_envelope();
            new_voice.amplitude = velocity;
            new_voice.time_on = time;
            new_voice.envelope->note_on(time);
        } else {
            if (voice->time_off > voice->time_on) {
                voice->amplitude = velocity;
                voice->time_on = time;
                voice->envelope->note_on(time);
            }
        }
    }

    void Synthesizer::note_off(double time, syn::NoteId note, syn::InstrumentId instrument) {
        if (const auto [begin, end] {m_instruments.at(instrument)->range()}; note < begin || note > end) {
            return;
        }

        if (const auto voice {find_voice(note, instrument)}; voice != m_voices.end()) {
            if (voice->time_on > voice->time_off) {
                voice->time_off = time;
                voice->envelope->note_off(time);
            }
        }
    }

    void Synthesizer::set_polyphony(std::size_t max_voices) {
        m_max_voices = std::clamp(max_voices, MIN_VOICES, MAX_VOICES);
    }

    void Synthesizer::update_voices(double time) {
        std::erase_if(m_voices, [](const syn::Voice& voice) {
            return voice.envelope->done();
        });

        auto size {m_voices.size()};

        while (size > m_max_voices) {
            // Remove the oldest voice policy
            const auto oldest_voice {std::ranges::min_element(m_voices, [](const auto& lhs, const auto& rhs) {
                return lhs.time_on < rhs.time_on;
            })};

            // Simply force a note off on the voice and don't abruptly interrupt it
            if (oldest_voice->time_on > oldest_voice->time_off) {
                oldest_voice->time_off = time;
                oldest_voice->envelope->note_off(time);
            }

            size--;
        }
    }

    std::vector<syn::Voice>::iterator Synthesizer::find_voice(syn::NoteId note, syn::InstrumentId instrument) {
        return std::ranges::find_if(m_voices, [note, instrument](const syn::Voice& voice) {
            return voice.note == note && voice.instrument == instrument;
        });
    }

    void Synthesizer::sample_update(double time) const noexcept {
        for (const syn::Voice& voice : m_voices) {
            voice.envelope->update(time);
        }
    }

    double Synthesizer::sample_sound(double time) const noexcept {
        double output {};

        for (const syn::Voice& voice : m_voices) {
            // The update function should take care of nicely stopping voices, if there are too many

            const auto& instrument {m_instruments.at(voice.instrument)};

            output +=
                voice.amplitude *
                voice.envelope->value() *  // FIXME technically a race condition
                syn::amplitude(instrument->volume()) *
                instrument->sound(time, voice.time_on, voice.note);
        }

        return output / double(m_max_voices);
    }

    RealSynthesizer::~RealSynthesizer() noexcept {
        if (!*this) {
            return;
        }

        // Prevent audio from being processed in the base class destructor, causing pure virtual function calls
        halt();
    }

    void RealSynthesizer::note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) {
        audio::AudioLockGuard guard {this};

        Synthesizer::note_on(m_time, note, instrument, velocity);
    }

    void RealSynthesizer::note_off(syn::NoteId note, syn::InstrumentId instrument) {
        audio::AudioLockGuard guard {this};

        Synthesizer::note_off(m_time, note, instrument);
    }

    double RealSynthesizer::update() {
        audio::AudioLockGuard guard {this};

        update_voices(m_time);

        return sample();
    }

    void RealSynthesizer::silence() {
        audio::AudioLockGuard guard {this};

        m_time = 0.0;
        m_voices.clear();
    }

    void RealSynthesizer::polyphony(std::size_t max_voices) {
        audio::AudioLockGuard guard {this};

        set_polyphony(max_voices);
    }

    void RealSynthesizer::callback_update() noexcept {
        sample_update(m_time);
    }

    double RealSynthesizer::callback_sound() const noexcept {
        return sample_sound(m_time);
    }

    void VirtualSynthesizer::note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) {
        Synthesizer::note_on(m_time, note, instrument, velocity);
    }

    void VirtualSynthesizer::note_off(syn::NoteId note, syn::InstrumentId instrument) {
        Synthesizer::note_off(m_time, note, instrument);
    }

    double VirtualSynthesizer::update() {
        update_voices(m_time);

        sample_update(m_time);
        const double sample {sample_sound(m_time)};

        m_buffer.push_back(math::clamp_sample(sample));

        m_time += 1.0 / double(audio::SAMPLE_FREQUENCY);

        return sample;
    }

    void VirtualSynthesizer::silence() {
        m_time = 0.0;
        m_voices.clear();
    }

    void VirtualSynthesizer::polyphony(std::size_t max_voices) {
        set_polyphony(max_voices);
    }

    void VirtualSynthesizer::reset() {
        m_time = 0.0;
        m_buffer.clear();
        m_voices.clear();
    }
}
