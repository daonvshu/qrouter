#pragma once

#include <qevent.h>

class QRouterPageEvent : public QEvent {
public:
    QRouterPageEvent(QString event, QVariant data)
        : QEvent(type)
        , event(std::move(event))
        , data(std::move(data))
    {}

    static QEvent::Type type;

    QString event;
    QVariant data;
};