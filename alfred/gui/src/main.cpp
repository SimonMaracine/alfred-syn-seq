#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "application.hpp"

int main(int argc, char** argv) {
    (void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "Alfred");
    (void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, "0.1.0");
    (void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "dev.simonmara.alfred");
    (void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Simon");
    (void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/SimonMaracine");
    (void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "application");

    try {
        Application application;
        application.run();
    } catch (const VideoError& e) {
        std::cerr << "Fatal video error: " << e.what() << '\n';
        return 1;
    } catch (const audio::AudioError& e) {
        std::cerr << "Fatal audio error: " << e.what() << '\n';
        (void) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Alfred Audio Error", "A critical audio error occurred. Check the log.", nullptr);
        return 1;
    }

    return 0;
}
