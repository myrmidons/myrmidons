#include "TestFood.h"
#include "../bot/Food.hpp"

void TestFood::initTestCase()
{
}

void TestFood::foodBenchmark()
{
    QBENCHMARK {
        Food f;
    }
}

void TestFood::cleanupTestCase()
{
}
