#pragma once

#include <source_location>
#include <utility>
#include <chrono>

// Global console and file logging

namespace alfred::log {
    inline constexpr const char* FILE = "alfred.log";

    enum class Level {
        Debug,
        Information,
        Warning,
        Error,
        Critical
    };

    constexpr const char* to_string(Level level) {
        switch (level) {
            case Level::Debug:
                return "Debug";
            case Level::Information:
                return "Information";
            case Level::Warning:
                return "Warning";
            case Level::Error:
                return "Error";
            case Level::Critical:
                return "Critical";
        }

        std::unreachable();
    }

    bool initialize();
    void uninitialize();

    namespace chrono = std::chrono;
    using TimeOfDay = chrono::hh_mm_ss<chrono::seconds>;

    void log(Level level, const std::source_location& location, const std::string& message);

    template<typename... Args>
    struct debug {
        explicit debug(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log(Level::Debug, location, std::format(fmt, std::forward<Args>(args)...));
        }
    };

    template<typename... Args>
    debug(std::format_string<Args...> fmt, Args&&... args) -> debug<Args...>;

    template<typename... Args>
    struct information {
        explicit information(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log(Level::Information, location, std::format(fmt, std::forward<Args>(args)...));
        }
    };

    template<typename... Args>
    information(std::format_string<Args...> fmt, Args&&... args) -> information<Args...>;

    template<typename... Args>
    struct warning {
        explicit warning(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log(Level::Warning, location, std::format(fmt, std::forward<Args>(args)...));
        }
    };

    template<typename... Args>
    warning(std::format_string<Args...> fmt, Args&&... args) -> warning<Args...>;

    template<typename... Args>
    struct error {
        explicit error(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log(Level::Error, location, std::format(fmt, std::forward<Args>(args)...));
        }
    };

    template<typename... Args>
    error(std::format_string<Args...> fmt, Args&&... args) -> error<Args...>;

    template<typename... Args>
    struct critical {
        explicit critical(std::format_string<Args...> fmt, Args&&... args, const std::source_location& location = std::source_location::current()) {
            log(Level::Critical, location, std::format(fmt, std::forward<Args>(args)...));
        }
    };

    template<typename... Args>
    critical(std::format_string<Args...> fmt, Args&&... args) -> critical<Args...>;
}

#ifdef ALFRED_DISTRIBUTION
    #define LOG_DEBUG(...) (void) 0
    #define LOG_INFORMATION(...) (void) 0
    #define LOG_WARNING(...) (void) 0
    #define LOG_ERROR(...) (void) 0
    #define LOG_CRITICAL(...) (void) 0
#else
    #define LOG_DEBUG(...) log::debug(__VA_ARGS__)
    #define LOG_INFORMATION(...) log::information(__VA_ARGS__)
    #define LOG_WARNING(...) log::warning(__VA_ARGS__)
    #define LOG_ERROR(...) log::error(__VA_ARGS__)
    #define LOG_CRITICAL(...) log::critical(__VA_ARGS__)
#endif
