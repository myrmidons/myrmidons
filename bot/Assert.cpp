#include "Assert.hpp"
#include "State.hpp"
#include <sstream>

#ifdef DEBUG
#include <QDebug>
#endif

void AssertionFailed(const char* expr,
					 const char* function, const char* file, long line)
{
	std::stringstream ss;
	ss << "Assertion \"" << expr << "\" failed in \"" << function << "\", in " << file << ":" << line;
	std::string msg = ss.str();

	LOG_DEBUG( msg );

#ifdef DEBUG
	qFatal(msg.c_str());
#endif

	abort();
}
