#pragma once

#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>
#include <fmt/color.h>
#include <fmt/core.h>

#include <string>
#include <utility>

namespace bamf {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
};

constexpr fmt::color log_level_to_colour(LogLevel level) {
    switch (level) {
    case LogLevel::Trace:
        return fmt::color::steel_blue;
    case LogLevel::Debug:
        return fmt::color::cornflower_blue;
    case LogLevel::Info:
        return fmt::color::light_cyan;
    case LogLevel::Warn:
        return fmt::color::yellow;
    case LogLevel::Error:
        return fmt::color::orange_red;
    }
}

constexpr const char *log_level_to_string(LogLevel level) {
    switch (level) {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    }
}

class Logger {
    const std::string m_name;

public:
    BAMF_MAKE_NON_COPYABLE(Logger)
    BAMF_MAKE_NON_MOVABLE(Logger)

    explicit Logger(std::string name) : m_name(std::move(name)) {}
    ~Logger() = default;

    template <typename FmtStr, typename... Args>
    void log(LogLevel level, const FmtStr &fmt, const Args &...args) const;

    template <typename FmtStr, typename... Args>
    void trace(const FmtStr &fmt, const Args &...args) const;

    template <typename FmtStr, typename... Args>
    void debug(const FmtStr &fmt, const Args &...args) const;

    template <typename FmtStr, typename... Args>
    void info(const FmtStr &fmt, const Args &...args) const;

    template <typename FmtStr, typename... Args>
    void warn(const FmtStr &fmt, const Args &...args) const;

    template <typename FmtStr, typename... Args>
    void error(const FmtStr &fmt, const Args &...args) const;
};

template <typename FmtStr, typename... Args>
void Logger::log(LogLevel level, const FmtStr &fmt, const Args &...args) const {
    auto formatted = fmt::format(fmt, args...);
    fmt::print(fmt::fg(log_level_to_colour(level)), "{:<5} [{}] {}\n", log_level_to_string(level), m_name, formatted);
}

template <typename FmtStr, typename... Args>
void Logger::trace(const FmtStr &fmt, const Args &...args) const {
    log(LogLevel::Trace, fmt, args...);
}

template <typename FmtStr, typename... Args>
void Logger::debug(const FmtStr &fmt, const Args &...args) const {
    log(LogLevel::Debug, fmt, args...);
}

template <typename FmtStr, typename... Args>
void Logger::info(const FmtStr &fmt, const Args &...args) const {
    log(LogLevel::Info, fmt, args...);
}

template <typename FmtStr, typename... Args>
void Logger::warn(const FmtStr &fmt, const Args &...args) const {
    log(LogLevel::Warn, fmt, args...);
}

template <typename FmtStr, typename... Args>
void Logger::error(const FmtStr &fmt, const Args &...args) const {
    log(LogLevel::Error, fmt, args...);
}

} // namespace bamf
