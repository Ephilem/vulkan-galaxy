//
// Created by raph on 11/11/24.
//
#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <string>
#include <sstream>
#include <mutex>


class Logger {
public:
    enum class Level {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL,
    };

    struct Colors {
        static constexpr const char* RED     = "\033[1;31m";
        static constexpr const char* YELLOW  = "\033[1;33m";
        static constexpr const char* GREEN   = "\033[1;32m";
        static constexpr const char* BLUE    = "\033[1;34m";
        static constexpr const char* MAGENTA = "\033[1;35m";
        static constexpr const char* CYAN    = "\033[1;36m";
        static constexpr const char* GRAY    = "\033[1;90m";
        static constexpr const char* RESET   = "\033[0m";

        static constexpr const char* BG_RED     = "\033[41m";
        static constexpr const char* BG_YELLOW  = "\033[43m";
        static constexpr const char* BG_BLUE    = "\033[44m";
    };

    explicit Logger(std::string component);


    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

    void setLogLevel(Level level) { minimumLevel = level; }
    void setUseColors(bool use) { useColors = use; }
    void setShowTimestamp(bool show) { showTimestamp = show; }

    const std::string& getComponent() const { return componentName; }

private:
    void log(Level level, const std::string& message);

    static std::map<std::string, size_t> maxLengths;
    static std::string levelToString(Level level);
    static const char* levelToColor(Level level);
    static std::string getCurrentTime();
    static std::string formatMessage(const std::string& message);
    const std::string componentName;

    Level minimumLevel;
    bool useColors;
    bool showTimestamp;
};

#endif //LOGGER_H
