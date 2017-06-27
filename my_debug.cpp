#include "my_debug.h"
#include "cocos2d.h"

void my_debug::debug_msg(std::string file, int line, std::string message,bool boxtype) {
	std::stringstream name;
	name << "[" << line << "]" << " " << message;
	std::string title = name.str();
	CCLOG("");
	CCLOG("%s", file.c_str());
	CCLOG("%s", title.c_str());
}
