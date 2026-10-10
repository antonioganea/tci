#pragma once

#include <TCISDK/internal.h>
#include <string>
#include <sstream>

using namespace TCISDK_Internal;

namespace TCISDK {

inline void Broadcast(std::string text) {
    BroadcastMessage(text.c_str());
}

inline void Log(std::string text) {
    ConsoleMessage(text.c_str());
}

class Message {
private:
    std::string m_text;

public:
    inline Message(const std::string& msg) : m_text(msg) {}
    inline Message() : m_text("") {}

    template<typename T>
    Message& operator<<(const T& value) {
        std::stringstream ss;
        ss << value;
        m_text += ss.str();
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Message& msg) {
        return os << msg.m_text;
    }

    inline void Broadcast() noexcept {
        BroadcastMessage(m_text.c_str());
    }

    inline void Log() noexcept {
        ConsoleMessage(m_text.c_str());
    }
};

} // namespace TCISDK