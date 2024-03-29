HEADERS += \
    Ant.hpp \
    Bot.hpp \
    Bug.hpp \
    Food.hpp \
    Pos.hpp \
    Square.hpp \
    State.hpp \
    Timer.hpp \
    Room.hpp \
    Util.hpp \
    General.hpp \
    Map.hpp \
    AntStar.hpp \
    Assert.hpp \
	Tracker.hpp \
	Path.hpp \
	CommInterface.hpp \
    DebugWindow.hpp \
    RoomContent.hpp \
    Logger.hpp \
	Coordinator.hpp \
    PathFinder.hpp

SOURCES += \
    Ant.cpp \
    Bot.cpp \
    Food.cpp \
    MyBot.cpp \
    State.cpp \
    Room.cpp \
    General.cpp \
    Map.cpp \
    AntStar.cpp \
    Assert.cpp \
    Tracker.cpp \
    RoomContent.cpp \
    Path.cpp \
    DebugWindow.cpp \
    Logger.cpp \
    Square.cpp \
    Coordinator.cpp \
    PathFinder.cpp

# Add annoying warning disables below, e.g. -Wno-cats-allowed
# QMAKE_CXXFLAGS_WARN_ON = \

debug {
    DEFINES += DEBUG

	DEFINES += \
		BOT_WITH_QT

	CONFIG += qt
	QT += network
}

macx {
    # 10.7==Lion not yet supported
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6

    DEFINES += THIS_IS_MAC=1
}

QMAKE_CXXFLAGS += -Wall -pedantic -Wextra -Wno-long-long

#DEFINES += \
#	BOT_WITH_QT

CONFIG += qt
QT += network









