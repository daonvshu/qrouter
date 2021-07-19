#pragma once

#include <QtWidgets/QWidget>
#include "ui_qrouter.h"

class QRouterTest : public QWidget
{
    Q_OBJECT

public:
    QRouterTest(QWidget *parent = Q_NULLPTR);

private:
    Ui::QRouterClass ui;

private:
    void printPageStack();

protected:
    bool event(QEvent* event) override;
};
