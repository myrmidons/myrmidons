#include "TestBot.hpp"
#include "../bot/Bot.hpp"

void TestBot::initTestCase()
{
}

void TestBot::botConstructor()
{
    IODevice* io = NULL;
    Bot b(*io);
}

void TestBot::cleanupTestCase()
{
}
