#include "logging.hpp"

#include <fstream>
#include <ostream>
#include <mutex>

#ifdef __cpp_lib_print
    #include <print>
#else
    #include <iostream>
    #include <format>
#endif

#include "utility.hpp"

namespace alfred::logging {
    static struct {
        std::ofstream stream;
        std::mutex mutex;
    } g_stream;

    void initialize() {
        std::lock_guard guard {g_stream.mutex};

        g_stream.stream.open(utility::data_file_path() / "alfred.log", std::ios_base::app);

        if (!g_stream.stream.is_open()) {
            throw LoggingError("Could not open log file");
        }
    }

    void uninitialize() {
        std::lock_guard guard {g_stream.mutex};

        g_stream.stream.close();
    }

    void println_console([[maybe_unused]] Severity severity, [[maybe_unused]] const std::source_location& location, [[maybe_unused]] TimeOfDay time_of_day, [[maybe_unused]] const std::string& message) {
        // Printing to console is already thread safe

        // Distribution build for Windows doesn't have a console, so there is no printing available
        // Disable printing for Linux too
#ifndef ALFRED_DISTRIBUTION
    #ifdef __cpp_lib_print
        std::println(
            stderr,
            "[{} {} {} {} {}:{}] {}",
            severity_to_string(severity),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
    #else
        std::cerr << std::format(
            "[{} {} {} {} {}:{}] {}\n",
            severity_to_string(severity),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
    #endif
#endif
    }

    void println_file(Severity severity, [[maybe_unused]] const std::source_location& location, TimeOfDay time_of_day, const std::string& message) {
        std::lock_guard guard {g_stream.mutex};

        if (!g_stream.stream.is_open()) {
            return;
        }

#ifdef __cpp_lib_print
    #ifdef ALFRED_DISTRIBUTION
        std::println(
            g_stream.stream,
            "[{} {}] {}",
            severity_to_string(severity),
            time_of_day,
            message
        );
    #else
        std::println(
            g_stream.stream,
            "[{} {} {} {} {}:{}] {}",
            severity_to_string(severity),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
    #endif
#else
    #ifdef ALFRED_DISTRIBUTION
        g_stream.stream << std::format(
            "[{} {}] {}\n",
            severity_to_string(severity),
            time_of_day,
            message
        );
    #else
        g_stream.stream << std::format(
            "[{} {} {} {} {}:{}] {}\n",
            severity_to_string(severity),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
    #endif
#endif
    }
}
