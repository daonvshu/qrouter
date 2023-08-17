#include "qrouter.h"

#include "abstractrouterwidget.h"

#include <qapplication.h>

QEvent::Type QRouterPageEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

QRouter QRouter::router;

QRouter::QRouter()
{}

QRouter& QRouter::install(QStackedWidget* stackContainer, int contextId) {
    RouterContainerItem item;
    item.container = stackContainer;
    router.containers.insert(contextId, item);
    return router;
}

QRouter& QRouter::of(int contextId) {
    router.m_curContextId.insert(QThread::currentThread(), contextId);
    return router;
}

QStringList QRouter::readStack() {
    QStringList stackNames;

    auto& item = currentContainer();
    for (const auto& i : item.stack) {
        stackNames << i->metaObject()->className();
    }
    return stackNames;
}

AbstractRouterWidget* QRouter::current() {
    auto& item = currentContainer();
    if (item.stack.isEmpty()) {
        return nullptr;
    }
    return item.stack.last();
}

QString QRouter::currentName() {
    auto item = current();
    if (item == nullptr) {
        return {};
    }
    return item->metaObject()->className();
}

AbstractRouterWidget *QRouter::getInstanceFromStack(const QByteArray &pageClassName) {
    auto& item = currentContainer();

    AbstractRouterWidget* widgetTag = nullptr;
    for (int i=0; i<item.stack.size(); i++) {
        if (item.stack.at(i)->metaObject()->className() == pageClassName) {
            widgetTag = item.stack.value(i);
            break;
        }
    }

    if (widgetTag == nullptr) {
        if (keepSingletonPageInstance.contains(pageClassName)) {
            widgetTag = keepSingletonPageInstance.value(pageClassName);
        }
    }

    return widgetTag;
}

int QRouter::getIdByContainer(QStackedWidget *container) {
    for (auto i = router.containers.begin(); i != router.containers.end(); ++i) {
        if (i->container == container) {
            return i.key();
        }
    }
    return -1;
}

void QRouter::initStack(const QList<QByteArray>& pages) {
    if (pages.isEmpty()) {
        return;
    }
    auto& item = currentContainer();
    for (const auto& page: pages) {
        try {
            auto widget = reflectByName(page, item.container, {});
            item.stack.append(widget);
            item.container->addWidget(widget);
        } catch (QRouterRuntimeException& e) {
            qFatal(e.message.toLatin1().data());
        }
    }
    item.container->setCurrentWidget(item.stack.last());
}

void QRouter::push(const QByteArray& pageClassName, const QVariant& data) {
    if (QThread::currentThread() != qApp->thread()) {
        throw QRouterRuntimeException("cannot push a page in work thread!");
    }

    auto& item = currentContainer();

    AbstractRouterWidget* widget;
    if (keepSingletonPageInstance.contains(pageClassName)) {
        widget = keepSingletonPageInstance.take(pageClassName);
    } else {
        for (auto p : item.stack) {
            if (p->metaObject()->className() == pageClassName) {
                if (p->singletonInstance()) {
                    move2Top(pageClassName);
                    return;
                }
            }
        }
        try {
            widget = reflectByName(pageClassName, item.container, data);
        } catch (QRouterRuntimeException& e) {
            qFatal(e.message.toLatin1().data());
        }
    }
    item.stack.append(widget);

    item.container->addWidget(widget);
    item.container->setCurrentWidget(widget);
}

void QRouter::pushReplace(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainer();

    if (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        removePageInstance(widget);
    }

    push(pageClassName, data);
}

bool QRouter::pushOrMove2Top(const QByteArray &pageClassName, const QVariant &data) {
    if (move2Top(pageClassName)) {
        return true;
    }
    push(pageClassName, data);
    return false;
}

void QRouter::popAndPush(const QByteArray &pageClassName, const QVariant& data) {
    auto& item = currentContainer();

    if (!item.stack.isEmpty()) {
        if (item.stack.last()->attemptClose()) {
            auto widget = item.stack.takeLast();
            item.container->removeWidget(widget);
            removePageInstance(widget);
        } else {
            return;
        }
    }

    push(pageClassName, data);
}

void QRouter::close(const QByteArray &pageClassName) {
    auto& item = currentContainer();

    bool isLast = false;
    for (int i=0; i<item.stack.size(); i++) {
        if (item.stack.at(i)->metaObject()->className() == pageClassName) {
            isLast = i == item.stack.size() - 1;
            auto widget = item.stack.takeAt(i);
            item.container->removeWidget(widget);
            removePageInstance(widget);
            break;
        }
    }

    if (isLast) {
        item.container->setCurrentWidget(item.stack.last());
    }
}

void QRouter::pushAndClear(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainer();

    while (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        removePageInstance(widget);
    }

    push(pageClassName, data);
}

bool QRouter::move2Top(const QByteArray& pageClassName) {
    auto& item = currentContainer();

    AbstractRouterWidget* widgetTag = nullptr;
    for (int i=0; i<item.stack.size(); i++) {
        if (item.stack.at(i)->metaObject()->className() == pageClassName) {
            widgetTag = item.stack.takeAt(i);
            break;
        }
    }

    if (widgetTag == nullptr) {
        if (keepSingletonPageInstance.contains(pageClassName)) {
            widgetTag = keepSingletonPageInstance.take(pageClassName);
            item.container->addWidget(widgetTag);
        } else {
            return false;
        }
    }

    item.stack.append(widgetTag);
    item.container->setCurrentWidget(widgetTag);

    return true;
}

