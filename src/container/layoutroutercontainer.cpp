#include "container/layoutroutercontainer.h"

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
    if (!pageInstances.contains(widget)) {
        pageInstances.append(widget);
    }
    removeAllWidget();
    widget->setVisible(true);
    container->addWidget(widget);
}

void LayoutRouterContainer::removeWidget(AbstractRouterWidget *widget) {
    if (pageInstances.contains(widget)) {
        pageInstances.removeOne(widget);
    }
    if (auto item = container->itemAt(0)) {
        if (item->widget() == widget) {
            removeAllWidget();
        }
    }
}

void LayoutRouterContainer::removeAllWidget() {
    if (auto item = container->takeAt(0)) {
        item->widget()->setVisible(false);
        delete item;
    }
}