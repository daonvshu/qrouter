#pragma once

#include <qpixmap.h>
#include <qsharedpointer.h>
#include <qwidget.h>

class AbstractRouterWidget;
class RouterAnimation;

struct RouterTransition {
    bool enabled = true;
};

class AbstractRouterContainer : public QObject {
public:
    explicit AbstractRouterContainer(QWidget* container): QObject(container) {}

    /**
     * get all child page of parent widget
     * @return
     */
    virtual QWidget* childPageParent() = 0;

    /**
     * set event send target for 'postEventToRoot'
     * @return
     */
    virtual QObject* rootPageEventReceiver() = 0;

    /**
     * set current display page widget
     * @param widget
     */
    virtual void setCurrentWidget(AbstractRouterWidget* widget) = 0;

    /**
     * Set the current page and optionally animate a snapshot of the previous page.
     * @param widget page to display
     * @param transition transition configuration
     * @param outgoingSnapshot snapshot captured before the previous page was removed
     * @param animation animation implementation
     */
    virtual void transitionTo(AbstractRouterWidget* widget, const RouterTransition& transition, const QPixmap& outgoingSnapshot = QPixmap(), const QSharedPointer<RouterAnimation>& animation = {}) {
        Q_UNUSED(transition)
        Q_UNUSED(outgoingSnapshot)
        Q_UNUSED(animation)
        setCurrentWidget(widget);
    }

    /**
     * remove page widget from container
     * @param widget
     */
    virtual void removeWidget(AbstractRouterWidget* widget) = 0;
};
