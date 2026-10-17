#include "container/stackroutercontainer.h"

#include "utils/routerpagetransition.h"

#include <qlayout.h>

StackRouterContainer::StackRouterContainer(QStackedWidget *stackedWidget)
    : AbstractRouterContainer(stackedWidget)
    , container(stackedWidget)
{}

QWidget *StackRouterContainer::childPageParent() {
    return container;
}

QObject *StackRouterContainer::rootPageEventReceiver() {
    return container->parent();
}

void StackRouterContainer::setCurrentWidget(AbstractRouterWidget *widget) {
    cancelTransition();
    setCurrentWidgetInternal(widget);
}

void StackRouterContainer::prepareForSnapshot() {
    cancelTransition();
    container->layout()->activate();
}

void StackRouterContainer::setCurrentWidgetInternal(AbstractRouterWidget *widget) {
    if (container->indexOf(widget) == -1) {
        container->addWidget(widget);
    }
    container->setCurrentWidget(widget);
}

void StackRouterContainer::cancelTransition() {
    delete activeTransition.data();
    activeTransition.clear();
}

void StackRouterContainer::transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot, const QSharedPointer<RouterAnimation>& animation) {
    cancelTransition();
    setCurrentWidgetInternal(widget);
    container->layout()->activate();

    if (transition.enabled) {
        activeTransition = RouterPageTransition::start(widget, outgoingSnapshot, transition.outgoingSnapshotOffset, animation);
    }
}

void StackRouterContainer::removeWidget(AbstractRouterWidget *widget) {
    cancelTransition();
    container->removeWidget(widget);
}
