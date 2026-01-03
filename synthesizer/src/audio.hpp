#pragma once

#include <stdexcept>

#include <SDL3/SDL.h>

class AudioStream {
public:
    AudioStream();
    virtual ~AudioStream();

    void resume() const;
    double get_time() const { return m_time; }

    virtual double sound(double time) const = 0;
    virtual double envelope(double time) const = 0;
    virtual double volume() const = 0;
protected:
    double current_sound() const;
    static double clamp(double value);
    static void audio_stream_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);

    SDL_AudioStream* m_stream {};
    double m_time {};
    int m_frequency {44100};
};

struct AudioStreamError : std::runtime_error {
    using std::runtime_error::runtime_error;
};