void QRouter::pop(QVariant data) {
    auto& item = currentContainer();

    if (!item.stack.isEmpty()) {
        if (item.stack.last()->attemptClose()) {
            auto widget = item.stack.takeLast();
            if (data.isNull()) {
                data = widget->readAttemptCloseData();
            }
            item.container->removeWidget(widget);
            removePageInstance(widget);

            item.container->setCurrentWidget(item.stack.last());
            item.stack.last()->onNavigateResult(data);
        }
    }
}

void QRouter::popUntil(const QByteArray& untilName) {
    auto& item = currentContainer();

    while (!item.stack.isEmpty() && item.stack.last()->metaObject()->className() != untilName) {
        if (!item.stack.last()->attemptClose()) {
            item.container->setCurrentWidget(item.stack.last());
            return;
        }
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        removePageInstance(widget);
    }

    if (!item.stack.isEmpty()) {
        item.container->setCurrentWidget(item.stack.last());
    }
}

void QRouter::popUntil(int stackSize) {
    auto& item = currentContainer();

    while (item.stack.size() > stackSize) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        removePageInstance(widget);
    }

    if (!item.stack.isEmpty()) {
        item.container->setCurrentWidget(item.stack.last());
    }
}


QVariant QRouter::sendEventCur(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();
        if (item.stack.isEmpty()) {
            return {};
        }
        return item.stack.last()->onRouterEvent(event, data);
    } catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }
    return {};
}

QVariant QRouter::sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();

        for (const auto &page: item.stack) {
            if (page->metaObject()->className() == pageClassName) {
                return page->onRouterEvent(event, data);
            }
        }
    } catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }

    if (keepSingletonPageInstance.contains(pageClassName)) {
        return keepSingletonPageInstance[pageClassName]->onRouterEvent(event, data);
    }

    return {};
}

void QRouter::sendEventAll(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();

        for (const auto &widget: item.stack) {
            widget->onRouterEvent(event, data);
        }
    }  catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }

    for (auto i = keepSingletonPageInstance.begin(); i != keepSingletonPageInstance.end(); ++i) {
        i.value()->onRouterEvent(event, data);
    }
}

void QRouter::postEventCur(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();
        if (item.stack.isEmpty()) {
            return;
        }
        return qApp->postEvent(item.stack.last(), new QRouterPageEvent(event, data));
    }  catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }
}

void QRouter::postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();

        for (const auto &page: item.stack) {
            if (page->metaObject()->className() == pageClassName) {
                qApp->postEvent(page, new QRouterPageEvent(event, data));
                return;
            }
        }
    }  catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }

    if (keepSingletonPageInstance.contains(pageClassName)) {
        qApp->postEvent(keepSingletonPageInstance[pageClassName], new QRouterPageEvent(event, data));
    }
}

void QRouter::postEventToRoot(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();
        qApp->postEvent(item.container->parent(), new QRouterPageEvent(event, data));
    } catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }
}

void QRouter::postEventAll(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        auto &item = currentContainer();

        for (const auto &widget: item.stack) {
            qApp->postEvent(widget, new QRouterPageEvent(event, data));
        }
    } catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }

    for (auto i = keepSingletonPageInstance.begin(); i != keepSingletonPageInstance.end(); ++i) {
        qApp->postEvent(i.value(), new QRouterPageEvent(event, data));
    }
}

AbstractRouterWidget* QRouter::reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data) {
#if QT_VERSION_MAJOR >= 6
    const auto metaObj = QMetaType::fromName(className + '*').metaObject();
#else
    int type = QMetaType::type(className + '*');
    const auto metaObj = QMetaType::metaObjectForType(type);
#endif
    if (metaObj == nullptr) {
        throw QRouterRuntimeException(QString("cannot find page:'%1', the page class may not register by call 'qRegisterMetaType<%1*>()'.").arg(QString(className)));
    }

    auto obj = metaObj->newInstance(Q_ARG(QVariant, data), Q_ARG(QWidget*, parent));
    auto widget = dynamic_cast<AbstractRouterWidget*>(obj);
    if (widget == nullptr) {
        throw QRouterRuntimeException(QString("cannot create instance of page:'%1', the page class constructor may not be assigned of 'Q_INVOKABLE'.").arg(QString(className)));
    }

    return widget;
}

QRouter::RouterContainerItem& QRouter::currentContainer() {
    int id = m_curContextId.value(QThread::currentThread());

    if (!containers.contains(id)) {
        throw QRouterRuntimeException(QString("cannot find container with context id:%1, the container may not be installed.").arg(id));
    }

    return containers[id];
}

void QRouter::removePageInstance(AbstractRouterWidget *widget) {
    if (widget->singletonInstance()) {
        keepSingletonPageInstance.insert(widget->metaObject()->className(), widget);
    } else {
        widget->deleteLater();
    }
}

