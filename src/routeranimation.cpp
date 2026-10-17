#include "routeranimation.h"

#include "routeranimwidget.h"

#include <qparallelanimationgroup.h>
#include <qpropertyanimation.h>

void FadeSlideRouterAnimation::setOffset(int value) {
    offset = qMax(0, value);
}

void FadeSlideRouterAnimation::setDuration(int value) {
    duration = qMax(0, value);
}

void FadeSlideRouterAnimation::setEasingCurve(const QEasingCurve& value) {
    easingCurve = value;
}

void FadeSlideRouterAnimation::setDirection(Direction value) {
    direction = value;
}

void FadeSlideRouterAnimation::setEnterOpacityRange(qreal start, qreal end) {
    enterStartOpacity = qBound(qreal(0.0), start, qreal(1.0));
    enterEndOpacity = qBound(qreal(0.0), end, qreal(1.0));
}

void FadeSlideRouterAnimation::setExitOpacityRange(qreal start, qreal end) {
    exitStartOpacity = qBound(qreal(0.0), start, qreal(1.0));
    exitEndOpacity = qBound(qreal(0.0), end, qreal(1.0));
}

QPoint FadeSlideRouterAnimation::offsetForDirection(Direction value) const {
    switch (value) {
    case Direction::Top:
        return QPoint(0, offset);
    case Direction::Bottom:
        return QPoint(0, -offset);
    case Direction::Left:
        return QPoint(offset, 0);
    case Direction::Right:
        return QPoint(-offset, 0);
    }
    return {};
}

QAbstractAnimation* FadeSlideRouterAnimation::createAnimation(QWidget* widget, bool entering) {
    auto* snapshot = qobject_cast<RouterAnimWidget*>(widget);
    if (!snapshot) {
        return nullptr;
    }

    const QRect geometry = widget->geometry();
    const QPoint displacement = offsetForDirection(direction);
    const QRect startGeometry = entering ? geometry.translated(displacement) : geometry;
    const QRect endGeometry = entering ? geometry : geometry.translated(displacement);
    const qreal startOpacity = entering ? enterStartOpacity : exitStartOpacity;
    const qreal endOpacity = entering ? enterEndOpacity : exitEndOpacity;

    widget->setGeometry(startGeometry);
    snapshot->setOpacity(startOpacity);
    auto* group = new QParallelAnimationGroup(widget);

    auto* opacityAnimation = new QPropertyAnimation(snapshot, "opacity", group);
    opacityAnimation->setStartValue(startOpacity);
    opacityAnimation->setEndValue(endOpacity);
    opacityAnimation->setDuration(duration);
    opacityAnimation->setEasingCurve(easingCurve);

    auto* geometryAnimation = new QPropertyAnimation(widget, "geometry", group);
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(endGeometry);
    geometryAnimation->setDuration(duration);
    geometryAnimation->setEasingCurve(easingCurve);

    return group;
}

QAbstractAnimation* FadeSlideRouterAnimation::animateEnter(QWidget* widget) {
    return createAnimation(widget, true);
}

QAbstractAnimation* FadeSlideRouterAnimation::animateExit(QWidget* widget) {
    return createAnimation(widget, false);
}
