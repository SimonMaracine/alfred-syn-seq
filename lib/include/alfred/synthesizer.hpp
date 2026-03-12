#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

namespace synthesizer {
    inline constexpr std::size_t MIN_VOICES = 2;
    inline constexpr std::size_t MAX_VOICES = 12;

    // The synthesizer is responsible for instruments/presets storage and for voices handling and mixing
    // It needs to be regularly updated
    class Synthesizer {
    public:
        Synthesizer();
        virtual ~Synthesizer() = default;

        // MIDI-like note events
        virtual void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) = 0;
        virtual void note_off(syn::NoteId note, syn::InstrumentId instrument) = 0;

        // Regularly called in order to keep the synthesizer functional
        virtual double update() = 0;

        // Stop any current voice
        virtual void silence() = 0;

        // Silence the synthesizer after calling this
        virtual void polyphony(std::size_t max_voices) = 0;

        std::size_t current_voices() const { return m_voices.size(); }
        std::size_t polyphony() const { return m_max_voices; }

        // Instruments/presets interrogation/mutation
        void for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const;
        void for_each_instrument(const std::function<void(syn::Instrument&)>& function);
        const syn::Instrument& get_instrument(syn::InstrumentId instrument) const;
        syn::Instrument& get_instrument(syn::InstrumentId instrument);
    protected:
        void note_on(double time, syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity);
        void note_off(double time, syn::NoteId note, syn::InstrumentId instrument);
        void set_polyphony(std::size_t max_voices);
        void update_voices(double time);
        std::vector<syn::Voice>::iterator find_voice(syn::NoteId note, syn::InstrumentId instrument);
        void sample_update(double time) const noexcept;
        double sample_sound(double time) const noexcept;

        std::unordered_map<syn::InstrumentId, std::unique_ptr<syn::Instrument>> m_instruments;
        std::vector<syn::Voice> m_voices;
        std::size_t m_max_voices = 4;
    };

    // Real time synthesizer whose output is the computer speakers
    // Tied to the clock time of the audio device
    class RealSynthesizer : public Synthesizer, public audio::Audio {
    public:
        using Synthesizer::polyphony;

        ~RealSynthesizer() noexcept override;

        void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) override;
        void note_off(syn::NoteId note, syn::InstrumentId instrument) override;
        double update() override;
        void silence() override;
        void polyphony(std::size_t max_voices) override;
    private:
        void callback_update() noexcept override;
        double callback_sound() const noexcept override;
    };

    // Synthesizer whose output is an internal buffer
    // It is not tied to some specific clock time
    // Every call of update produces one sample of output
    class VirtualSynthesizer : public Synthesizer {
    public:
        using Synthesizer::polyphony;

        void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) override;
        void note_off(syn::NoteId note, syn::InstrumentId instrument) override;
        double update() override;
        void silence() override;
        void polyphony(std::size_t max_voices) override;

        // Invalidate this synthesizer
        void reset();

        std::size_t get_buffer_size() const { return m_buffer.size(); }
        const double* get_buffer_data() const { return m_buffer.data(); }
        double* get_buffer_data() { return m_buffer.data(); }
    private:
        double m_time {};
        std::vector<double> m_buffer;
    };
}
