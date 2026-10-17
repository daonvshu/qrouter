#pragma once

#include <qobject.h>
#include <qpointer.h>
#include <qsharedpointer.h>
#include <qpixmap.h>

class QGraphicsOpacityEffect;
class QParallelAnimationGroup;
class QWidget;
class RouterAnimation;
class RouterAnimWidget;

class RouterPageTransition final : public QObject {
public:
    static RouterPageTransition* start(QWidget* page, const QPixmap& outgoingSnapshot,
                                      const QPointF& outgoingOffset,
                                      const QSharedPointer<RouterAnimation>& animation);
    ~RouterPageTransition() override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    RouterPageTransition(QWidget* page, const QSharedPointer<RouterAnimation>& animation);
    void finish();

    QPointer<QWidget> page;
    QPointer<QWidget> host;
    QPointer<QWidget> window;
    QPointer<RouterAnimWidget> enterOverlay;
    QPointer<RouterAnimWidget> exitOverlay;
    QPointer<QGraphicsOpacityEffect> pageEffect;
    QParallelAnimationGroup* animations = nullptr;
    QSharedPointer<RouterAnimation> animationOwner;
    bool finished = false;
};
