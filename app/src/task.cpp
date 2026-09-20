#include "task.hpp"

#include <SDL3/SDL.h>

namespace alfred::task {
    void TaskManager::update() {
        task_manager::TaskManager<std::uint64_t>::update(SDL_GetTicksNS());
    }

    void TaskManager::reset() {
        task_manager::TaskManager<std::uint64_t>::reset(SDL_GetTicksNS());
    }
}
