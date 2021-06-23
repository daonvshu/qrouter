#pragma once

#include <qobject.h>
#include <qstackedwidget.h>

#include "abstractrouterwidget.h"

class QRouter {
public:

    static QRouter& install(QStackedWidget* stackContainer, int contextId = 0);

    static QRouter& of(int contextId = 0);

    QStringList readStack();

    void push(const QByteArray& pageClassName, const QVariant& data = QVariant());
    void pushReplace(const QByteArray& pageClassName, const QVariant& data = QVariant());
    void popAndPush();
    void pushAndRemove();
    void pushAndClear(const QByteArray& pageClassName, const QVariant& data = QVariant());

    void pop(const QVariant& data);
    void popUntil(const QByteArray& untilName);

    QVariant sendEvent(const QString& event, const QVariant& data = QVariant());
    void sendEventAll(const QString& event, const QVariant& data = QVariant());

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

    RouterContainerItem& currentContainter();
};