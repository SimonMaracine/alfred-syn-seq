#include "image.hpp"

#include <format>
#include <utility>

#include <SDL3/SDL.h>

namespace image {
    SurfaceRef::SurfaceRef(SDL_Surface* surface)
        : m_surface(surface) {}

    void SurfaceRef::add_alternate(SurfaceRef surface) const {
        if (!SDL_AddSurfaceAlternateImage(m_surface, surface.get())) {
            throw ImageError(std::format("SDL_AddSurfaceAlternateImage: {}", SDL_GetError()));
        }
    }

    Surface::Surface(std::span<const unsigned char> buffer) {
        SDL_IOStream* stream = SDL_IOFromConstMem(buffer.data(), buffer.size());

        if (!stream) {
            throw ImageError(std::format("SDL_IOFromConstMem: {}", SDL_GetError()));
        }

        m_surface = SDL_LoadPNG_IO(stream, true);

        if (!m_surface) {
            throw ImageError(std::format("SDL_LoadPNG_IO: {}", SDL_GetError()));
        }
    }

    Surface::~Surface() {
        SDL_DestroySurface(m_surface);
    }

    Surface::Surface(Surface&& other) noexcept
        : SurfaceRef(std::exchange(other.m_surface, nullptr)) {}

    Surface& Surface::operator=(Surface&& other) noexcept {
        SDL_DestroySurface(m_surface);
        m_surface = std::exchange(other.m_surface, nullptr);
        return *this;
    }

    TextureRef::TextureRef(SDL_Texture* texture)
        : m_texture(texture) {
    }

    Texture::Texture(SDL_Renderer* renderer, SurfaceRef surface) {
        m_texture = SDL_CreateTextureFromSurface(renderer, surface.get());

        if (!m_texture) {
            throw ImageError(std::format("SDL_CreateTextureFromSurface: {}", SDL_GetError()));
        }
    }

    Texture::~Texture() {
        if (m_texture) {
            SDL_DestroyTexture(m_texture);
        }
    }

    Texture::Texture(Texture&& other) noexcept
        : TextureRef(std::exchange(other.m_texture, nullptr)) {
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        SDL_DestroyTexture(m_texture);
        m_texture = std::exchange(other.m_texture, nullptr);
        return *this;
    }
}
