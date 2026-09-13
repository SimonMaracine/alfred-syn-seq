#include "log.hpp"

#include <fstream>
#include <ostream>
#include <mutex>
#include <print>

#include "utility.hpp"

namespace alfred::log {
    static struct {
        std::ofstream stream;
        std::mutex mutex;
    } g_log;

    bool initialize() {
        std::lock_guard guard {g_log.mutex};

        g_log.stream.open(utility::data_file_path() / FILE, std::ios_base::app);

        if (!g_log.stream.is_open()) {
            return false;
        }

        return true;
    }

    void uninitialize() {
        std::lock_guard guard {g_log.mutex};

        g_log.stream.close();
    }

    void log(Level level, const std::source_location& location, const std::string& message) {
        const auto time = chrono::system_clock::now();
        const auto time_of_day = TimeOfDay(chrono::floor<chrono::seconds>(time - chrono::floor<chrono::days>(time)));

        std::lock_guard guard {g_log.mutex};

        // Distribution build for Windows doesn't have a console, so there is no printing available
        // Disable printing for Linux too
#ifndef ALFRED_DISTRIBUTION
        std::println(
            stderr,
            "[{} {} {} {} {}:{}] {}",
            to_string(level),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
#endif

        if (!g_log.stream.is_open()) {
            return;
        }

#ifdef ALFRED_DISTRIBUTION
        std::println(
            g_log.stream,
            "[{} {}] {}",
            to_string(level),
            time_of_day,
            message
        );
#else
        std::println(
            g_log.stream,
            "[{} {} {} {} {}:{}] {}",
            to_string(level),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
#endif
    }
}
