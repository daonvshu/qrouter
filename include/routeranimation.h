#pragma once

#include <qabstractanimation.h>
#include <qrect.h>
#include <qwidget.h>

class RouterAnimation {
public:
    virtual ~RouterAnimation() = default;

    /**
     * Animate a transparent snapshot of the page entering the container.
     * @param widget RouterAnimWidget snapshot with an animatable opacity property
     * @return an unstarted animation owned and started by the framework, or nullptr for no animation
     */
    virtual QAbstractAnimation* animateEnter(QWidget* widget) = 0;

    /**
     * Animate a transparent snapshot of the page leaving the container.
     * @param widget RouterAnimWidget snapshot with an animatable opacity property
     * @return an unstarted animation owned and started by the framework, or nullptr for no animation
     */
    virtual QAbstractAnimation* animateExit(QWidget* widget) = 0;
};

class FadeSlideRouterAnimation final : public RouterAnimation {
public:
    QAbstractAnimation* animateEnter(QWidget* widget) override;

    QAbstractAnimation* animateExit(QWidget* widget) override;
};
