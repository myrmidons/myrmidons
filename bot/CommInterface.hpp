#pragma once

#ifdef BOT_WITH_QT
#include <QApplication>
#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "Bot.hpp"
#include <sstream>

class CommInterface : public QObject, public IODevice
{
	Q_OBJECT
private:
	std::stringstream inputStream;
	std::ostringstream outputStream;
protected:
	Bot* bot;

public:
	explicit CommInterface(QObject* parent = 0) : QObject(parent) {}
	virtual void go() = 0;

	// from IODevice
	virtual std::istream& input() { return inputStream; }
	virtual std::ostream& output() { return outputStream; }

	void setBot(Bot* bot);

	void onInputLine(const char* line);
	virtual void outputText(const QByteArray& text) = 0;
};

inline void CommInterface::setBot(Bot* bot)
{
	this->bot = bot;
}

inline void CommInterface::onInputLine(const char *line)
{
	inputStream << line << "\n";
	if (qstrcmp(line, "go") == 0 ||
		qstrcmp(line, "ready") == 0)
	{
		bot->playOneTurn();
		inputStream.str("");

		std::string text = outputStream.str();
		QByteArray buf(text.c_str(), text.size());
		outputText(buf);
		outputStream.clear();
		outputStream.str("");
	}
}

class TcpCommInterface : public CommInterface
{
	Q_OBJECT
private:
	unsigned int port;
	QTcpServer* serv;
	QTcpSocket* sock;

public:
	explicit TcpCommInterface(unsigned short port, QObject* parent = 0)
		: CommInterface(parent)
		, port(port)
		, serv(new QTcpServer(this))
	{
	}

	void go()
	{
		connect(serv, SIGNAL(newConnection()), SLOT(connected()));
		serv->listen(QHostAddress::Any, port);
	}

	virtual void outputText(const QByteArray& text)
	{
		sock->write(text.constData(), text.size());
	}

private Q_SLOTS:
	void connected()
	{
		sock = serv->nextPendingConnection();
		sock->setSocketOption(QAbstractSocket::LowDelayOption, 1);
		connect(sock, SIGNAL(readyRead()),
				SLOT(readSome()));
		connect(sock, SIGNAL(disconnected()),
				SLOT(disconnected()));
	}

	void readSome()
	{
		while (sock->canReadLine())
		{
			QString line = sock->readLine().trimmed();
			onInputLine(line.toAscii().constData());
		}
	}

	void disconnected()
	{
		QApplication::exit(0);
	}
};

class LocalCommInterface : public CommInterface
{

};

#endif
