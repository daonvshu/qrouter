#pragma once

#include <qobject.h>
#include <qvariant.h>

#include "abstractroutercontainer.h"

class AbstractRouterWidget;
class RouterStackManager {
public:
    /////////////////////////////// base information ///////////////////////////////
    /**
     * Get all current page stack names
     * @return names
     */
    QStringList readStack();

    /**
     * Get current top page name
     * @return
     */
    QString currentName();

    /**
     * Get current top page instance
     * @return
     */
    AbstractRouterWidget* current();

    /**
     * Get all page instance from current page stack
     * @return
     */
    QList<AbstractRouterWidget*> currentInstances();

    /**
     * Get instance by name in current page stack, return null if not exist
     * @param pageClassName
     * @return
     */
    AbstractRouterWidget* getInstanceFromStack(const QByteArray& pageClassName);

    /**
     * Get instance by stack index in current page stack, return null if out of range
     * @param stackIndex
     * @return
     */
    AbstractRouterWidget* getInstanceFromStack(int stackIndex);

    /////////////////////////////// stack operation ///////////////////////////////

    /**
     * Initialize the page stack by page names
     * @param pages
     */
    void initStack(const QList<QByteArray>& pages);

    /**
     * Create a page instance and push it into the current page stack
     * @param pageClassName
     * @param data data for new page instance
     */
    void push(const QByteArray& pageClassName, const QVariant& data = QVariant());

    /**
     * Create a page instance and replace the top one on the current page stack
     * @param pageClassName
     * @param data data for new page instance
     */
    void pushReplace(const QByteArray& pageClassName, const QVariant& data = QVariant());

    /**
     * If the page does not exist in the current page stack, create a page instance and push it into the current stack,
     * otherwise move the page to the top of the stack
     * @param pageClassName
     * @param data data for the created page instance
     * @return true if the page instance exist in stack
     */
    bool pushOrMove2Top(const QByteArray& pageClassName, const QVariant& data = QVariant());

    /**
     * Remove the top page from the stack and push the new page into the current stack
     * @param pageClassName
     * @param data data for new page instance
     */
    void popAndPush(const QByteArray& pageClassName, const QVariant& data = QVariant());

    /**
     * Close the specified page (if it exists)
     * @param pageClassName the specified page
     */
    void close(const QByteArray& pageClassName);

    /**
     * Clear current page stack, create a new page and push into the stack
     * @param pageClassName
     * @param data data for new page instance
     */
    void pushAndClear(const QByteArray& pageClassName, const QVariant& data = QVariant());

    /**
     * Move the specified page to top of the page stack
     * @param pageClassName the specified page
     * @return true if the specified page exist in page stack
     */
    bool move2Top(const QByteArray& pageClassName);

    /**
     * Pop up the current page and return to the previous page
     * @param data data for the previous page (receive data by 'onNavigateResult')
     */
    void pop(QVariant data = QVariant());

    /**
     * Pop up the page until the specified page
     * @param untilName
     */
    void popUntil(const QByteArray& untilName);

    /**
     * Pop up the page until 'stackSize' of the current page stack
     * @param stackSize
     */
    void popUntil(int stackSize);

    /////////////////////////////// event operation ///////////////////////////////

    /**
     * Send a event to current page of the top of page stack
     * @param event event name
     * @param data data for current page, receive data by 'runRouterEvent' (no data to feedback) or 'executeRouterEvent'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     * @return processed data of current page
     */
    QVariant sendEventCur(const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /**
     * Send a event to the specified page
     * @param pageClassName the specified page name
     * @param event event name
     * @param data data for current the specified page, receive data by 'runRouterEvent' (no data to feedback) or 'executeRouterEvent'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     * @return processed data of the specified page
     */
    QVariant sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /**
     * Send a event to all page of the current page stack
     * @param event event name
     * @param data data for all page, receive data by 'runRouterEvent'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     */
    void sendEventAll(const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /**
     * Post a event to current page of the top of page stack
     * @param event event name
     * @param data data for current page, receive data by 'runRouterEvent'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     */
    void postEventCur(const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /**
     * Post a event to the specified page
     * @param pageClassName the specified page name
     * @param event event name
     * @param data data for current the specified page, receive data by 'runRouterEvent'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     */
    void postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /**
     * Post a event to the container parent widget (stackWidget->parent())
     * @param event event name
     * @param data data for container parent, receive data by override 'bool event(QEvent* event)'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     */
    void postEventToRoot(const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /**
     * Post a event to all page of the current page stack
     * @param event event name
     * @param data data for all page, receive data by 'runRouterEvent'
     * @param ignoreContainerNotInstalled ignore send event when the current context id of container is not installed
     */
    void postEventAll(const QString& event, const QVariant& data = QVariant(), bool ignoreContainerNotInstalled = true);

    /////////////////////////////// transition animation operation ///////////////////////////////

    //RouterStackManager& alpha(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    //RouterStackManager& translate(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    //RouterStackManager& scale(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    //RouterStackManager& rotate(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);

private:
    AbstractRouterContainer* container = nullptr;
    QList<AbstractRouterWidget*> stack;

    QHash<QString, AbstractRouterWidget*> keepSingletonPageInstance;

    friend class QRouter;

private:
    static AbstractRouterWidget* reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data);

    void removePageInstance(AbstractRouterWidget* widget);
};