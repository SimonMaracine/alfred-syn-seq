#include <cstdlib>
#include <cstring>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "application.hpp"
#include "utility.hpp"
#include "logging.hpp"
#include "error.hpp"
#include "version.hpp"

static const char* sample_frames(int argc, char** argv) {
    if (argc > 1) {
        if (std::strcmp(argv[1], "--low-latency") == 0) {
            logging::information("Low latency");
            return "256";
        }

        if (std::strcmp(argv[1], "--high-latency") == 0) {
            logging::information("High latency");
            return "1024";
        }
    }

    return "512";
}

int main(int argc, char** argv) {
    (void) std::atexit(logging::uninitialize);
    (void) std::atexit(SDL_Quit);

    try {
        logging::initialize();
    } catch (const logging::LoggingError& e) {
        logging::error("Could not initialize logging: {}", e.what());
    }

    if (!SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, sample_frames(argc, argv))) {
        logging::error("SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES)");
    }

    if (!SDL_SetHint(SDL_HINT_AUDIO_DEVICE_RAW_STREAM, "1")) {
        logging::error("SDL_SetHint(SDL_HINT_AUDIO_DEVICE_RAW_STREAM)");
    }

    utility::set_property(SDL_PROP_APP_METADATA_NAME_STRING, "Alfred");
    utility::set_property(SDL_PROP_APP_METADATA_VERSION_STRING, ALFRED_VERSION);
    utility::set_property(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "dev.simonmara.alfred");
    utility::set_property(SDL_PROP_APP_METADATA_CREATOR_STRING, "Simon");
    utility::set_property(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/SimonMaracine/alfred-syn-seq");
    utility::set_property(SDL_PROP_APP_METADATA_TYPE_STRING, "application");

    logging::information("Version: {}", utility::get_property(SDL_PROP_APP_METADATA_VERSION_STRING));

    try {
        application::Application application;
        application.run();
    } catch (const video::VideoError& e) {
        logging::critical("Fatal video error: {}", e.what());
        utility::show_error_message_box("Alfred Video Error", "A critical video error occurred. Check the logs.");
        return 1;
    } catch (const audio::AudioError& e) {
        logging::critical("Fatal audio error: {}", e.what());
        utility::show_error_message_box("Alfred Audio Error", "A critical audio error occurred. Check the logs.");
        return 1;
    } catch (const error::Error& e) {
        logging::critical("Fatal error: {}", e.what());
        utility::show_error_message_box("Alfred Error", "A critical error occurred. Check the logs.");
        return 1;
    }
#ifdef ALFRED_DISTRIBUTION
    catch (...) {
        logging::critical("Unknown exception");
        utility::show_error_message_box("Alfred Unknown Exception", "An unknown exception occurred.");
        return 1;
    }
#endif

    return 0;
}
