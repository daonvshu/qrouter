#pragma once

#include <qboxlayout.h>
#include <qpointer.h>

#include "../abstractroutercontainer.h"
#include "../abstractrouterwidget.h"

class RouterPageTransition;

class LayoutRouterContainer : public AbstractRouterContainer {
public:
    explicit LayoutRouterContainer(QBoxLayout* boxLayout);

    QWidget* childPageParent() override;

    QObject * rootPageEventReceiver() override;

    void setCurrentWidget(AbstractRouterWidget *widget) override;

    void prepareForSnapshot() override;

    void transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot = QPixmap(), const QSharedPointer<RouterAnimation>& animation = {}) override;

    void removeWidget(AbstractRouterWidget* widget) override;

    virtual void removeAllWidget();

private:
    void cancelTransition();
    void setCurrentWidgetInternal(AbstractRouterWidget* widget);
    void removeAllWidgetInternal();

    QBoxLayout* container;
    QList<QWidget*> pageInstances;
    QPointer<RouterPageTransition> activeTransition;
};
