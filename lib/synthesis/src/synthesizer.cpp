#include "alfred/synthesizer.hpp"

#include <algorithm>
#include <ranges>
#include <cassert>

#include "alfred/instruments.hpp"
#include "alfred/math.hpp"
#include "alfred/definitions.hpp"

namespace alfred::synthesizer {
    Synthesizer::Synthesizer() {
        // Built-in instruments
        insert_instrument(std::make_unique<instruments::ShortSynthPiano>());
        insert_instrument(std::make_unique<instruments::Metronome>());
        insert_instrument(std::make_unique<instruments::Ghost>());
        insert_instrument(std::make_unique<instruments::Harmonica>());
        insert_instrument(std::make_unique<instruments::DrumBass>());
        insert_instrument(std::make_unique<instruments::DrumSnare>());
        insert_instrument(std::make_unique<instruments::DrumHiHat>());
        insert_instrument(std::make_unique<instruments::SynthPiano>());
        insert_instrument(std::make_unique<instruments::Guitar>());
        insert_instrument(std::make_unique<instruments::Strings>());
        insert_instrument(std::make_unique<instruments::Cello>());

        // Small optimization
        m_voices.reserve(50);
    }

    void Synthesizer::for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const {
        for (const auto& instrument : m_instruments | std::views::values) {
            function(*instrument);
        }
    }

    const syn::Instrument& Synthesizer::get_instrument(syn::InstrumentId instrument) const {
        return *m_instruments.at(instrument);
    }

    syn::volume::Volume Synthesizer::mixer_volume(syn::InstrumentId instrument) const {
        return m_volumes.at(instrument).load(std::memory_order::relaxed);
    }

    void Synthesizer::mixer_volume(syn::InstrumentId instrument, syn::volume::Volume volume) {
        m_volumes.at(instrument).store(volume, std::memory_order::relaxed);
    }

    void Synthesizer::mixer_reset() {
        for (auto& volume : m_volumes | std::views::values) {
            volume.store(syn::volume::DEFAULT, std::memory_order::relaxed);
        }
    }

    void Synthesizer::merge_instruments(const Synthesizer& other) {
        // Need to make a deep copy of the instruments in this fashion

        std::unordered_map<syn::InstrumentId, syn::InstrumentPtr> instruments;

        for (const auto& [id, instrument] : other.m_instruments) {
            instruments[id] = instrument->clone();
        }

        m_instruments.merge(std::move(instruments));
    }

    bool Synthesizer::insert_instrument(std::unique_ptr<syn::Instrument> instrument) {
        // Add new or override if already existing
        // Return true if inserted new

        const syn::InstrumentId id = instrument->id();
        const auto iter = m_instruments.find(id);
        const bool present = iter != m_instruments.end();

        if (present) {
            iter->second = std::move(instrument);
        } else {
            m_instruments[id] = std::move(instrument);
            m_volumes[id] = syn::volume::DEFAULT;
        }

        return !present;
    }

    bool Synthesizer::in_range(syn::NoteId note, syn::InstrumentId instrument) const {
        const auto [begin, end] = m_instruments.at(instrument)->range();
        return note >= begin && note <= end;
    }

    void Synthesizer::note_on(double time, syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) {
        assert(velocity >= 0.0 && velocity <= 1.0);

        if (!in_range(note, instrument)) {
            return;
        }

        if (const auto voice = find_voice(note, instrument); voice == m_voices.end()) {
			const auto& instrument_obj = m_instruments.at(instrument);

            auto new_voice = instrument_obj->new_voice();
            new_voice->note = note;
            new_voice->instrument = instrument;
            new_voice->overall_envelope = instrument_obj->new_overall_envelope();
            new_voice->amplitude = velocity;
            new_voice->time_on = time;
            new_voice->overall_envelope->note_on(time);
            new_voice->note_on(time);

            m_voices.push_back(std::move(new_voice));
        } else {
            if ((*voice)->time_off > (*voice)->time_on) {
                (*voice)->amplitude = velocity;
                (*voice)->time_on = time;
                (*voice)->overall_envelope->note_on(time);
                (*voice)->note_on(time);
            }
        }
    }

    void Synthesizer::note_off(double time, syn::NoteId note, syn::InstrumentId instrument) {
        if (!in_range(note, instrument)) {
            return;
        }

        if (const auto voice = find_voice(note, instrument); voice != m_voices.end()) {
            note_off(time, **voice);
        }
    }

    void Synthesizer::note_off(double time, syn::voice::Voice& voice) {
        if (voice.time_on > voice.time_off) {
            voice.time_off = time;
            voice.overall_envelope->note_off(time);
            voice.note_off(time);
        }
    }

    void Synthesizer::set_polyphony(std::size_t max_voices) {
        m_max_voices = std::clamp(max_voices, MIN_VOICES, MAX_VOICES);
    }

    void Synthesizer::update_voices(double time) {
        std::erase_if(m_voices, [](const auto& voice) {
            return voice->overall_envelope->done();
        });

        auto size = m_voices.size();

        while (size > m_max_voices) {
            // Remove the oldest voice policy
            const auto oldest_voice = std::ranges::min_element(m_voices, [](const auto& lhs, const auto& rhs) {
                return lhs->time_on < rhs->time_on;
            });

            // Simply force a note off on the voice and don't abruptly interrupt it
            note_off(time, **oldest_voice);

            size--;
        }
    }

    std::vector<syn::voice::Ptr>::iterator Synthesizer::find_voice(syn::NoteId note, syn::InstrumentId instrument) {
        return std::ranges::find_if(m_voices, [note, instrument](const auto& voice) {
            return voice->note == note && voice->instrument == instrument;
        });
    }

    void Synthesizer::sample_update(double time) const noexcept {
        for (const auto& voice : m_voices) {
            voice->overall_envelope->update(time);
            voice->update(time);
        }
    }

    double Synthesizer::sample_sound(double time) const noexcept {
        double output {};

        // The update function should take care of nicely stopping voices, if there are too many
        for (const auto& voice : m_voices) {
            // These throw, if the voice's instrument somehow isn't found
            // It is important that this function only reads data from the synthesizer
            const auto& instrument = m_instruments.at(voice->instrument);
            const auto& volume = m_volumes.at(voice->instrument);

            output +=
                voice->amplitude *
                voice->overall_envelope->value() *
                syn::volume::amplitude(volume.load(std::memory_order::relaxed)) *
                instrument->sound(time, *voice);
        }

        return output / double(m_max_voices);
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
        const double sample = sample_sound(m_time);

        m_buffer.push_back(math::clamp_sample(sample));

        m_time += 1.0 / double(def::SAMPLE_FREQUENCY);

        return sample;
    }

    void VirtualSynthesizer::silence() {
        for (const auto& voice : m_voices) {
            Synthesizer::note_off(m_time, *voice);
        }
    }

    void VirtualSynthesizer::silence_immediately() {
        m_time = 0.0;
        m_voices.clear();
    }

    void VirtualSynthesizer::polyphony(std::size_t max_voices) {
        set_polyphony(max_voices);
    }

    bool VirtualSynthesizer::store_instrument(std::unique_ptr<syn::Instrument> instrument) {
        return insert_instrument(std::move(instrument));
    }

    void VirtualSynthesizer::reset() {
        m_time = 0.0;
        m_buffer.clear();
        m_voices.clear();
    }
}
