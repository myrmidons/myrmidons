#ifndef TESTFOOD_H
#define TESTFOOD_H

#include "AutoTest.hpp"

class TestFood : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void foodConstructor();
    void cleanupTestCase();
};

DECLARE_TEST(TestFood)

#endif // TESTFOOD_H
