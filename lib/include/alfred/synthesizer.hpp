#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <span>

#include "alfred/audio.hpp"
#include "alfred/synthesis.hpp"

namespace synthesizer {
    inline constexpr std::size_t MIN_VOICES {2};
    inline constexpr std::size_t MAX_VOICES {10};

    class Synthesizer {
    public:
        Synthesizer();
        virtual ~Synthesizer() = default;

        virtual void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) = 0;
        virtual void note_off(syn::NoteId note, syn::InstrumentId instrument) = 0;
        virtual void update() = 0;
        virtual void silence() = 0;

        void note_on(double time, syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity);
        void note_off(double time, syn::NoteId note, syn::InstrumentId instrument);

        std::size_t current_voices() const { return m_voices.size(); }
        std::size_t polyphony() const { return m_max_voices; }

        // Silence the synthesizer after calling this
        void polyphony(std::size_t max_voices);

        void active_instruments(std::span<syn::InstrumentId> active_instruments);
        void for_each_instrument(const std::function<void(const syn::Instrument&)>& function) const;
        void for_each_instrument(const std::function<void(syn::Instrument&)>& function);
        const syn::Instrument& get_instrument(syn::InstrumentId instrument) const;
        syn::Instrument& get_instrument(syn::InstrumentId instrument);
    protected:
        void update_voices();
        std::vector<syn::Voice>::iterator find_voice(syn::NoteId note, syn::InstrumentId instrument);
        void mix_update(double time) const noexcept;
        double mix_sound(double time) const noexcept;

        std::unordered_map<syn::InstrumentId, std::unique_ptr<syn::Instrument>> m_instruments;
        std::vector<syn::InstrumentId> m_active_instruments;
        std::vector<syn::Voice> m_voices;
        std::atomic_size_t m_max_voices {4};
    };

    class RealSynthesizer : public Synthesizer, public audio::Audio {
    public:
        ~RealSynthesizer() noexcept override;

        void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) override;
        void note_off(syn::NoteId note, syn::InstrumentId instrument) override;
        void update() override;
        void silence() override;
    private:
        void callback_update() noexcept override;
        double callback_sound() const noexcept override;
    };

    class VirtualSynthesizer : public Synthesizer {
    public:
        void note_on(syn::NoteId note, syn::InstrumentId instrument, syn::Velocity velocity) override;
        void note_off(syn::NoteId note, syn::InstrumentId instrument) override;
        void update() override;
        void silence() override;

        void reset();

        std::size_t get_buffer_size() const { return m_buffer.size(); }
        const double* get_buffer_data() const { return m_buffer.data(); }
    private:
        double m_time {};
        std::vector<double> m_buffer;
    };
}
