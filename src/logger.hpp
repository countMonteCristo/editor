#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <string>
#include <unordered_map>


enum class LogLevel {
    Debug = 0,
    Info,
    Warning,
    Error,
    Critical,
};

const std::unordered_map<LogLevel, std::string> log_level_to_str = {
    {LogLevel::Debug,       "DEBUG"},
    {LogLevel::Info,        "INFO"},
    {LogLevel::Warning,     "WARN"},
    {LogLevel::Error,       "ERROR"},
    {LogLevel::Critical,    "CRIT"},
};


class Logger {
public:
    static Logger& instance();

    void init();

    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);
    void critical(const std::string& msg);

    void log(LogLevel level, const std::string& msg);

private:
    explicit Logger() {}

    LogLevel level_;
};

#endif // LOGGER_HPP_
