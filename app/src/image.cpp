#include "image.hpp"

#include <format>
#include <utility>
#include <cstring>

namespace image {
    SurfaceRef::SurfaceRef(SDL_Surface* surface)
        : m_surface(surface) {}

    Surface::Surface(const std::string& buffer) {
        SDL_IOStream* stream {SDL_IOFromConstMem(buffer.data(), buffer.size())};

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

    Surface::Surface(Surface&& other)
        : SurfaceRef(std::exchange(other.m_surface, nullptr)) {}

    Surface& Surface::operator=(Surface&& other) {
        SDL_DestroySurface(m_surface);
        m_surface = std::exchange(other.m_surface, nullptr);
        return *this;
    }

    TextureRef::TextureRef(SDL_Renderer* renderer, SDL_Texture* texture)
        : m_texture(texture) {
        const SDL_PropertiesID properties {SDL_GetTextureProperties(m_texture)};

        if (!properties) {
            throw ImageError(std::format("SDL_GetTextureProperties: {}", SDL_GetError()));
        }

        const char* name {SDL_GetRendererName(renderer)};

        if (!name) {
            throw ImageError(std::format("SDL_GetRendererName: {}", SDL_GetError()));
        }

        if (std::strcmp(name, "opengl") == 0) {
            const Sint64 number {SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_OPENGL_TEXTURE_NUMBER, -1)};

            if (number < 0) {
                throw ImageError(std::format("SDL_GetNumberProperty: {}", SDL_GetError()));
            }

            m_identifier = std::uint64_t(number);
        } if (std::strcmp(name, "opengles2") == 0) {
            const Sint64 number {SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_OPENGLES2_TEXTURE_NUMBER, -1)};

            if (number < 0) {
                throw ImageError(std::format("SDL_GetNumberProperty: {}", SDL_GetError()));
            }

            m_identifier = std::uint64_t(number);
        } else if (std::strcmp(name, "vulkan") == 0) {
            const Sint64 number {SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_VULKAN_TEXTURE_NUMBER, -1)};

            if (number < 0) {
                throw ImageError(std::format("SDL_GetNumberProperty: {}", SDL_GetError()));
            }

            m_identifier = std::uint64_t(number);
        } else if (std::strcmp(name, "gpu") == 0) {
            const void* pointer {SDL_GetPointerProperty(properties, SDL_PROP_TEXTURE_GPU_TEXTURE_POINTER, nullptr)};

            if (!pointer) {
                throw ImageError(std::format("SDL_GetPointerProperty: {}", SDL_GetError()));
            }

            m_identifier = reinterpret_cast<std::uint64_t>(pointer);
        } else if (std::strcmp(name, "direct3d12") == 0) {
            const void* pointer {SDL_GetPointerProperty(properties, SDL_PROP_TEXTURE_D3D12_TEXTURE_POINTER, nullptr)};

            if (!pointer) {
                throw ImageError(std::format("SDL_GetPointerProperty: {}", SDL_GetError()));
            }

            m_identifier = reinterpret_cast<std::uint64_t>(pointer);
        } else if (std::strcmp(name, "direct3d11") == 0) {
            const void* pointer {SDL_GetPointerProperty(properties, SDL_PROP_TEXTURE_D3D11_TEXTURE_POINTER, nullptr)};

            if (!pointer) {
                throw ImageError(std::format("SDL_GetPointerProperty: {}", SDL_GetError()));
            }

            m_identifier = reinterpret_cast<std::uint64_t>(pointer);
        }
    }

    Texture::Texture(SDL_Renderer* renderer, SurfaceRef surface) {
        m_texture = SDL_CreateTextureFromSurface(renderer, surface.get());

        if (!m_texture) {
            throw ImageError(std::format("SDL_CreateTextureFromSurface: {}", SDL_GetError()));
        }
    }

    Texture::~Texture() {
        SDL_DestroyTexture(m_texture);
    }

    Texture::Texture(Texture&& other)
        : TextureRef(nullptr, std::exchange(other.m_texture, nullptr)) {
        m_identifier = std::exchange(other.m_identifier, 0);
    }

    Texture& Texture::operator=(Texture&& other) {
        SDL_DestroyTexture(m_texture);
        m_texture = std::exchange(other.m_texture, nullptr);
        m_identifier = std::exchange(other.m_identifier, 0);
        return *this;
    }
}
