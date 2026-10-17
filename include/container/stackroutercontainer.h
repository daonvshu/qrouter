#pragma once

#include <qpointer.h>
#include <qstackedwidget.h>

#include "../abstractroutercontainer.h"
#include "../abstractrouterwidget.h"

class RouterPageTransition;

class StackRouterContainer : public AbstractRouterContainer {
public:
    explicit StackRouterContainer(QStackedWidget* stackedWidget);

    QWidget* childPageParent() override;

    QObject * rootPageEventReceiver() override;

    void setCurrentWidget(AbstractRouterWidget *widget) override;

    void prepareForSnapshot() override;

    void transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot = QPixmap(), const QSharedPointer<RouterAnimation>& animation = {}) override;

    void removeWidget(AbstractRouterWidget *widget) override;

private:
    void cancelTransition();
    void setCurrentWidgetInternal(AbstractRouterWidget* widget);

    QStackedWidget* container;
    QPointer<RouterPageTransition> activeTransition;
};
