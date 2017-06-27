#pragma once
#include <string>

class my_debug {
public:
	static void debug_msg(
		std::string file, int line, std::string message,bool boxtype = false);
};
