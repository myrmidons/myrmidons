#ifndef TESTPOS_H
#define TESTPOS_H

#include "AutoTest.hpp"

class TestPos : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void posConstructor();
    void cleanupTestCase();
};

DECLARE_TEST(TestPos)

#endif // TESTPOS_H
