#pragma once

#include <print>
#include <source_location>
#include <utility>
#include <chrono>

namespace logging {
    enum class Severity {
        Debug,
        Information,
        Warning,
        Error,
        Critical
    };

    constexpr const char* severity_to_string(Severity severity) {
        switch (severity) {
            case Severity::Debug:
                return "Debug";
            case Severity::Information:
                return "Information";
            case Severity::Warning:
                return "Error";
            case Severity::Error:
                return "Error";
            case Severity::Critical:
                return "Critical";
        }

        std::unreachable();
    }

    template<Severity severity, typename... Args>
    void log(const std::source_location& location, std::format_string<Args...> fmt, Args&&... args) {
        namespace chrono = std::chrono;

        const auto time {chrono::system_clock::now()};
        const chrono::hh_mm_ss time_of_day {chrono::floor<chrono::seconds>(time - chrono::floor<chrono::days>(time))};
        const auto message {std::format(fmt, std::forward<Args>(args)...)};

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

    template<typename... Args>
    struct debug {
        debug(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log<Severity::Debug>(location, fmt, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    debug(std::format_string<Args...> fmt, Args&&... args) -> debug<Args...>;

    template<typename... Args>
    struct information {
        information(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log<Severity::Information>(location, fmt, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    information(std::format_string<Args...> fmt, Args&&... args) -> information<Args...>;

    template<typename... Args>
    struct warning {
        warning(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log<Severity::Warning>(location, fmt, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    warning(std::format_string<Args...> fmt, Args&&... args) -> warning<Args...>;

    template<typename... Args>
    struct error {
        error(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log<Severity::Error>(location, fmt, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    error(std::format_string<Args...> fmt, Args&&... args) -> error<Args...>;

    template<typename... Args>
    struct critical {
        critical(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log<Severity::Critical>(location, fmt, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    critical(std::format_string<Args...> fmt, Args&&... args) -> critical<Args...>;
}

#ifdef ALFRED_DISTRIBUTION
    #define LOG_DEBUG(...) static_cast<void>(0)
    #define LOG_INFORMATION(...) static_cast<void>(0)
    #define LOG_WARNING(...) static_cast<void>(0)
    #define LOG_ERROR(...) static_cast<void>(0)
    #define LOG_CRITICAL(...) static_cast<void>(0)
#else
    #define LOG_DEBUG(...) logging::debug(__VA_ARGS__)
    #define LOG_INFORMATION(...) logging::information(__VA_ARGS__)
    #define LOG_WARNING(...) logging::warning(__VA_ARGS__)
    #define LOG_ERROR(...) logging::error(__VA_ARGS__)
    #define LOG_CRITICAL(...) logging::critical(__VA_ARGS__)
#endif
