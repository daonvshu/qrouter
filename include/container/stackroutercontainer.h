#pragma once

#include <qpointer.h>
#include <qstackedwidget.h>

#include "../abstractroutercontainer.h"
#include "../abstractrouterwidget.h"

class QAbstractAnimation;
class RouterAnimWidget;

class StackRouterContainer : public AbstractRouterContainer {
public:
    explicit StackRouterContainer(QStackedWidget* stackedWidget);

    QWidget* childPageParent() override;

    QObject * rootPageEventReceiver() override;

    void setCurrentWidget(AbstractRouterWidget *widget) override;

    void transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot = QPixmap(), const QSharedPointer<RouterAnimation>& animation = {}) override;

    void removeWidget(AbstractRouterWidget *widget) override;

private:
    void cancelTransition();
    void setCurrentWidgetInternal(AbstractRouterWidget* widget);
    void releaseAnimationOwnerIfIdle();

    QStackedWidget* container;
    QPointer<QAbstractAnimation> activeEnterAnimation;
    QPointer<QAbstractAnimation> activeExitAnimation;
    QPointer<RouterAnimWidget> activeExitOverlay;
    QPointer<AbstractRouterWidget> activeEnterWidget;
    QSharedPointer<RouterAnimation> activeAnimation;
    QRect activeEnterEndGeometry;
    quint64 transitionGeneration = 0;
};
