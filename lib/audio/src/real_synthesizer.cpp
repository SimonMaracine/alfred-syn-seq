#include "alfred/real_synthesizer.hpp"

namespace alfred::synthesizer {
    static constexpr double TIME_FOR_RESET = 10.0;

    RealSynthesizer::~RealSynthesizer() noexcept {
        if (!*this) {
            return;
        }

        // Prevent audio from being processed in the base class destructor, causing pure virtual function calls
        pause();
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

        // Time is of course monotonic
        if (!m_voices.empty()) {
            m_time_sound = m_time;
        }

        // Reset the time after a certain period of inactivity in order to never lose precision
        if (m_time - m_time_sound > TIME_FOR_RESET) {
            m_time_sound = 0.0;
            silence_immediately();
        }

        return sample();
    }

    void RealSynthesizer::silence() {
        audio::AudioLockGuard guard {this};

        for (const auto& voice : m_voices) {
            Synthesizer::note_off(m_time, *voice);
        }
    }

    void RealSynthesizer::silence_immediately() {
        audio::AudioLockGuard guard {this};

        m_time = 0.0;
        m_voices.clear();
    }

    void RealSynthesizer::polyphony(std::size_t max_voices) {
        audio::AudioLockGuard guard {this};

        set_polyphony(max_voices);
    }

    bool RealSynthesizer::store_instrument(std::unique_ptr<syn::Instrument> instrument) {
        audio::AudioLockGuard guard {this};

        return insert_instrument(std::move(instrument));
    }

    void RealSynthesizer::callback_update() noexcept {
        sample_update(m_time);
    }

    double RealSynthesizer::callback_sound() const noexcept {
        return sample_sound(m_time);
    }
}
