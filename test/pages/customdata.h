#pragma once

#include <qobject.h>

struct CustomData {
    qint64 sendTime;
    QString message;
    int pageStackId;
};

Q_DECLARE_METATYPE(CustomData);