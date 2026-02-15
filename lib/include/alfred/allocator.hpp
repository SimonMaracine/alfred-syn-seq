#pragma once

#include <memory>
#include <algorithm>
#include <utility>
#include <cstddef>

// Global static stateless allocator

namespace allocator {
    template<std::size_t StorageSize, std::size_t ObjectSize, std::size_t ObjectAlignment>
    struct StaticAllocatorStorage {
        static constexpr auto STORAGE_SIZE {StorageSize};
        static constexpr auto OBJECT_SIZE {ObjectSize};
        static constexpr auto OBJECT_ALIGNMENT {ObjectAlignment};

        alignas(OBJECT_ALIGNMENT) unsigned char m_base[STORAGE_SIZE * OBJECT_SIZE] {};
        bool m_objects[STORAGE_SIZE] {};
        std::size_t m_pointer {};

        static StaticAllocatorStorage& get() {
            static StaticAllocatorStorage instance;
            return instance;
        }
    };

    template<typename T, typename Storage>
    struct StaticAllocator {
        using value_type = T;
        using size_type = std::size_t;

        static_assert(sizeof(T) <= Storage::OBJECT_SIZE);
        static_assert(alignof(T) <= Storage::OBJECT_ALIGNMENT);

        value_type* allocate(size_type n) {
            auto& storage {Storage::get()};

            const auto try_allocate {
                [&](size_type i) {
                    if (std::all_of(storage.m_objects + i, storage.m_objects + i + n, [](const bool& object) { return !object; })) {
                        std::for_each(storage.m_objects + i, storage.m_objects + i + n, [](bool& object) { object = true; });
                        storage.m_pointer = i + 1;

                        return true;
                    }

                    return false;
                }
            };

            for (size_type i {storage.m_pointer}; i < storage.STORAGE_SIZE - n + 1; i++) {
                if (try_allocate(i)) {
                    return reinterpret_cast<value_type*>(storage.m_base) + i;
                }
            }

            for (size_type i {}; i < storage.m_pointer - n + 1; i++) {
                if (try_allocate(i)) {
                    return reinterpret_cast<value_type*>(storage.m_base) + i;
                }
            }

            std::unreachable();
        }

        void deallocate(value_type* p, size_type n) {
            auto& storage {Storage::get()};

            const size_type object_pointer {reinterpret_cast<size_type>(p) - reinterpret_cast<size_type>(storage.m_base)};
            const size_type index {object_pointer / sizeof(value_type)};

            std::for_each(storage.m_objects + index, storage.m_objects + index + n, [](bool& object) { object = false; });
        }
    };

    template<typename T, typename U, typename Storage>
    bool operator==(const StaticAllocator<T, Storage>&, const StaticAllocator<U, Storage>&) { return true; }

    template<typename T, typename U, typename Storage>
    bool operator!=(const StaticAllocator<T, Storage>&, const StaticAllocator<U, Storage>&) { return false; }

    template<typename T, typename Storage>
    struct StaticAllocated {
        void* operator new(std::size_t) {
            StaticAllocator<T, Storage> alloc;
            using Alloc = std::allocator_traits<decltype(alloc)>;

            return Alloc::allocate(alloc, 1);
        }

        void operator delete(void* ptr) {
            StaticAllocator<T, Storage> alloc;
            using Alloc = std::allocator_traits<decltype(alloc)>;

            Alloc::deallocate(alloc, static_cast<T*>(ptr), 1);
        }
    };
}
