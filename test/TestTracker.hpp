#ifndef TESTTRACKER_H
#define TESTTRACKER_H

#include "AutoTest.hpp"

class TestTracker : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void trackerConstructor();
    void cleanupTestCase();
};

DECLARE_TEST(TestTracker)

#endif // TESTTRACKER_H
