#pragma once

#include <qwidget.h>

class AbstractRouterWidget;
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
     * remove page widget from container
     * @param widget
     */
    virtual void removeWidget(AbstractRouterWidget* widget) = 0;
};