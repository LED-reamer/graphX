#pragma once
#include <iostream>
#include <string>

#include <ctime>

//#define NODEBUGCOLORS

#define LOGInfo(text) gx::debug::logInfo(text); std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
#define LOGError(text, ...) gx::debug::logInfo(text, __VA_ARGS__); std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
#define LOGtest(text, ...) gx::debug::logInfo(text, __VA_ARGS__); std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;

namespace gx
{
	namespace debug
	{

		void logInfo(std::string text);
		void logError(std::string text, bool exit = false);
		void logWarning(std::string text);
		//if 0 or false -> raise error
		void test(bool noError = false, std::string text = "An error occurred!", bool exit = false);

		static void logErrorStatic(std::string text, bool exit = false)
        {
            std::time_t currentTime = std::time(nullptr);
            std::tm timeInfo;
            localtime_s(&timeInfo, &currentTime);
            char timestamp[80];
            strftime(timestamp, sizeof(timestamp), "%a %b %d %T %Y", &timeInfo);

#ifndef NODEBUGCOLORS
            std::cout << "\033[0m[\033[32m" << timestamp << "\033[0m] [\033[31mERROR\033[0m] \033[34m" << text << "\033[0m" << std::endl;
#endif
#ifdef NODEBUGCOLORS
            std::cout << "[" << timestamp << "] [ERROR] " << text << std::endl;
#endif

            if (exit) {
                std::exit(EXIT_FAILURE);
            }
        }
	}
}