#ifndef TESTFOOD_H
#define TESTFOOD_H

#include "AutoTest.h"

class TestFood : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void foodBenchmark();
    void cleanupTestCase();
};

DECLARE_TEST(TestFood)

#endif // TEST1_H
