#pragma once

#include <string>
#include <cstdint>
#include <cstddef>

// Fowler-Noll-Vo algorithm, FNV-1a more exactly

namespace hash {
    namespace internal {
        struct Variant32 {
            using Type = std::uint32_t;
            static constexpr std::uint32_t FNV_OFFSET_BASIS = 2166136261u;
            static constexpr std::uint32_t FNV_PRIME = 16777619u;
        };

        struct Variant64 {
            using Type = std::uint64_t;
            static constexpr std::uint64_t FNV_OFFSET_BASIS = 14695981039346656037u;
            static constexpr std::uint64_t FNV_PRIME = 1099511628211u;
        };

        template<typename T>
        class HashedStr {
        public:
            using Type = T::Type;

            constexpr HashedStr() noexcept
                : hash(0) {}

            explicit consteval HashedStr(const char* const string) noexcept
                : hash(fnv1a(string)) {}

            explicit HashedStr(const std::string& string) noexcept
                : hash(fnv1a(string.c_str())) {}

            constexpr operator Type() const noexcept { return hash; }
            constexpr bool operator==(const HashedStr other) const noexcept { return hash == other.hash; }
        private:
            static constexpr Type fnv1a(const char* const string) noexcept {
                Type hash = T::FNV_OFFSET_BASIS;

                for (std::size_t i {}; string[i] != '\0'; i++) {
                    hash ^= Type(string[i]);
                    hash *= T::FNV_PRIME;
                }

                return hash;
            }

            Type hash;
        };
    }

    using HashedStr32 = internal::HashedStr<internal::Variant32>;
    using HashedStr64 = internal::HashedStr<internal::Variant64>;

    namespace literals {
        consteval HashedStr32 operator""_h(const char* const string, std::size_t) noexcept {
            return HashedStr32(string);
        }

        consteval HashedStr64 operator""_H(const char* const string, std::size_t) noexcept {
            return HashedStr64(string);
        }
    }

    template<typename T>
    struct StrHash {
        constexpr std::size_t operator()(const T hashed_string) const noexcept {
            // Standard library's hash function is already perfect
            return std::hash<typename T::Type>()(hashed_string);
        }
    };
}
