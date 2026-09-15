#pragma once

#include <task_manager.hpp>

namespace alfred::task {
    struct TaskManager : task_manager::TaskManager<unsigned long long> {
        void update();
        void reset();
    };

    using AsyncTask = task_manager::AsyncTask;
}
