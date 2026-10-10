#include "TCISDK/sdk/logger.h"
#include <TCISDK/sdk.h>
#include <TCISDK/internal.h>

TCISDK::Logger g_logger = TCISDK::Logger(TCISDK::Logger::Level::DEBUG, "TCISimpleExtension");

int Broadcast(const std::string& text) {
    TCISDK::Message msg(text);
    msg.Broadcast();
    return 0;
}

int Log(const std::string& text) {
    TCISDK::Message msg(text);
    msg.Log();
    return 0;
}

TCISDK_MAIN() {
    TCISDK::Extension::Register("broadcast", Broadcast);
    TCISDK::Extension::Register("log", Log);

    return 0;
}