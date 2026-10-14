#include "container/stackroutercontainer.h"

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

void StackRouterContainer::setCurrentWidgetInternal(AbstractRouterWidget *widget) {
    if (container->indexOf(widget) == -1) {
        container->addWidget(widget);
    }
    container->setCurrentWidget(widget);
}

void StackRouterContainer::cancelTransition() {
    ++transitionGeneration;

    stopAndDelete(activeEnterAnimation);
    stopAndDelete(activeExitAnimation);

    if (activeEnterWidget) {
        activeEnterWidget->setGeometry(activeEnterEndGeometry);
    }
    if (activeExitOverlay) {
        activeExitOverlay->deleteLater();
    }

    activeEnterAnimation.clear();
    activeExitAnimation.clear();
    activeExitOverlay.clear();
    activeEnterWidget.clear();
    activeAnimation.clear();
}

void StackRouterContainer::releaseAnimationOwnerIfIdle() {
    if (!activeEnterAnimation && !activeExitAnimation) {
        activeAnimation.clear();
    }
}

void StackRouterContainer::transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot, const QSharedPointer<RouterAnimation>& animation) {
    cancelTransition();
    setCurrentWidgetInternal(widget);

    if (!transition.enabled ||
        outgoingSnapshot.isNull() ||
        animation.isNull()) {
        return;
    }

    activeAnimation = animation;

    const QRect endGeometry = widget->geometry();
    const quint64 generation = transitionGeneration;

    {
        activeEnterWidget = widget;
        activeEnterEndGeometry = endGeometry;

        auto* enterAnimation = animation->animateEnter(widget);
        if (enterAnimation == nullptr) {
            widget->setGeometry(endGeometry);
            activeEnterWidget.clear();
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
            });
            enterAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }

    auto* overlay = new RouterAnimWidget(container);
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

void StackRouterContainer::removeWidget(AbstractRouterWidget *widget) {
    cancelTransition();
    container->removeWidget(widget);
}
