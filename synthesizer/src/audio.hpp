#pragma once

#include <stdexcept>

#include <SDL3/SDL.h>

class AudioStream {
public:
    AudioStream();
    virtual ~AudioStream();

    void resume() const;

    virtual double make_noise(double time) const = 0;
private:
    static double clamp(double value);
    static void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);

    SDL_AudioStream* m_stream {};
    double m_time {};
    int m_frequency {44100};
};

struct AudioStreamError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
