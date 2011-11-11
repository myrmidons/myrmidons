#ifndef TESTANT_H
#define TESTANT_H

#include "AutoTest.hpp"

class TestAnt : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void antBenchmark();
    void cleanupTestCase();
};

DECLARE_TEST(TestAnt)

#endif // TESTANT_H
