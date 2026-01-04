#include "audio.hpp"

#include <format>
#include <memory>
#include <limits>
#include <cmath>

AudioStream::AudioStream() {
    const SDL_AudioSpec audio_specification {
        SDL_AUDIO_S16,
        1,
        m_frequency
    };

    m_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &audio_specification,
        &AudioStream::audio_stream_callback,
        this
    );

    if (!m_stream) {
        throw AudioStreamError(std::format("SDL_OpenAudioDeviceStream: {}", SDL_GetError()));
    }
}

AudioStream::~AudioStream() {
    if (m_stream) {
        SDL_DestroyAudioStream(m_stream);
    }
}

void AudioStream::resume() const {
    if (!SDL_ResumeAudioStreamDevice(m_stream)) {
        throw AudioStreamError(std::format("SDL_ResumeAudioStreamDevice: {}", SDL_GetError()));
    }
}

void AudioStream::lock() const {
    if (!SDL_LockAudioStream(m_stream)) {
        throw AudioStreamError(std::format("SDL_LockAudioStream: {}", SDL_GetError()));
    }
}

void AudioStream::unlock() const {
    if (!SDL_UnlockAudioStream(m_stream)) {
        throw AudioStreamError(std::format("SDL_UnlockAudioStream: {}", SDL_GetError()));
    }
}

double AudioStream::current_sound() const {
    return sound(m_time) * volume();
}

double AudioStream::clamp(double value) {
    if (value >= 0.0) {
        return std::min(value, 1.0);
    } else {
        return std::max(value, -1.0);
    }
}

void AudioStream::audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
    // This code is run under a mutex lock

    AudioStream& self {*static_cast<AudioStream*>(userdata)};

    const auto buffer {std::make_unique<short[]>(additional_amount / sizeof(short))};

    for (int i {}; i < additional_amount / int(sizeof(short)); i++) {
        const double sound {clamp(self.current_sound())};

        buffer[i] = short(sound * double(std::numeric_limits<short>::max()));

        self.m_time += 1.0 / double(self.m_frequency);
    }

    (void) SDL_PutAudioStreamData(stream, buffer.get(), additional_amount / sizeof(short) * sizeof(short));
}

StreamLockGuard::StreamLockGuard(const AudioStream* audio_stream)
    : m_audio_stream(audio_stream) {
    m_audio_stream->lock();
}

StreamLockGuard::~StreamLockGuard() {
    try {
        m_audio_stream->unlock();
    } catch (...) {
        std::terminate();
    }
}
