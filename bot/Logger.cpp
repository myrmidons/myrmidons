#include "Logger.hpp"
#include <QDir>

#ifdef DEBUG

Logger* Logger::s_logger = NULL;

Logger* Logger::instance() {
	if (!s_logger)
		s_logger = new Logger();
	return s_logger;
}

void Logger::free() {
	delete s_logger;
	s_logger = NULL;
}

Logger::Logger() {
	// Erase old log files (and image files too, just to play nice):
	QDir workdir(".");
	QStringList entries = workdir.entryList();
	foreach(QString str, entries) {
		if (str.endsWith(".txt") || str.endsWith(".png"))
			QFile::remove(str);
	}

	////////////////////////////////////////////////////////////

	m_bug.open("log__preturn.txt");
}

Logger::~Logger() {
	m_bug.close();
}

void Logger::newTurn(int turn) {
	m_bug.close();

	char nameBuf[100] = {};
	sprintf(nameBuf, "log_%04d.txt", turn);
	m_bug.open(nameBuf);
}


void Logger::endGame() {
	m_bug.close();
	m_bug.open("log__endgame.txt");
}

#endif
