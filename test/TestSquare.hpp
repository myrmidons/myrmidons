#ifndef TESTSQUARE_H
#define TESTSQUARE_H

#include "AutoTest.hpp"

class TestSquare : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void squareConstructor();
    void cleanupTestCase();
};

DECLARE_TEST(TestSquare)

#endif // TESTSQUARE_H
