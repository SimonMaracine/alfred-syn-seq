#pragma once

#include <stdexcept>
#include <span>
#include <utility>
#include <vector>
#include <cstdint>

// Nyquist criterion
// The sampling frequency must be at least twice the highest frequency in the signal

struct SDL_AudioStream;

namespace audio {
    inline constexpr int SAMPLE_FREQUENCY {44100};
    inline constexpr int BITS_PER_SAMPLE {16};

    using Resolution = std::int16_t;

    class Audio {
    public:
        Audio();
        virtual ~Audio();

        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;
        Audio(Audio&&) = delete;
        Audio& operator=(Audio&&) = delete;

        using Device = std::pair<unsigned int, const char*>;
        using Devices = std::span<const Device>;

        static const char* driver();
        Device device() const;
        Devices devices() const;
        void query_devices();

        void open();
        void open(unsigned int device);
        void close() const;
        void resume() const;
        void halt() const;
        void lock() const;
        void unlock() const;
        void volume(double volume) const;
        double volume() const;

        operator bool() const { return m_stream; }

        virtual void callback_update() noexcept = 0;
        virtual double callback_sound() const noexcept = 0;
    protected:
        double m_time {};
    private:
        static void stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) noexcept;

        SDL_AudioStream* m_stream {};

        std::vector<Device> m_devices;
    };

    class AudioLockGuard {
    public:
        explicit AudioLockGuard(const Audio* audio);
        ~AudioLockGuard() noexcept;

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
