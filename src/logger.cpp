#include "logger.hpp"

#include "settings.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>


Logger& Logger::instance() {
    static Logger instance_;
    return instance_;
}

void Logger::init() {
    level_ = LogLevel::Debug;
    int level = Settings::const_instance().const_log().level;
    if (level < static_cast<int>(LogLevel::Debug) || level > static_cast<int>(LogLevel::Critical)) {
        critical(std::string("Cannot set logging level to ") + std::to_string(level));
    }
    level_ = static_cast<LogLevel>(level);
}

// TODO: customizable outputs for logger (std:cerr, logfile, etc)
void Logger::debug(const std::string& msg) {
    Logger::log(LogLevel::Debug, msg);
}

void Logger::info(const std::string& msg) {
    Logger::log(LogLevel::Info, msg);
}

void Logger::warning(const std::string& msg) {
    Logger::log(LogLevel::Warning, msg);
}

void Logger::error(const std::string& msg) {
    Logger::log(LogLevel::Error, msg);
}

void Logger::critical(const std::string& msg) {
    Logger::log(LogLevel::Critical, msg);
    exit(1);
}

void Logger::log(LogLevel level, const std::string& msg) {
    if (level < level_) {
        return;
    }

    auto it = log_level_to_str.find(level);
    if (it == log_level_to_str.end()) {
        std::cerr << "Internal Error: cannot find string representetion for level " << static_cast<int>(level) << std::endl;
        exit(1);
    }

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::string level_str = it->second;
    std::cerr << std::put_time(&tm, "[%Y.%m.%d %H:%M:%S]\t") << "[" << level_str << "]\t" << msg << std::endl;
}
