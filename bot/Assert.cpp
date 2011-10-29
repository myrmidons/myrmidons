#include "Assert.hpp"
#include "State.hpp"
#include "Logger.hpp"
#include <sstream>

#ifdef THIS_IS_MAC
#	include <execinfo.h>
#endif

#ifdef DEBUG
#	include <QDebug>
#endif

#ifdef THIS_IS_MAC
std::string stackTrace() {
	std::stringstream ss;

	void* callstack[512];
	int i, frames = backtrace(callstack, 512);
	char** strs = backtrace_symbols(callstack, frames);
	for (i = 0; i < frames; ++i)
		ss << strs[i] << std::endl;
	free(strs);

	return ss.str();
}
#endif

void AssertionFailed(const char* expr,
					 const char* function, const char* file, long line)
{
	std::stringstream ss;
	ss << "Assertion \"" << expr << "\" failed in \"" << function << "\", in " << file << ":" << line;

#ifdef THIS_IS_MAC
	ss << std::endl << "-----------------" << std::endl;
	ss << stackTrace();
	ss << std::endl << "-----------------";
#endif

	std::string msg = ss.str();

	LOG_ERROR( msg );

#ifdef DEBUG
	qFatal("%s", msg.c_str());
#endif

	abort();
}
