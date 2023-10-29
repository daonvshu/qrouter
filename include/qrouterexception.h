#pragma once

#include <qexception.h>

class QRouterRuntimeException : public QException {
public:
    explicit QRouterRuntimeException(QString message)
        : message(std::move(message))
    {}

    void raise() const override { throw *this; }
    [[nodiscard]] QRouterRuntimeException* clone() const override { return new QRouterRuntimeException(*this); }

    QString message;
};