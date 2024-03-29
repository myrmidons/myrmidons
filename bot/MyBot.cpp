#include "Bot.hpp"
#include "Room.hpp"
#include "Map.hpp"
#include "Tracker.hpp"
#include "Logger.hpp"
#include "Coordinator.hpp"
using namespace std;

#ifdef BOT_WITH_QT
#include <QApplication>
#include <QStringList>
#include <sstream>
#include "CommInterface.hpp"

#endif

struct StandardIODevice : IODevice
{
	StandardIODevice()
	{
		cout.sync_with_stdio(0); //this line makes your bot faster
	}

	std::istream& input()
	{
		return std::cin;
	}

	std::ostream& output()
	{
		return std::cout;
	}
};

/*
    This program will play a single game of Ants while communicating with
    the engine via standard input and output.

    The function "makeMoves()" in Bot.cc is where it makes the moves
    each turn and is probably the best place to start exploring. You are
    allowed to edit any part of any of the files, remove them, or add your
    own, provided you continue conforming to the input and output format
    outlined on the specifications page at:
        http://www.ai-contest.com
*/
int main(int argc, char *argv[])
{
	IODevice* io = NULL;
#ifdef BOT_WITH_QT
	QApplication app(argc, argv);
	QStringList args = app.arguments();

	unsigned short port = 0;

	bool standalone = args.size() == 1;
	CommInterface* commInterface = NULL;

	if (standalone)
	{
		io = commInterface = new LocalCommInterface;
	}
	else
	{
		std::istringstream iss(args[1].toAscii().constData());
		if (!(iss >> port))
		{
			std::cerr << "could not parse port argument\n";
			return 2;
		}
		io = commInterface = new TcpCommInterface(port);
	}

#else
	io = new StandardIODevice;
#endif

	State state(io->output());
	g_state = &state;

	Map map;
	g_map = &map; // Must be before g_tracker.

	Coordinator coord;
	g_coordinator = &coord;

	Tracker tracker;
	g_tracker = &tracker;

	Rooms rooms;
	g_rooms = &rooms;

	Bot bot(*io);
#ifdef BOT_WITH_QT
	TurnInitiator ti(bot);
	if (commInterface)
	{
		ti.connect(commInterface, SIGNAL(processTurn()), SLOT(doTurn()));
		commInterface->go();
	}

	app.exec();
#else
	bot.playGame();
#endif

	LOG_DEBUG("Reached the end of main().");

#ifdef DEBUG
	Logger::free();
#endif

    return 0;
}
