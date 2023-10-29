#include "container/stackroutercontainer.h"

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
    if (container->indexOf(widget) == -1) {
        container->addWidget(widget);
    }
    container->setCurrentWidget(widget);
}

void StackRouterContainer::removeWidget(AbstractRouterWidget *widget) {
    container->removeWidget(widget);
}