#pragma once

#include <span>
#include <stdexcept>

#include <SDL3/SDL.h>

namespace image {
    class SurfaceRef {
    public:
        SurfaceRef() = default;
        SurfaceRef(SDL_Surface* surface);

        SDL_Surface* get() const { return m_surface; }

        void add_alternate(SurfaceRef surface) const;
    protected:
        SDL_Surface* m_surface {};
    };

    class Surface : public SurfaceRef {
    public:
        Surface() = default;
        explicit Surface(std::span<const unsigned char> buffer);
        ~Surface();

        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;
        Surface(Surface&& other);
        Surface& operator=(Surface&& other);
    };

    class TextureRef {
    public:
        TextureRef() = default;
        TextureRef(SDL_Texture* texture);

        SDL_Texture* get() const { return m_texture; }
    protected:
        SDL_Texture* m_texture {};
    };

    class Texture : public TextureRef {
    public:
        Texture() = default;
        Texture(SDL_Renderer* renderer, SurfaceRef surface);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);
    };

    struct ImageError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}
