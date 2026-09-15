#include "task.hpp"

#include <SDL3/SDL.h>

namespace alfred::task {
    void TaskManager::update() {
        task_manager::TaskManager<unsigned long long>::update(SDL_GetTicksNS());
    }

    void TaskManager::reset() {
        task_manager::TaskManager<unsigned long long>::reset(SDL_GetTicksNS());
    }
}
