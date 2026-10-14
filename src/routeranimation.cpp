#include "routeranimation.h"

#include <QGraphicsOpacityEffect>
#include <qparallelanimationgroup.h>
#include <qpropertyanimation.h>

namespace {
QGraphicsOpacityEffect* installOpacityEffect(QWidget* widget) {
    if (widget->graphicsEffect() != nullptr) {
        return nullptr;
    }
    auto* effect = new QGraphicsOpacityEffect(widget);
    effect->setOpacity(1.0);
    widget->setGraphicsEffect(effect);
    return effect;
}

void removeOpacityEffect(QWidget* widget, QGraphicsOpacityEffect* effect) {
    if (widget != nullptr && widget->graphicsEffect() == effect) {
        widget->setGraphicsEffect(nullptr);
    }
}

void removeOpacityEffectWhenAnimationDestroyed(QAbstractAnimation* animation, QWidget* widget, QGraphicsOpacityEffect* effect) {
    if (effect != nullptr) {
        QObject::connect(animation, &QObject::destroyed, widget, [widget, effect] {
            removeOpacityEffect(widget, effect);
        });
    }
}
}

QAbstractAnimation* FadeSlideRouterAnimation::animateEnter(QWidget* widget) {
    constexpr int offset = 10;
    constexpr int duration = 300;
    const QEasingCurve easingCurve = QEasingCurve::OutCubic;

    const QRect endGeometry = widget->geometry();
    const QRect startGeometry = endGeometry.translated(0, offset);
    widget->setGeometry(startGeometry);

    auto* group = new QParallelAnimationGroup(widget);
    auto* effect = installOpacityEffect(widget);
    removeOpacityEffectWhenAnimationDestroyed(group, widget, effect);

    if (effect != nullptr) {
        effect->setOpacity(0.0);
        auto* opacityAnimation = new QPropertyAnimation(effect, "opacity", group);
        opacityAnimation->setStartValue(0.0);
        opacityAnimation->setEndValue(1.0);
        opacityAnimation->setDuration(duration);
        opacityAnimation->setEasingCurve(easingCurve);
    }

    auto* geometryAnimation = new QPropertyAnimation(widget, "geometry", group);
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(endGeometry);
    geometryAnimation->setDuration(qMax(0, duration));
    geometryAnimation->setEasingCurve(easingCurve);

    return group;
}

QAbstractAnimation* FadeSlideRouterAnimation::animateExit(QWidget* widget) {
    constexpr int offset = 10;
    constexpr int duration = 300;
    const QEasingCurve easingCurve = QEasingCurve::OutCubic;

    const QRect startGeometry = widget->geometry();
    const QRect endGeometry = startGeometry.translated(0, offset);
    auto* group = new QParallelAnimationGroup(widget);

    auto* effect = installOpacityEffect(widget);
    removeOpacityEffectWhenAnimationDestroyed(group, widget, effect);

    if (effect != nullptr) {
        auto* opacityAnimation = new QPropertyAnimation(effect, "opacity", group);
        opacityAnimation->setStartValue(1.0);
        opacityAnimation->setEndValue(0.0);
        opacityAnimation->setDuration(duration);
        opacityAnimation->setEasingCurve(easingCurve);
    }

    auto* geometryAnimation = new QPropertyAnimation(widget, "geometry", group);
    geometryAnimation->setStartValue(startGeometry);
    geometryAnimation->setEndValue(endGeometry);
    geometryAnimation->setDuration(qMax(0, duration));
    geometryAnimation->setEasingCurve(easingCurve);

    return group;
}
