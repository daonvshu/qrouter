#pragma once

#include <qobject.h>

struct CustomData {
    qint64 sendTime;
    QString message;
};

Q_DECLARE_METATYPE(CustomData);