#include "logging.hpp"

#include <print>
#include <fstream>
#include <ostream>
#include <mutex>

#include "utility.hpp"

namespace logging {
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

    void println_console(Severity severity, const std::source_location& location, TimeOfDay time_of_day, const std::string& message) {
        // This is already thread safe

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

    void println_file(Severity severity, const std::source_location& location, TimeOfDay time_of_day, const std::string& message) {
        std::lock_guard guard {g_stream.mutex};

        if (!g_stream.stream.is_open()) {
            return;
        }

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
    }
}
