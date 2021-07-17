#pragma once

#include "../../src/abstractrouterwidget.h"

#include <ui_page2.h>

class TestPage2 : public AbstractRouterWidget {
    Q_OBJECT

public:
    Q_INVOKABLE explicit TestPage2(const QVariant& data, QWidget* parent = nullptr);

private:
    Ui::TestPage2 ui;

    static int typeId;
};

Q_DECLARE_METATYPE(TestPage2*);