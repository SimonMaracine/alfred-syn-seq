#include "alfred/audio.hpp"

#include <format>
#include <memory>
#include <cmath>

#include <SDL3/SDL.h>

#include "alfred/math.hpp"
#include "alfred/definitions.hpp"

namespace audio {
    Audio::Audio() {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            throw AudioError(std::format("SDL_InitSubSystem: {}", SDL_GetError()));
        }

        m_initialized = true;
    }

    Audio::~Audio() {
        if (!m_initialized) {
            return;
        }

        if (m_stream) {
            SDL_DestroyAudioStream(m_stream);
        }

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    Audio::Audio(Audio&& other) noexcept
        : m_time(other.m_time),
        m_stream(std::exchange(other.m_stream, nullptr)),
        m_devices(std::move(other.m_devices)),
        m_sample(other.m_sample),
        m_initialized(std::exchange(other.m_initialized, false)) {}

    Audio& Audio::operator=(Audio&& other) noexcept {
        m_time = other.m_time;
        m_stream = std::exchange(other.m_stream, nullptr);
        m_devices = std::move(other.m_devices);
        m_sample = other.m_sample;
        m_initialized = std::exchange(other.m_initialized, false);

        return *this;
    }

    const char* Audio::driver() {
        const char* driver = SDL_GetCurrentAudioDriver();

        if (!driver) {
            return "[...]";
        }

        return driver;
    }

    Audio::Device Audio::device() const {
        const SDL_AudioDeviceID device = SDL_GetAudioStreamDevice(m_stream);

        if (!device) {
            throw AudioError(std::format("SDL_GetAudioStreamDevice: {}", SDL_GetError()));
        }

        const char* name = SDL_GetAudioDeviceName(device);

        if (!name) {
            name = "[...]";
        }

        return { device, name };
    }

    Audio::Devices Audio::devices() const {
        return { m_devices.begin(), m_devices.end() };
    }

    void Audio::query_devices() {
        int count {};
        auto devices = std::unique_ptr<SDL_AudioDeviceID[], decltype(&SDL_free)>(SDL_GetAudioPlaybackDevices(&count), SDL_free);

        if (!devices) {
            throw AudioError(std::format("SDL_GetAudioPlaybackDevices: {}", SDL_GetError()));
        }

        m_devices.clear();

        for (std::size_t i {}; i < std::size_t(count); i++) {
            const char* name = SDL_GetAudioDeviceName(devices[i]);

            if (!name) {
                name = "[...]";
            }

            m_devices.emplace_back(devices[i], name);
        }
    }

    void Audio::open() {
        open(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    }

    void Audio::open(unsigned int device) {
        constexpr SDL_AudioSpec audio_specification {
            SDL_AUDIO_S16,
            1,
            def::SAMPLE_FREQUENCY
        };

        m_stream = SDL_OpenAudioDeviceStream(
            device,
            &audio_specification,
            &Audio::stream_callback,
            this
        );

        if (!m_stream) {
            throw AudioError(std::format("SDL_OpenAudioDeviceStream: {}", SDL_GetError()));
        }
    }

    void Audio::close() const {
        SDL_DestroyAudioStream(m_stream);
    }

    void Audio::resume() const {
        if (!SDL_ResumeAudioStreamDevice(m_stream)) {
            throw AudioError(std::format("SDL_ResumeAudioStreamDevice: {}", SDL_GetError()));
        }
    }

    void Audio::pause() const {
        if (!SDL_PauseAudioStreamDevice(m_stream)) {
            throw AudioError(std::format("SDL_PauseAudioStreamDevice: {}", SDL_GetError()));
        }

        if (!SDL_ClearAudioStream(m_stream)) {
            throw AudioError(std::format("SDL_ClearAudioStream: {}", SDL_GetError()));
        }
    }

    void Audio::lock() const {
        if (!SDL_LockAudioStream(m_stream)) {
            throw AudioError(std::format("SDL_LockAudioStream: {}", SDL_GetError()));
        }
    }

    void Audio::unlock() const {
        if (!SDL_UnlockAudioStream(m_stream)) {
            throw AudioError(std::format("SDL_UnlockAudioStream: {}", SDL_GetError()));
        }
    }

    void Audio::volume(double volume) const {
        // The gain is logarithmic, so change the curve

        if (!SDL_SetAudioStreamGain(m_stream, std::clamp(float(volume * volume), 0.0f, 1.0f))) {
            throw AudioError(std::format("SDL_SetAudioStreamGain: {}", SDL_GetError()));
        }
    }

    double Audio::volume() const {
        const float gain = SDL_GetAudioStreamGain(m_stream);

        if (gain == -1.0f) {
            throw AudioError(std::format("SDL_GetAudioStreamGain: {}", SDL_GetError()));
        }

        // Undo the changed curve

        return double(std::sqrt(gain));
    }

    thread_local struct {
        std::size_t size {};
        std::unique_ptr<def::Resolution[]> buffer;
    } g_buffer;

    void Audio::stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int) noexcept {
        // This code is run under an internal SDL mutex

        Audio& self = *static_cast<Audio*>(userdata);

        const auto samples = std::size_t(additional_amount) / sizeof(def::Resolution);

        if (g_buffer.size < samples) {
            g_buffer.size = samples;
            g_buffer.buffer = std::make_unique<def::Resolution[]>(samples);
        }

        for (std::size_t i {}; i < samples; i++) {
            self.callback_update();
            self.m_sample = self.callback_sound();

            g_buffer.buffer[i] = math::encode_sample<def::Resolution>(math::clamp_sample(self.m_sample));

            self.m_time += 1.0 / double(def::SAMPLE_FREQUENCY);
        }

        // Buffer size could be larger than the samples written!
        (void) SDL_PutAudioStreamData(stream, g_buffer.buffer.get(), int(samples * sizeof(def::Resolution)));
    }

    AudioLockGuard::AudioLockGuard(const Audio* audio)
        : m_audio(audio) {
        m_audio->lock();
    }

    AudioLockGuard::~AudioLockGuard() noexcept {
        m_audio->unlock();
    }
}
