#include "debug.h"

#include <iostream>

#include "graphX/internal/context.h"


void gx::debug::logInfo(std::string text)
{
    if (ctx.debugMode == false) return;

    std::time_t currentTime = std::time(nullptr);
    std::tm timeInfo;
    localtime_s(&timeInfo, &currentTime);
    char timestamp[80];
    strftime(timestamp, sizeof(timestamp), "%a %b %d %T %Y", &timeInfo);

    #ifndef NODEBUGCOLORS
    std::cout << "\033[0m[\033[32m" << timestamp << "\033[0m] [\033[33mINFO\033[0m] \033[34m" << text << "\033[0m" << std::endl;
    #endif
    #ifdef NODEBUGCOLORS
    std::cout << "[" << timestamp << "] [INFO] " << text << std::endl;
    #endif
}

void gx::debug::logError(std::string text, bool exit)
{
    if (ctx.debugMode == false) return;

    std::time_t currentTime = std::time(nullptr);
    std::tm timeInfo;
    localtime_s(&timeInfo, &currentTime);
    char timestamp[80];
    strftime(timestamp, sizeof(timestamp), "%a %b %d %T %Y", &timeInfo);

    #ifndef NODEBUGCOLORS
    std::cout << "\033[0m[\033[32m" << timestamp << "\033[0m] [\033[31mERROR\033[0m] \033[31m" << text << "\033[0m" << std::endl;
    #endif
    #ifdef NODEBUGCOLORS
    std::cout << "[" << timestamp << "] [ERROR] " << text << std::endl;
    #endif

    if (exit) {
        std::exit(EXIT_FAILURE);
    }
}

void gx::debug::logWarning(std::string text)
{
    if (ctx.debugMode == false) return;

    std::time_t currentTime = std::time(nullptr);
    std::tm timeInfo;
    localtime_s(&timeInfo, &currentTime);
    char timestamp[80];
    strftime(timestamp, sizeof(timestamp), "%a %b %d %T %Y", &timeInfo);

#ifndef NODEBUGCOLORS
    std::cout << "\033[0m[\033[32m" << timestamp << "\033[0m] [\033[95mWARNING\033[0m] \033[95m" << text << "\033[0m" << std::endl;
#endif
#ifdef NODEBUGCOLORS
    std::cout << "[" << timestamp << "] [WARNING] " << text << std::endl;
#endif
}

void gx::debug::test(bool noError, std::string text, bool exit)
{
    if (ctx.debugMode == false) return;

    if (!noError)
    {
        logError(text, exit);
    }
}