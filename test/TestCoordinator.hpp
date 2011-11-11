#ifndef TESTCOORDINATOR_H
#define TESTCOORDINATOR_H

#include "AutoTest.hpp"

class TestCoordinator : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void coordinatorBenchmark();
    void cleanupTestCase();
};

DECLARE_TEST(TestCoordinator)

#endif // TESTCOORDINATOR_H
