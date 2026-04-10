#pragma once

#include <alfred/synthesizer.hpp>

#include "alfred/audio.hpp"

namespace alfred::synthesizer {
    // Real time synthesizer whose output is the computer speakers
    // Tied to the clock time of the audio device
    // The audio stream callback, which is running from a different thread, should only read data from the synthesizer
    class RealSynthesizer : public Synthesizer, public audio::Audio {
    public:
        using Synthesizer::polyphony;

        ~RealSynthesizer() noexcept override;

        void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) override;
        void note_off(syn::NoteId note, syn::InstrumentId instrument) override;
        double update() override;
        void silence() override;
        void silence_immediately() override;
        void polyphony(std::size_t max_voices) override;
        bool store_instrument(std::unique_ptr<syn::Instrument> instrument) override;

        // Get the time (not wall clock time)
        double time() const { return m_time; }
    protected:
        void callback_update() noexcept override;
        double callback_sound() const noexcept override;
    private:
        // Time when there is at least one voice
        double m_time_sound {};
    };
}
