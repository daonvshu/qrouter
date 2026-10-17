#pragma once

#include <qabstractanimation.h>
#include <qeasingcurve.h>
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

class FadeSlideRouterAnimation : public RouterAnimation {
public:
    /**
     * Direction describes movement towards the resting position on entry.
     * Exit follows the same path in reverse.
     */
    enum class Direction {
        Top,
        Bottom,
        Left,
        Right
    };

    /**
     * Set the slide distance in logical pixels. Negative values are clamped to zero.
     */
    void setOffset(int value);

    /**
     * Set the duration of both phases in milliseconds. Negative values are clamped to zero.
     */
    void setDuration(int value);

    /**
     * Set the easing curve for both opacity and movement.
     */
    void setEasingCurve(const QEasingCurve& value);

    /**
     * Set the entry direction. Exit retraces the entry path away from the resting position.
     */
    void setDirection(Direction value);

    /**
     * Set the entering opacity endpoints, clamped to the range [0, 1].
     */
    void setEnterOpacityRange(qreal start, qreal end);

    /**
     * Set the exiting opacity endpoints, clamped to the range [0, 1].
     */
    void setExitOpacityRange(qreal start, qreal end);

    QAbstractAnimation* animateEnter(QWidget* widget) override;

    QAbstractAnimation* animateExit(QWidget* widget) override;

private:
    QAbstractAnimation* createAnimation(QWidget* widget, bool entering);
    QPoint offsetForDirection(Direction value) const;

    int offset = 10;
    int duration = 300;
    QEasingCurve easingCurve = QEasingCurve::OutCubic;
    Direction direction = Direction::Top;
    qreal enterStartOpacity = 0.0;
    qreal enterEndOpacity = 1.0;
    qreal exitStartOpacity = 1.0;
    qreal exitEndOpacity = 0.0;
};
