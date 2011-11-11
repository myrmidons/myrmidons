#ifndef TESTANTSTAR_H
#define TESTANTSTAR_H

#include "AutoTest.hpp"

class TestAntStar : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void antstarBenchmark();
    void cleanupTestCase();
};

DECLARE_TEST(TestAntStar)

#endif // TESTANTSTAR_H
