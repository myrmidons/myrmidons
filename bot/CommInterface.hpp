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
	QTcpServer* serv;
	QTcpSocket* sock;
	Bot* bot;

	std::stringstream inputStream;
	std::ostringstream outputStream;

public:
	CommInterface(QObject* parent = 0)
		: QObject(parent)
		, serv(new QTcpServer(this))
	{
	}

	void setBot(Bot* bot)
	{
		this->bot = bot;
	}

	void listen(unsigned short port)
	{
		connect(serv, SIGNAL(newConnection()), SLOT(connected()));
		serv->listen(QHostAddress::Any, port);
	}

	virtual std::istream& input() { return inputStream; }
	virtual std::ostream& output() { return outputStream; }

Q_SIGNALS:

private Q_SLOTS:
	void connected()
	{
		sock = serv->nextPendingConnection();
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
			inputStream << line.toAscii().constData() << "\n";
			if (line == "go" || line == "ready")
			{
				bot->playOneTurn();
				inputStream.str("");

				std::string text = outputStream.str();
				sock->write(text.c_str(), text.size());
				outputStream.clear();
			}
		}
	}

	void disconnected()
	{
		QApplication::exit(0);
	}
};

#endif
