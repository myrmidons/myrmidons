#include "TestCoordinator.hpp"
#include "../bot/Coordinator.hpp"

void TestCoordinator::initTestCase()
{
}

void TestCoordinator::coordinatorConstructor()
{
    Coordinator c;
}

void TestCoordinator::moveAntsAfterDesire()
{
    Coordinator c;
    AntSet antset;
    c.moveAntsAfterDesire(antset);
}

/*
// Invalid test. EBSAC.
void TestCoordinator::moveAntsAfterDesireSim()
{
    Coordinator c;
    AntSet antset;
    Ant ant;

    antset.insert(&ant);
    c.moveAntsAfterDesire(antset);
}
*/

void TestCoordinator::cleanupTestCase()
{
}
