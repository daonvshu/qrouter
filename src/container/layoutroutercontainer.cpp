#include "container/layoutroutercontainer.h"

#include "routeranimation.h"
#include "routeranimwidget.h"

namespace {
void stopAndDelete(QPointer<QAbstractAnimation>& animation) {
    if (animation) {
        animation->stop();
        delete animation.data();
    }
    animation.clear();
}
}

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

void LayoutRouterContainer::setCurrentWidgetInternal(AbstractRouterWidget *widget) {
    if (!pageInstances.contains(widget)) {
        pageInstances.append(widget);
    }
    removeAllWidgetInternal();
    widget->setVisible(true);
    container->addWidget(widget);
}

void LayoutRouterContainer::cancelTransition() {
    ++transitionGeneration;

    stopAndDelete(activeEnterAnimation);
    stopAndDelete(activeExitAnimation);

    if (activeEnterWidget) {
        activeEnterWidget->setGeometry(activeEnterEndGeometry);
    }
    if (activeExitOverlay) {
        activeExitOverlay->deleteLater();
    }
    if (activeLayoutTransition) {
        container->setEnabled(activeLayoutWasEnabled);
        container->activate();
    }

    activeEnterAnimation.clear();
    activeExitAnimation.clear();
    activeExitOverlay.clear();
    activeEnterWidget.clear();
    activeLayoutTransition = false;
    activeAnimation.clear();
}

void LayoutRouterContainer::releaseAnimationOwnerIfIdle() {
    if (!activeEnterAnimation && !activeExitAnimation) {
        activeAnimation.clear();
    }
}

void LayoutRouterContainer::transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot, const QSharedPointer<RouterAnimation>& animation) {
    cancelTransition();
    setCurrentWidgetInternal(widget);

    if (!transition.enabled ||
        outgoingSnapshot.isNull() ||
        animation.isNull()) {
        return;
    }

    activeAnimation = animation;

    container->activate();
    const QRect endGeometry = widget->geometry();
    const quint64 generation = transitionGeneration;

    {
        activeLayoutTransition = true;
        activeLayoutWasEnabled = container->isEnabled();
        container->setEnabled(false);
        activeEnterWidget = widget;
        activeEnterEndGeometry = endGeometry;

        auto* enterAnimation = animation->animateEnter(widget);
        if (enterAnimation == nullptr) {
            widget->setGeometry(endGeometry);
            activeEnterWidget.clear();
            activeLayoutTransition = false;
            container->setEnabled(activeLayoutWasEnabled);
            container->activate();
        } else {
            activeEnterAnimation = enterAnimation;
            connect(enterAnimation, &QAbstractAnimation::finished, this, [this, widget, endGeometry, generation] {
                if (generation != transitionGeneration) {
                    return;
                }
                widget->setGeometry(endGeometry);
                activeEnterAnimation.clear();
                activeEnterWidget.clear();
                releaseAnimationOwnerIfIdle();
                activeLayoutTransition = false;
                container->setEnabled(activeLayoutWasEnabled);
                container->activate();
            });
            enterAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }

    auto* overlay = new RouterAnimWidget(container->parentWidget());
    overlay->setGeometry(endGeometry);
    overlay->setPixmap(outgoingSnapshot);
    activeExitOverlay = overlay;
    overlay->raise();
    overlay->show();

    auto* exitAnimation = animation->animateExit(overlay);
    if (exitAnimation == nullptr) {
        overlay->deleteLater();
        activeExitOverlay.clear();
    } else {
        activeExitAnimation = exitAnimation;
        connect(exitAnimation, &QAbstractAnimation::finished, this, [this, overlay, generation] {
            if (generation != transitionGeneration) {
                return;
            }
            overlay->deleteLater();
            activeExitAnimation.clear();
            activeExitOverlay.clear();
            releaseAnimationOwnerIfIdle();
        });
        exitAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    releaseAnimationOwnerIfIdle();
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
