#pragma once

#include "../../src/abstractrouterwidget.h"

#include <ui_page1.h>

class TestPage1 : public AbstractRouterWidget {
    Q_OBJECT

public:
    Q_INVOKABLE explicit TestPage1(const QVariant& data, QWidget* parent = nullptr);

    void onNavigateResult(const QVariant& data) override;

private:
    Ui::TestPage1 ui;

    static int typeId;
};

Q_DECLARE_METATYPE(TestPage1*);