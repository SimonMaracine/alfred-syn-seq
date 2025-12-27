#include <iostream>
#include <numbers>
#include <cmath>

#include "application.hpp"
#include "audio.hpp"

class SynthesizerStream : public AudioStream {
    double make_noise(double time) const override {
        return 0.2 * std::sin(440.0 * 2.0 * std::numbers::pi * time);
    }
};

class Synthesizer : public Application {
public:
    void on_start() override {
        m_audio_stream.resume();
    }

    void on_event(const SDL_Event& event) override {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                break;
        }
    }
private:
    SynthesizerStream m_audio_stream;
};

int main() {
    try {
        Synthesizer synthesizer;
        synthesizer.run();
    } catch (const ApplicationError& e) {
        std::cerr << "Fatal application error: " << e.what() << '\n';
        return 1;
    } catch (const AudioStreamError& e) {
        std::cerr << "Fatal audio error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
