#pragma once

#include "abstractrouterwidget.h"

#include <ui_page1.h>

class TestPage1 : public AbstractRouterWidget {
    Q_OBJECT

public:
    Q_INVOKABLE explicit TestPage1(const QVariant& data, QWidget* parent = nullptr);

    void onNavigateResult(const QVariant& data) override;

private:
    Ui::TestPage1 ui;

    static int typeId;

protected:
    void runRouterEvent(const QString& event, const QVariant& data) override;
};

Q_DECLARE_METATYPE(TestPage1*);