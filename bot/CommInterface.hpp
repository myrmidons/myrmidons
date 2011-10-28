#pragma once

#ifdef BOT_WITH_QT
#include <QApplication>
#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QFuture>
#include <QThread>
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

	void onInputLine(std::string line);
	virtual void outputText(const QByteArray& text) = 0;

Q_SIGNALS:
	void processTurn();
};

inline void CommInterface::onInputLine(std::string line)
{
	inputStream << line << "\n";
	if (line == "go"||
		line == "ready")
	{
		emit processTurn();
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

class LocalCommInterfaceTube : public QObject
{
	Q_OBJECT
public:
	void send(std::string line) { emit lineReceived(QString::fromStdString(line)); }
Q_SIGNALS:
	void lineReceived(QString line);
};

class LocalCommInterfaceWorker : public QThread
{
public:
	LocalCommInterfaceWorker() {}

	QFutureInterface<void> tubeCreate, tubeBound;

	LocalCommInterfaceTube* tube;
	virtual void run()
	{
		tube = new LocalCommInterfaceTube;
		tubeCreate.reportFinished();

		tubeBound.future().waitForFinished();

		std::string line;
		while (std::getline(std::cin, line))
		{
			tube->send(line);
		}
	}
};

class LocalCommInterface : public CommInterface
{
	Q_OBJECT
private:
	LocalCommInterfaceWorker* t;

public:
	LocalCommInterface()
	{
		std::cout.sync_with_stdio(0);
	}

	virtual void go()
	{
		t = new LocalCommInterfaceWorker();
		t->tubeCreate.reportStarted();
		t->start();

		t->tubeBound.reportStarted();
		t->tubeCreate.waitForFinished();
		connect(t->tube, SIGNAL(lineReceived(QString)), SLOT(onTubeLine(QString)));
		t->tubeBound.reportFinished();
	}

	virtual void outputText(const QByteArray &text)
	{
		std::cout << text.constData() << std::flush;
	}

private Q_SLOTS:
	void onTubeLine(QString line)
	{
		onInputLine(line.toAscii().constData());
	}
};


class TurnInitiator : public QObject
{
	Q_OBJECT
private:
	Bot& bot;
public:
	explicit TurnInitiator(Bot& bot) : bot(bot) {}
public Q_SLOTS:
	void doTurn()
	{
		if (!bot.playOneTurn())
			QApplication::quit();
	}
};

#endif
