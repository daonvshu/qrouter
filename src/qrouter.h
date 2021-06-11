#pragma once

#include <qobject.h>

class QRouter {
public:
    static void push();
    static void pushReplace();
    static void popAndPush();
    static void pushAndRemove();

    static void pop();
    static void popUntil();

private:
    QRouter();

private:
    static QRouter router;
};