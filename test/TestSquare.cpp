#include "TestSquare.hpp"
#include "../bot/Square.hpp"
//#include "../bot/Tracker.hpp"

void TestSquare::initTestCase()
{
}

void TestSquare::squareConstructor()
{
    Square s;
    QVERIFY(!s.discovered);
    QVERIFY(!s.isWater);
    QVERIFY(!s.isHill);
    QVERIFY(!s.isFood);
    QVERIFY(NULL == s.pAnt);
    QVERIFY(NULL == s.destinyAnt);
    QVERIFY(NULL == s.room);
    QVERIFY(NO_TEAM == s.antTeam);
    QVERIFY(NO_TEAM == s.hillTeam);
    QVERIFY(s.hillAlive);

    QVERIFY(!s.m_isVisible);
    QVERIFY(!s.isFood);
    QVERIFY(0 == s.deadAnts.size());
}

void TestSquare::cleanupTestCase()
{
}
