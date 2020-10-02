#include "Constants.h"

void full_crash_err(const char* text, const char* file, int line)
{
	std::cout << "FATAL CRASH" << std::endl;
	std::cout << text << "in line:" << line << ", in file:" << file << std::endl;
	abort();
}
