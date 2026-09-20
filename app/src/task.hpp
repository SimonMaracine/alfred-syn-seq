#pragma once

#include <cstdint>

#include <task_manager.hpp>

namespace alfred::task {
    struct TaskManager : task_manager::TaskManager<std::uint64_t> {
        void update();
        void reset();
    };

    using AsyncTask = task_manager::AsyncTask;
}
