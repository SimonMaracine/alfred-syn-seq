#pragma once

#include <stdexcept>
#include <span>
#include <utility>
#include <vector>
#include <atomic>
#include <cstdint>

// Nyquist criterion
// The sampling frequency must be at least twice the highest frequency in the signal

struct SDL_AudioStream;

namespace audio {
    inline constexpr int SAMPLE_FREQUENCY {44100};

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
        double time() const { return m_time; }

        virtual void update() = 0;
        virtual double sound() const = 0;
    private:
        static double clamp(double value);
        static void stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);

        SDL_AudioStream* m_stream {};
        std::atomic<double> m_time {0.0};
        std::vector<Device> m_devices;
    };

    class AudioLockGuard {
    public:
        AudioLockGuard(const Audio* audio);
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
