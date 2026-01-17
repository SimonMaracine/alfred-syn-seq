#include "alfred/audio.hpp"

#include <format>
#include <memory>
#include <limits>
#include <exception>
#include <cmath>

#include <SDL3/SDL.h>

namespace audio {
    static constexpr int FREQUENCY {44100};

    Audio::Audio() {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            throw AudioError(std::format("SDL_InitSubSystem: {}", SDL_GetError()));
        }

        get_devices();
    }

    Audio::~Audio() {
        if (m_stream) {
            SDL_DestroyAudioStream(m_stream);
        }

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    const char* Audio::driver() const {
        const char* driver {SDL_GetCurrentAudioDriver()};

        if (!driver) {
            return "[Error]";
        }

        return driver;
    }

    Audio::Devices Audio::list_devices() const {
        return { m_device_names.begin(), m_device_names.end() };
    }

    void Audio::get_devices() {
        int count {};

        SDL_AudioDeviceID* devices {SDL_GetAudioPlaybackDevices(&count)};

        if (!devices) {
            throw AudioError(std::format("SDL_GetAudioPlaybackDevices: {}", SDL_GetError()));
        }

        m_device_names.clear();

        for (int i {}; i < count; i++) {
            const char* name {SDL_GetAudioDeviceName(devices[i])};

            if (!name) {
                name = "[Error]";
            }

            m_device_names.emplace_back(devices[i], name);
        }

        SDL_free(devices);
    }

    void Audio::open() {
        open(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
    }

    void Audio::open(unsigned int device) {
        const SDL_AudioSpec audio_specification {
            SDL_AUDIO_S16,
            1,
            FREQUENCY
        };

        m_stream = SDL_OpenAudioDeviceStream(
            device,
            &audio_specification,
            &Audio::audio_stream_callback,
            this
        );

        if (!m_stream) {
            throw AudioError(std::format("SDL_OpenAudioDeviceStream: {}", SDL_GetError()));
        }

        m_device = SDL_GetAudioStreamDevice(m_stream);

        if (!m_device) {
            throw AudioError(std::format("SDL_GetAudioStreamDevice: {}", SDL_GetError()));
        }
    }

    void Audio::close() {
        SDL_DestroyAudioStream(m_stream);
    }

    void Audio::resume() const {
        if (!SDL_ResumeAudioStreamDevice(m_stream)) {
            throw AudioError(std::format("SDL_ResumeAudioStreamDevice: {}", SDL_GetError()));
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

    double Audio::current_sound() const {
        return sound(m_time) * volume();
    }

    double Audio::clamp(double value) {
        if (value >= 0.0) {
            return std::min(value, 1.0);
        } else {
            return std::max(value, -1.0);
        }
    }

    void Audio::audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
        // This code is run under a mutex lock

        Audio& self {*static_cast<Audio*>(userdata)};

        const auto buffer {std::make_unique<short[]>(additional_amount / sizeof(short))};

        for (int i {}; i < additional_amount / int(sizeof(short)); i++) {
            const double sound {clamp(self.current_sound())};

            buffer[i] = short(sound * double(std::numeric_limits<short>::max()));

            self.m_time += 1.0 / double(FREQUENCY);
        }

        (void) SDL_PutAudioStreamData(stream, buffer.get(), additional_amount / sizeof(short) * sizeof(short));
    }

    AudioLockGuard::AudioLockGuard(const Audio* audio)
        : m_audio(audio) {
        m_audio->lock();
    }

    AudioLockGuard::~AudioLockGuard() {
        try {
            m_audio->unlock();
        } catch (...) {
            std::terminate();
        }
    }
}
