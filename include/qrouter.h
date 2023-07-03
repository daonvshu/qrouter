#pragma once

#include <qobject.h>
#include <qstackedwidget.h>
#include <qevent.h>
#include <qeasingcurve.h>
#include <qthread.h>
#include <qhash.h>

class QRouterPageEvent : public QEvent {
public:
    QRouterPageEvent(const QString& event, const QVariant& data)
        : QEvent(type)
        , event(event)
        , data(data)
    {}

    static QEvent::Type type;

    QString event;
    QVariant data;
};

class AbstractRouterWidget;
class QRouter {
public:
    /**
     * Use the specified 'stackedWidget' as the page stack container
     * @param stackContainer page stack container
     * @param contextId page stack id
     * @return
     */
    static QRouter& install(QStackedWidget* stackContainer, int contextId = 0);

    /**
     * Get page stack instance by id
     * @param contextId
     * @return
     */
    static QRouter& of(int contextId = 0);

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
     * Get current top page instance
     * @tparam T instance type
     * @param contextId page id
     * @return
     */
    template<typename T>
    static T* currentInstance(int contextId = 0) {
        return dynamic_cast<T*>(QRouter::of(contextId).current());
    }

    /**
     * Get instance by name in current page stack, return null if not exist
     * @param pageClassName
     * @return
     */
    AbstractRouterWidget* getInstanceFromStack(const QByteArray& pageClassName);

    /**
     * Get instance by name in current page stack, return null if not exist
     * @tparam T instance type
     * @param contextId page id
     * @return
     */
    template<typename T>
    static T* getInstance(int contextId = 0) {
        return dynamic_cast<T*>(QRouter::of(contextId).getInstanceFromStack(T::staticMetaObject.className()));
    }

    /**
     * Get page stack id of the specified container
     * @param container
     * @return -1 if not register by 'install'
     */
    static int getIdByContainer(QStackedWidget* container);

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
     * @return processed data of current page
     */
    QVariant sendEventCur(const QString& event, const QVariant& data = QVariant());

    /**
     * Send a event to the specified page
     * @param pageClassName the specified page name
     * @param event event name
     * @param data data for current the specified page, receive data by 'runRouterEvent' (no data to feedback) or 'executeRouterEvent'
     * @return processed data of the specified page
     */
    QVariant sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data = QVariant());

    /**
     * Send a event to all page of the current page stack
     * @param event event name
     * @param data data for all page, receive data by 'runRouterEvent'
     */
    void sendEventAll(const QString& event, const QVariant& data = QVariant());

    /**
     * Post a event to current page of the top of page stack
     * @param event event name
     * @param data data for current page, receive data by 'runRouterEvent'
     */
    void postEventCur(const QString& event, const QVariant& data = QVariant());

    /**
     * Post a event to the specified page
     * @param pageClassName the specified page name
     * @param event event name
     * @param data data for current the specified page, receive data by 'runRouterEvent'
     */
    void postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data = QVariant());

    /**
     * Post a event to the container parent widget (stackWidget->parent())
     * @param event event name
     * @param data data for container parent, receive data by override 'bool event(QEvent* event)'
     */
    void postEventToRoot(const QString& event, const QVariant& data = QVariant());

    /**
     * Post a event to all page of the current page stack
     * @param event event name
     * @param data data for all page, receive data by 'runRouterEvent'
     */
    void postEventAll(const QString& event, const QVariant& data = QVariant());

    /////////////////////////////// transition animation operation ///////////////////////////////

    QRouter& alpha(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    QRouter& translate(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    QRouter& scale(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);
    QRouter& rotate(int duration = 300, const QEasingCurve& easingCurve = QEasingCurve::OutCubic);

private:
    QRouter();

private:
    static QRouter router;

    struct RouterContainerItem {
        QStackedWidget* container;
        QList<AbstractRouterWidget*> stack;
    };

    QHash<int, RouterContainerItem> containers;
    QHash<QThread*, int> m_curContextId;

    QHash<QString, AbstractRouterWidget*> keepSingletonPageInstance;

private:
    static AbstractRouterWidget* reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data);

    RouterContainerItem& currentContainer();

    void removePageInstance(AbstractRouterWidget* widget);
};