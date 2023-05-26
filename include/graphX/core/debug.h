#pragma once
#include <iostream>
#include <string>

//#define NODEBUGCOLORS

#define LOGInfo(text) logInfo(text); std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
#define LOGError(text, ...) logInfo(text, __VA_ARGS__); std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
#define LOGtest(text, ...) logInfo(text, __VA_ARGS__); std::cout << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;

namespace gx
{
	namespace debug
	{

		void logInfo(std::string text);
		void logError(std::string text, bool exit = false);
		//if 0 or false -> raise error
		void test(bool noError = false, std::string text = "An error occurred!", bool exit = false);
	}
}