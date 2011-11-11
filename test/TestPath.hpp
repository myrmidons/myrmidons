#ifndef TESTPATH_H
#define TESTPATH_H

#include "AutoTest.hpp"

class TestPath : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void pathConstructor();
    void cleanupTestCase();
};

DECLARE_TEST(TestPath)

#endif // TESTPATH_H
