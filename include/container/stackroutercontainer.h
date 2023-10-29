#pragma once

#include <qstackedwidget.h>

#include "../abstractroutercontainer.h"
#include "../abstractrouterwidget.h"

class StackRouterContainer : public AbstractRouterContainer {
public:
    explicit StackRouterContainer(QStackedWidget* stackedWidget);

    QWidget* childPageParent() override;

    QObject * rootPageEventReceiver() override;

    void setCurrentWidget(AbstractRouterWidget *widget) override;

    void removeWidget(AbstractRouterWidget *widget) override;

private:
    QStackedWidget* container;
};