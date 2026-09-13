#include <cstdlib>
#include <cstring>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "application.hpp"
#include "utility.hpp"
#include "log.hpp"
#include "error.hpp"
#include "version.hpp"

using namespace alfred;

static const char* sample_frames(int argc, char** argv) {
    if (argc > 1) {
        if (std::strcmp(argv[1], "--low-latency") == 0) {
            log::information("Low latency");
            return "256";
        }

        if (std::strcmp(argv[1], "--high-latency") == 0) {
            log::information("High latency");
            return "1024";
        }
    }

    return "512";
}

int main(int argc, char** argv) {
    (void) std::atexit(log::uninitialize);
    (void) std::atexit(SDL_Quit);

    utility::initialize_file_paths("simonmara", "alfred");

    if (!log::initialize()) {
        log::error("Could not initialize logging");
    }

    if (!SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, sample_frames(argc, argv))) {
        log::error("SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES)");
    }

    if (!SDL_SetHint(SDL_HINT_AUDIO_DEVICE_RAW_STREAM, "1")) {
        log::error("SDL_SetHint(SDL_HINT_AUDIO_DEVICE_RAW_STREAM)");
    }

    utility::set_property(SDL_PROP_APP_METADATA_NAME_STRING, "Alfred");
    utility::set_property(SDL_PROP_APP_METADATA_VERSION_STRING, ALFRED_VERSION);
    utility::set_property(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "dev.simonmara.alfred");
    utility::set_property(SDL_PROP_APP_METADATA_CREATOR_STRING, "Simon");
    utility::set_property(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/SimonMaracine/alfred-syn-seq");
    utility::set_property(SDL_PROP_APP_METADATA_TYPE_STRING, "application");

    log::information("Version: {}", utility::get_property(SDL_PROP_APP_METADATA_VERSION_STRING));

    try {
        application::Application application;
        application.run();
    } catch (const video::VideoError& e) {
        log::critical("Fatal video error: {}", e.what());
        utility::show_error_message_box("Alfred Video Error", "A critical video error occurred. Check the logs.");
        return 1;
    } catch (const audio::AudioError& e) {
        log::critical("Fatal audio error: {}", e.what());
        utility::show_error_message_box("Alfred Audio Error", "A critical audio error occurred. Check the logs.");
        return 1;
    } catch (const error::Error& e) {
        log::critical("Fatal error: {}", e.what());
        utility::show_error_message_box("Alfred Error", "A critical error occurred. Check the logs.");
        return 1;
    }
#ifdef ALFRED_DISTRIBUTION
    catch (...) {
        log::critical("Unknown exception");
        utility::show_error_message_box("Alfred Unknown Exception", "An unknown exception occurred.");
        return 1;
    }
#endif

    return 0;
}
