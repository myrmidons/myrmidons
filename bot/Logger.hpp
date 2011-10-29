#ifndef LOGGER_HPP
#define LOGGER_HPP

#ifdef DEBUG

#include "Bug.hpp" // Used for the accutally output

// Singleton logger. Will create new logfile each turn for easier debuggin.
class Logger {
public:
	static Logger* instance();
	static void free();

	void newTurn(int turn);

	Bug& getBugger() { return m_bug; }

private:
	Logger();
	~Logger();

	static Logger* s_logger;
	Bug m_bug;
};

#	define LOG_DEBUG(msg) Logger::instance()->getBugger() << msg << std::endl << std::flush
#	define LOG_ERROR(msg) LOG_DEBUG("ERROR: " << msg)
#	define LOG_WARNING(msg) LOG_DEBUG("WARNING: " << msg)
#	define STAMP_BODY "STAMP: Function " << __FUNCTION__ << " in file " __FILE__ << " on row " <<  __LINE__
#	define STAMP_ LOG_DEBUG(STAMP_BODY)
#	define STAMP(foo) LOG_DEBUG(STAMP_BODY << ": " << foo)
#else
#	define LOG_DEBUG(msg)
#	define LOG_ERROR(msg)
#	define LOG_WARNING(msg)
#	define STAMP_
#	define STAMP(foo)
#endif

#endif // LOGGER_HPP
