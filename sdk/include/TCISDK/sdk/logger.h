#pragma once

#include <TCISDK/sdk/message.h>

namespace TCISDK {

class Logger {
public:
    enum class Level { DEBUG, TRACE, INFO, WARNING, ERROR };

private:
    std::string m_extName;
    Level m_minLevel = Level::INFO;

    void Log(Level level, std::string text) {
        if (level < m_minLevel) return;

        Message msg;
        msg << "[TCISDK : " << m_extName << "] " << to_string(level) << ": " << text;
        msg.Log();
    }

    std::string_view to_string(Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::TRACE:   return "TRACE";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR:   return "ERROR";
        }
        return "UNKNOWN";
    }

public:
    Logger(Level minLevel, std::string extName) : m_extName(extName), m_minLevel(minLevel) {}
    Logger(std::string extName) : m_extName(extName) {}

    void SetMinLogLevel(Level level) {
        m_minLevel = level;
    }

    void Debug(std::string text) {
        Log(Level::DEBUG, text);
    }

    void Trace(std::string text) {
        Log(Level::TRACE, text);
    }

    void Info(std::string text) {
        Log(Level::INFO, text);
    }

    void Warning(std::string text) {
        Log(Level::WARNING, text);
    }

    void Error(std::string text) {
        Log(Level::ERROR, text);
    }
};

} // namespace TCISDK