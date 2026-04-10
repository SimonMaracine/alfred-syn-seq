#pragma once

#include <functional>
#include <thread>
#include <exception>
#include <utility>
#include <vector>
#include <forward_list>
#include <mutex>

namespace alfred::task {
    class AsyncTask;
    class TaskManager;

    using ImmediateTaskFunction = std::function<void()>;
    using ImmediateThreadSafeTaskFunction = std::function<void()>;
    using RepeatableTaskFunction = std::function<bool()>;
    using DelayedTaskFunction = std::function<void()>;
    using ContinuousTaskFunction = std::function<bool()>;
    using AsyncTaskFunction = std::move_only_function<void(AsyncTask&)>;

    class ImmediateTask {
    public:
        explicit ImmediateTask(ImmediateTaskFunction task_function)
            : m_task_function(std::move(task_function)) {}
    private:
        ImmediateTaskFunction m_task_function;

        friend class TaskManager;
    };

    class ImmediateThreadSafeTask {
    public:
        explicit ImmediateThreadSafeTask(ImmediateThreadSafeTaskFunction task_function)
            : m_task_function(std::move(task_function)) {}
    private:
        ImmediateThreadSafeTaskFunction m_task_function;

        friend class TaskManager;
    };

    class RepeatableTask {
    public:
        RepeatableTask(RepeatableTaskFunction task_function, unsigned long long interval)
            : m_task_function(std::move(task_function)), m_interval(interval) {}
    private:
        RepeatableTaskFunction m_task_function;

        unsigned long long m_interval {};
        unsigned long long m_last_time {};
        unsigned long long m_total_time {};

        friend class TaskManager;
    };

    class DelayedTask {
    public:
        DelayedTask(DelayedTaskFunction task_function, unsigned long long delay)
            : m_task_function(std::move(task_function)), m_delay(delay) {}
    private:
        DelayedTaskFunction m_task_function;

        unsigned long long m_delay {};
        unsigned long long m_last_time {};
        unsigned long long m_total_time {};

        friend class TaskManager;
    };

    class ContinuousTask {
    public:
        ContinuousTask(ContinuousTaskFunction task_function, unsigned long long interval)
            : m_task_function(std::move(task_function)), m_interval(interval) {}
    private:
        ContinuousTaskFunction m_task_function;

        unsigned long long m_interval {};
        unsigned long long m_last_time {};
        unsigned long long m_total_time {};

        friend class TaskManager;
    };

    class AsyncTask {
    public:
        explicit AsyncTask(AsyncTaskFunction task_function)
            : m_thread(std::move(task_function), std::ref(*this)) {}

        void finish();
        void finish(std::exception_ptr exception);
        bool stop_requested() const;
    private:
        std::exception_ptr m_exception;
        std::jthread m_thread;

        friend class TaskManager;
    };

    class TaskManager {
    public:
        void add_immediate_task(ImmediateTaskFunction task_function);
        void add_immediate_thread_safe_task(ImmediateThreadSafeTaskFunction task_function);
        void add_repeatable_task(RepeatableTaskFunction task_function, unsigned long long interval);
        void add_delayed_task(DelayedTaskFunction task_function, unsigned long long delay);
        void add_continuous_task(ContinuousTaskFunction task_function, unsigned long long interval);
        void add_async_task(AsyncTaskFunction task_function);

        void clear();
        void update();
    private:
        void update_immediate_tasks();
        void update_immediate_thread_safe_tasks();
        void update_repeatable_tasks();
        void update_delayed_tasks();
        void update_continuous_tasks();
        void update_async_tasks();

        static void update_immediate_task(ImmediateTask& task);
        static void update_immediate_thread_safe_task(ImmediateThreadSafeTask& task);
        static bool update_repeatable_task(RepeatableTask& task);
        static bool update_delayed_task(DelayedTask& task);
        static bool update_continuous_task(ContinuousTask& task);

        std::vector<ImmediateTask> m_immediate_tasks_incoming;
        std::vector<ImmediateTask> m_immediate_tasks_active;
        std::vector<ImmediateThreadSafeTask> m_immediate_thread_safe_tasks_incoming;
        std::vector<ImmediateThreadSafeTask> m_immediate_thread_safe_tasks_active;
        std::vector<RepeatableTask> m_repeatable_tasks_incoming;
        std::vector<RepeatableTask> m_repeatable_tasks_active;
        std::vector<DelayedTask> m_delayed_tasks_incoming;
        std::vector<DelayedTask> m_delayed_tasks_active;
        std::vector<ContinuousTask> m_continuous_tasks_incoming;
        std::vector<ContinuousTask> m_continuous_tasks_active;
        std::forward_list<AsyncTask> m_async_tasks;
        std::mutex m_mutex;
    };
}
