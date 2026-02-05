#include <cstdlib>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "application.hpp"
#include "logging.hpp"

static void set_property(const char* property, const char* value) {
    (void) SDL_SetAppMetadataProperty(property, value);
}

static void show_error_message_box(const char* title, const char* message) {
    (void) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
}

int main(int, char**) {
    std::atexit(logging::uninitialize);
    std::atexit(SDL_Quit);

    try {
        logging::initialize();
    } catch (const logging::LoggingError& e) {
        logging::error("Could not initialize logging: {}", e.what());
    }

    if (!SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "512")) {
        logging::error("SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES)");
    }

    set_property(SDL_PROP_APP_METADATA_NAME_STRING, "Alfred");
    set_property(SDL_PROP_APP_METADATA_VERSION_STRING, "0.1.0");
    set_property(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "dev.simonmara.alfred");
    set_property(SDL_PROP_APP_METADATA_CREATOR_STRING, "Simon");
    set_property(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/SimonMaracine/alfred");
    set_property(SDL_PROP_APP_METADATA_TYPE_STRING, "application");

    try {
        application::Application application;
        application.run();
    } catch (const video::VideoError& e) {
        logging::critical("Fatal video error: {}", e.what());
        show_error_message_box("Alfred Video Error", "A critical video error occurred. Check the logs.");
        return 1;
    } catch (const audio::AudioError& e) {
        logging::critical("Fatal audio error: {}", e.what());
        show_error_message_box("Alfred Audio Error", "A critical audio error occurred. Check the logs.");
        return 1;
    }
#ifdef ALFRED_DISTRIBUTION
    catch (...) {
        logging::critical("Unknown fatal error");
        show_error_message_box("Alfred Unknown Error", "An unknown critical error occurred.");
        return 1;
    }
#endif

    return 0;
}
