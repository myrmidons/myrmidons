HEADERS += \
    Ant.hpp \
    Bot.hpp \
    Bug.hpp \
    Food.hpp \
    Pos.hpp \
    Square.hpp \
    State.hpp \
    Timer.hpp \
    Identifier.hpp \
    Room.hpp \
    Util.hpp \
    General.hpp \
    Map.hpp \
    AntStar.hpp \
    Assert.hpp

SOURCES += \
    Ant.cpp \
    Bot.cpp \
    Food.cpp \
    MyBot.cpp \
    State.cpp \
    Identifier.cpp \
    Room.cpp \
    General.cpp \
    Map.cpp \
    AntStar.cpp \
    Assert.cpp

# Add annoying warning disables below, e.g. -Wno-cats-allowed
# QMAKE_CXXFLAGS_WARN_ON = \

debug {
    DEFINES += DEBUG
}

unix {
    # 10.7==Lion not yet supported
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
}


