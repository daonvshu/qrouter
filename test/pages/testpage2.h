#pragma once

#include "abstractrouterwidget.h"

#include <ui_page2.h>

class TestPage2 : public AbstractRouterWidget {
    Q_OBJECT

public:
    Q_INVOKABLE explicit TestPage2(const QVariant& data, QWidget* parent = nullptr);

    bool attemptClose() override;

    QVariant readAttemptCloseData() override;

private:
    Ui::TestPage2 ui;

    static int typeId;

protected:
    void runRouterEvent(const QString& event, const QVariant& data) override;

private:
    void popWithData();
};

Q_DECLARE_METATYPE(TestPage2*);