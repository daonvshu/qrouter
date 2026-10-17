#include "routerpagetransition.h"

#include "routeranimation.h"
#include "routeranimwidget.h"

#include <qevent.h>
#include <qgraphicseffect.h>
#include <qparallelanimationgroup.h>

RouterPageTransition::RouterPageTransition(QWidget* widget, const QSharedPointer<RouterAnimation>& animation)
    : QObject(widget->parentWidget())
    , page(widget)
    , host(widget->parentWidget())
    , window(widget->window())
    , animations(new QParallelAnimationGroup(this))
    , animationOwner(animation)
{}

RouterPageTransition::~RouterPageTransition() {
    finish();
    delete animations;
    delete enterOverlay.data();
    delete exitOverlay.data();
}

RouterPageTransition* RouterPageTransition::start(QWidget* widget, const QPixmap& outgoingSnapshot,
                                                const QPointF& outgoingOffset,
                                                const QSharedPointer<RouterAnimation>& animation) {
    // Leave existing page effects under the page's control.
    if (!widget->isVisible() || !widget->updatesEnabled() || widget->graphicsEffect() ||
        !widget->parentWidget() || outgoingSnapshot.isNull() || animation.isNull()) {
        return nullptr;
    }

    QPointF incomingOffset;
    const QPixmap incomingSnapshot = RouterAnimWidget::capture(widget, &incomingOffset);
    if (incomingSnapshot.isNull()) {
        return nullptr;
    }

    auto* transition = new RouterPageTransition(widget, animation);
    auto* entering = new RouterAnimWidget(widget->parentWidget());
    entering->setGeometry(widget->geometry());
    entering->setPixmap(incomingSnapshot, incomingOffset);
    transition->enterOverlay = entering;

    auto* exiting = new RouterAnimWidget(widget->parentWidget());
    exiting->setGeometry(widget->geometry());
    exiting->setPixmap(outgoingSnapshot, outgoingOffset);
    transition->exitOverlay = exiting;

    if (auto* enterAnimation = animation->animateEnter(entering)) {
        transition->animations->addAnimation(enterAnimation);
    }
    auto* exitAnimation = animation->animateExit(exiting);
    if (exitAnimation) {
        transition->animations->addAnimation(exitAnimation);
    }
    if (transition->animations->animationCount() == 0) {
        delete transition;
        return nullptr;
    }

    // Suppress live page painting to avoid compositing translucent content twice.
    // Keep the page visible so layout and lifecycle events continue normally.
    auto* effect = new QGraphicsOpacityEffect(widget);
    effect->setOpacity(0.0);
    widget->setGraphicsEffect(effect);
    transition->pageEffect = effect;

    widget->installEventFilter(transition);
    transition->host->installEventFilter(transition);
    if (transition->window != transition->host) {
        transition->window->installEventFilter(transition);
    }
    connect(widget, &QObject::destroyed, transition, [transition] {
        transition->page.clear();
        transition->finish();
        transition->deleteLater();
    });
    connect(transition->animations, &QAbstractAnimation::finished, transition, [transition] {
        transition->finish();
        transition->deleteLater();
    });

    entering->show();
    entering->raise();
    if (exitAnimation) {
        exiting->show();
        exiting->raise();
    }
    transition->host->update(widget->geometry());
    transition->animations->start();
    return transition;
}

void RouterPageTransition::finish() {
    if (finished) {
        return;
    }
    finished = true;
    animations->stop();

    if (page) {
        page->removeEventFilter(this);
        if (pageEffect && page->graphicsEffect() == pageEffect.data()) {
            page->setGraphicsEffect(nullptr);
        }
    }
    if (host) {
        host->removeEventFilter(this);
        host->update();
    }
    if (window && window != host) {
        window->removeEventFilter(this);
    }
    if (enterOverlay) {
        enterOverlay->hide();
    }
    if (exitOverlay) {
        exitOverlay->hide();
    }
}

bool RouterPageTransition::eventFilter(QObject* watched, QEvent* event) {
    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move:
    case QEvent::Hide:
    case QEvent::ParentChange:
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    case QEvent::DevicePixelRatioChange:
#endif
        finish();
        deleteLater();
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}
