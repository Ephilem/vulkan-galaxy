//
// Created by raph on 11/11/24.
//

#include "Logger.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>


std::map<std::string, size_t> Logger::maxLengths = {
    {"LEVEL", 5},
    {"COMPONENT", 15} // updated dynamically
};


Logger::Logger(std::string component)
    : componentName(std::move(component))
    , minimumLevel(Level::DEBUG)
    , useColors(true)
    , showTimestamp(true) {
    maxLengths["COMPONENT"] = std::max(maxLengths["COMPONENT"], componentName.size());
}

void Logger::log(Level level, const std::string& message) {
    if (level < minimumLevel) return;

    std::stringstream ss;

    if (showTimestamp) {
        ss << "[" << getCurrentTime() << "] ";
    }



    std::string levelStr = levelToString(level);
    if (useColors) {
        ss << levelToColor(level);
    }
    ss << std::left << std::setw(maxLengths["LEVEL"]) << levelStr;
    if (useColors) {
        ss << Colors::RESET;
    }

    ss << " " << Colors::CYAN
       << std::left << std::setw(maxLengths["COMPONENT"]) << componentName
       << Colors::RESET;

    ss << " " << formatMessage(message);

    if (level >= Level::ERROR) {
        std::cerr << ss.str();
    } else {
        std::cout << ss.str();
    }
}

void Logger::trace(const std::string& message) { log(Level::TRACE, message); }
void Logger::debug(const std::string& message) { log(Level::DEBUG, message); }
void Logger::info(const std::string& message) { log(Level::INFO, message); }
void Logger::warning(const std::string& message) { log(Level::WARNING, message); }
void Logger::error(const std::string& message) { log(Level::ERROR, message); }
void Logger::fatal(const std::string& message) { log(Level::FATAL, message); }

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::TRACE:   return "TRACE";
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARN";
        case Level::ERROR:   return "ERROR";
        case Level::FATAL:   return "FATAL";
        default:            return "UNKNOWN";
    }
}

const char* Logger::levelToColor(Level level) {
    switch (level) {
        case Level::TRACE:   return Colors::BLUE;
        case Level::DEBUG:   return Colors::CYAN;
        case Level::INFO:    return Colors::GREEN;
        case Level::WARNING: return Colors::YELLOW;
        case Level::ERROR:   return Colors::RED;
        case Level::FATAL:   return Colors::MAGENTA;
        default:            return Colors::RESET;
    }
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::formatMessage(const std::string& message) {
    std::stringstream formatted;
    std::istringstream stream(message);
    std::string line;

    while (std::getline(stream, line)) {
        formatted << "    " << line << "\n";
    }

    return formatted.str();
}

