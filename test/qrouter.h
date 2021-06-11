#pragma once

#include <QtWidgets/QWidget>
#include "ui_qrouter.h"

class QRouter : public QWidget
{
    Q_OBJECT

public:
    QRouter(QWidget *parent = Q_NULLPTR);

private:
    Ui::QRouterClass ui;
};
