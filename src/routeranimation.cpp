#include "routeranimation.h"

#include "routeranimwidget.h"

#include <qparallelanimationgroup.h>
#include <qpropertyanimation.h>

namespace {
QAbstractAnimation* createAnimation(QWidget* widget, bool entering) {
    auto* snapshot = qobject_cast<RouterAnimWidget*>(widget);
    if (!snapshot) {
        return nullptr;
    }

    constexpr int offset = 10;
    constexpr int duration = 300;
    const QRect geometry = widget->geometry();
    const QRect startGeometry = entering ? geometry.translated(0, offset) : geometry;
    const QRect endGeometry = entering ? geometry : geometry.translated(0, offset);
    const qreal startOpacity = entering ? 0.0 : 1.0;
    const qreal endOpacity = entering ? 1.0 : 0.0;

    widget->setGeometry(startGeometry);
    snapshot->setOpacity(startOpacity);
    auto* group = new QParallelAnimationGroup(widget);

    auto* opacityAnimation = new QPropertyAnimation(snapshot, "opacity", group);
    opacityAnimation->setStartValue(startOpacity);
    opacityAnimation->setEndValue(endOpacity);
    opacityAnimation->setDuration(duration);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);

    auto* geometryAnimation = new QPropertyAnimation(widget, "geometry", group);
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(endGeometry);
    geometryAnimation->setDuration(duration);
    geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);

    return group;
}
}

QAbstractAnimation* FadeSlideRouterAnimation::animateEnter(QWidget* widget) {
    return createAnimation(widget, true);
}

QAbstractAnimation* FadeSlideRouterAnimation::animateExit(QWidget* widget) {
    return createAnimation(widget, false);
}
