#pragma once

#include <qboxlayout.h>

#include "../abstractroutercontainer.h"
#include "../abstractrouterwidget.h"

class LayoutRouterContainer : public AbstractRouterContainer {
public:
    explicit LayoutRouterContainer(QBoxLayout* boxLayout);

    QWidget* childPageParent() override;

    QObject * rootPageEventReceiver() override;

    void setCurrentWidget(AbstractRouterWidget *widget) override;

    void removeWidget(AbstractRouterWidget* widget) override;

    virtual void removeAllWidget();

private:
    QBoxLayout* container;
    QList<QWidget*> pageInstances;
};