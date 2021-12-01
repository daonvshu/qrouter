#pragma once

#include <qobject.h>
#include <qstackedwidget.h>
#include <qevent.h>
#include <qeasingcurve.h>

class QRouterPageEvent : public QEvent {
public:
    QRouterPageEvent(const QString& event, const QVariant& data)
        : QEvent(type)
        , event(event)
        , data(data)
    {}

    static QEvent::Type type;

    QString event;
    QVariant data;
};

class AbstractRouterWidget;
class QRouter {
public:

    static QRouter& install(QStackedWidget* stackContainer, int contextId = 0);

    static QRouter& of(int contextId = 0);

    QStringList readStack();

    QString currentName();

    AbstractRouterWidget* current();

    void initStack(const QList<QByteArray>& pages);

    void push(const QByteArray& pageClassName, const QVariant& data = QVariant());
    void pushReplace(const QByteArray& pageClassName, const QVariant& data = QVariant());
    void popAndPush();
    void pushAndRemove();
    void pushAndClear(const QByteArray& pageClassName, const QVariant& data = QVariant());

    bool move2Top(const QByteArray& pageClassName);

    void pop(QVariant data = QVariant());
    void popUntil(const QByteArray& untilName);
    void popUntil(int stackSize);

    QVariant sendEventCur(const QString& event, const QVariant& data = QVariant());
    QVariant sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data = QVariant());
    void sendEventAll(const QString& event, const QVariant& data = QVariant());

    void postEventCur(const QString& event, const QVariant& data = QVariant());
    void postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data = QVariant());
    void postEventToRoot(const QString& event, const QVariant& data = QVariant());
    void postEventAll(const QString& event, const QVariant& data = QVariant());

    QRouter& alpha(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    QRouter& translate(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    QRouter& scale(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    QRouter& rotate(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);

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

    RouterContainerItem& currentContainer();
};