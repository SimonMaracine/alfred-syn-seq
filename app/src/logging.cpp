#include "logging.hpp"

#include <print>
#include <fstream>
#include <ostream>
#include <mutex>

#include "utility.hpp"

namespace logging {
    static std::ofstream g_stream;
    static std::mutex g_stream_mutex;

    void initialize() {
        std::lock_guard guard {g_stream_mutex};

        g_stream.open(utility::data_file_path() / "alfred.log", std::ios_base::app);

        if (!g_stream.is_open()) {
            throw LoggingError("Could not open log file");
        }
    }

    void uninitialize() {
        std::lock_guard guard {g_stream_mutex};

        g_stream.close();
    }

    void console_println(Severity severity, const std::source_location& location, TimeOfDay time_of_day, const std::string& message) {
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
    }

    void file_println(Severity severity, const std::source_location& location, TimeOfDay time_of_day, const std::string& message) {
        std::lock_guard guard {g_stream_mutex};

        if (!g_stream.is_open()) {
            return;
        }

        std::println(
            g_stream,
            "[{} {} {} {} {}:{}] {}",
            severity_to_string(severity),
            time_of_day,
            location.file_name(),
            location.function_name(),
            location.line(),
            location.column(),
            message
        );
    }
}
