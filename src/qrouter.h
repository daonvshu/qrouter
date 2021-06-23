#pragma once

#include <qobject.h>
#include <qstackedwidget.h>

#include "abstractrouterwidget.h"

class QRouter {
public:

    static QRouter& install(QStackedWidget* stackContainer, int contextId = 0);

    static QRouter& of(int contextId = 0);

    QStringList readStack();

    void push(const QByteArray& pageClassName, const QVariant& data);
    void pushReplace(const QByteArray& pageClassName, const QVariant& data);
    void popAndPush();
    void pushAndRemove();

    void pop(const QVariant& data);
    void popUntil(const QByteArray& untilName);

private:
    QRouter();

private:
    static QRouter router;

    struct RouterContainerItem {
        QStackedWidget* container;
        QList<AbstractRouterWidget*> stack;
    };

    QHash<int, RouterContainerItem> containers;
    int m_curContextId;

private:
    AbstractRouterWidget* reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data);
};