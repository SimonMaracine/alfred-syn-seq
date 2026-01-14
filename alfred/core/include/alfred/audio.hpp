#pragma once

#include <stdexcept>

struct SDL_AudioStream;

namespace audio {
    class Audio {
    public:
        Audio();
        virtual ~Audio();

        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;
        Audio(Audio&&) = delete;
        Audio& operator=(Audio&&) = delete;

        void resume() const;
        void lock() const;
        void unlock() const;

        double get_time() const { return m_time; }

        virtual double sound(double time) const = 0;
        virtual double volume() const = 0;
    protected:
        double current_sound() const;
        static double clamp(double value);
        static void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);

        SDL_AudioStream* m_stream {};
        double m_time {};
    };

    class AudioLockGuard {
    public:
        AudioLockGuard(const Audio* audio);
        ~AudioLockGuard();

        AudioLockGuard(const AudioLockGuard&) = default;
        AudioLockGuard& operator=(const AudioLockGuard&) = default;
        AudioLockGuard(AudioLockGuard&&) = default;
        AudioLockGuard& operator=(AudioLockGuard&&) = default;
    private:
        const Audio* m_audio {};
    };

    struct AudioError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
