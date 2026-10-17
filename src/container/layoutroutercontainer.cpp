#include "container/layoutroutercontainer.h"

#include "utils/routerpagetransition.h"

LayoutRouterContainer::LayoutRouterContainer(QBoxLayout *boxLayout)
    : AbstractRouterContainer(boxLayout->parentWidget())
    , container(boxLayout)
{}

QWidget *LayoutRouterContainer::childPageParent() {
    return container->parentWidget();
}

QObject *LayoutRouterContainer::rootPageEventReceiver() {
    return container->parentWidget();
}

void LayoutRouterContainer::setCurrentWidget(AbstractRouterWidget *widget) {
    cancelTransition();
    setCurrentWidgetInternal(widget);
}

void LayoutRouterContainer::prepareForSnapshot() {
    cancelTransition();
    container->activate();
}

void LayoutRouterContainer::setCurrentWidgetInternal(AbstractRouterWidget *widget) {
    if (!pageInstances.contains(widget)) {
        pageInstances.append(widget);
    }
    removeAllWidgetInternal();
    widget->setVisible(true);
    container->addWidget(widget);
}

void LayoutRouterContainer::cancelTransition() {
    delete activeTransition.data();
    activeTransition.clear();
}

void LayoutRouterContainer::transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot, const QSharedPointer<RouterAnimation>& animation) {
    cancelTransition();
    setCurrentWidgetInternal(widget);
    container->activate();

    if (transition.enabled) {
        activeTransition = RouterPageTransition::start(widget, outgoingSnapshot, transition.outgoingSnapshotOffset, animation);
    }
}

void LayoutRouterContainer::removeWidget(AbstractRouterWidget *widget) {
    cancelTransition();
    if (pageInstances.contains(widget)) {
        pageInstances.removeOne(widget);
    }
    if (auto item = container->itemAt(0)) {
        if (item->widget() == widget) {
            removeAllWidgetInternal();
        }
    }
}

void LayoutRouterContainer::removeAllWidget() {
    cancelTransition();
    removeAllWidgetInternal();
}

void LayoutRouterContainer::removeAllWidgetInternal() {
    if (auto item = container->takeAt(0)) {
        item->widget()->setVisible(false);
        delete item;
    }
}
