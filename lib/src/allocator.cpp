#include "alfred/allocator.hpp"

namespace allocator {
    static StaticAllocatorStorage g_storage;

    StaticAllocatorStorage& StaticAllocatorStorage::get() {
        return g_storage;
    }
}
