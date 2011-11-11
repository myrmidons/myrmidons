#ifndef TESTBOT_H
#define TESTBOT_H

#include "AutoTest.hpp"

class TestBot : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void botBenchmark();
    void cleanupTestCase();
};

DECLARE_TEST(TestBot)

#endif // TESTBOT_H
