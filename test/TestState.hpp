#ifndef TESTSTATE_H
#define TESTSTATE_H

#include "AutoTest.hpp"

class TestState : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void stateBenchmark();
    void cleanupTestCase();
};

DECLARE_TEST(TestState)

#endif // TESTSTATE_H
