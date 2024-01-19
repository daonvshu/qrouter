#include "routerstackmanager.h"

#include "abstractrouterwidget.h"
#include "qrouterexception.h"
#include "qrouterpageevent.h"

#include <qcoreapplication.h>

QEvent::Type QRouterPageEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

QStringList RouterStackManager::readStack() {
    QStringList stackNames;

    for (const auto& i : stack) {
        stackNames << i->metaObject()->className();
    }
    return stackNames;
}

AbstractRouterWidget* RouterStackManager::current() {
    if (stack.isEmpty()) {
        return nullptr;
    }
    return stack.last();
}

QList<AbstractRouterWidget *> RouterStackManager::currentInstances() {
    return stack;
}

QString RouterStackManager::currentName() {
    auto item = current();
    if (item == nullptr) {
        return {};
    }
    return item->metaObject()->className();
}

AbstractRouterWidget *RouterStackManager::getInstanceFromStack(const QByteArray &pageClassName) {

    AbstractRouterWidget* widgetTag = nullptr;
    for (int i=0; i<stack.size(); i++) {
        if (stack.at(i)->metaObject()->className() == pageClassName) {
            widgetTag = stack.value(i);
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

AbstractRouterWidget *RouterStackManager::getInstanceFromStack(int stackIndex) {
    if (stackIndex < 0 || stackIndex >= stack.size()) {
        return nullptr;
    }
    return stack[stackIndex];
}

void RouterStackManager::initStack(const QList<QByteArray>& pages) {
    if (pages.isEmpty()) {
        return;
    }
    for (const auto& page: pages) {
        try {
            stack.append(reflectByName(page, container->childPageParent(), {}));
            container->setCurrentWidget(stack.last());
        } catch (QRouterRuntimeException& e) {
            qFatal(e.message.toLatin1().data());
        }
    }
}

void RouterStackManager::push(const QByteArray& pageClassName, const QVariant& data) {
    AbstractRouterWidget* widget;
    if (keepSingletonPageInstance.contains(pageClassName)) {
        widget = keepSingletonPageInstance.take(pageClassName);
    } else {
        for (auto p : stack) {
            if (p->metaObject()->className() == pageClassName) {
                if (p->singletonInstance()) {
                    move2Top(pageClassName);
                    return;
                }
            }
        }
        try {
            widget = reflectByName(pageClassName, container->childPageParent(), data);
        } catch (QRouterRuntimeException& e) {
            qFatal(e.message.toLatin1().data());
        }
    }
    stack.append(widget);
    container->setCurrentWidget(widget);
}

void RouterStackManager::pushReplace(const QByteArray& pageClassName, const QVariant& data) {
    if (!stack.isEmpty()) {
        auto widget = stack.takeLast();
        container->removeWidget(widget);
        removePageInstance(widget);
    }

    push(pageClassName, data);
}

bool RouterStackManager::pushOrMove2Top(const QByteArray &pageClassName, const QVariant &data) {
    if (move2Top(pageClassName)) {
        return true;
    }
    push(pageClassName, data);
    return false;
}

void RouterStackManager::popAndPush(const QByteArray &pageClassName, const QVariant& data) {
    if (!stack.isEmpty()) {
        if (stack.last()->attemptClose()) {
            auto widget = stack.takeLast();
            container->removeWidget(widget);
            removePageInstance(widget);
        } else {
            return;
        }
    }

    push(pageClassName, data);
}

void RouterStackManager::close(const QByteArray &pageClassName) {
    bool isLast = false;
    for (int i=0; i<stack.size(); i++) {
        if (stack.at(i)->metaObject()->className() == pageClassName) {
            isLast = i == stack.size() - 1;
            auto widget = stack.takeAt(i);
            container->removeWidget(widget);
            removePageInstance(widget);
            break;
        }
    }

    if (isLast && !stack.isEmpty()) {
        container->setCurrentWidget(stack.last());
    }
}

void RouterStackManager::pushAndClear(const QByteArray& pageClassName, const QVariant& data) {
    while (!stack.isEmpty()) {
        auto widget = stack.takeLast();
        container->removeWidget(widget);
        removePageInstance(widget);
    }

    push(pageClassName, data);
}

bool RouterStackManager::move2Top(const QByteArray& pageClassName) {
    AbstractRouterWidget* widgetTag = nullptr;
    for (int i=0; i<stack.size(); i++) {
        if (stack.at(i)->metaObject()->className() == pageClassName) {
            widgetTag = stack.takeAt(i);
            break;
        }
    }

    if (widgetTag == nullptr) {
        if (keepSingletonPageInstance.contains(pageClassName)) {
            widgetTag = keepSingletonPageInstance.take(pageClassName);
        } else {
            return false;
        }
    }

    stack.append(widgetTag);
    container->setCurrentWidget(widgetTag);

    return true;
}

void RouterStackManager::pop(QVariant data) {
    if (!stack.isEmpty()) {
        if (stack.last()->attemptClose()) {
            auto widget = stack.takeLast();
            if (data.isNull()) {
                data = widget->readAttemptCloseData();
            }
            container->removeWidget(widget);
            removePageInstance(widget);

            container->setCurrentWidget(stack.last());
            stack.last()->onNavigateResult(data);
        }
    }
}

void RouterStackManager::popUntil(const QByteArray& untilName) {
    while (!stack.isEmpty() && stack.last()->metaObject()->className() != untilName) {
        if (!stack.last()->attemptClose()) {
            container->setCurrentWidget(stack.last());
            return;
        }
        auto widget = stack.takeLast();
        container->removeWidget(widget);
        removePageInstance(widget);
    }

    if (!stack.isEmpty()) {
        container->setCurrentWidget(stack.last());
    }
}

void RouterStackManager::popUntil(int stackSize) {
    while (stack.size() > stackSize) {
        auto widget = stack.takeLast();
        container->removeWidget(widget);
        removePageInstance(widget);
    }

    if (!stack.isEmpty()) {
        container->setCurrentWidget(stack.last());
    }
}


QVariant RouterStackManager::sendEventCur(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        if (stack.isEmpty()) {
            return {};
        }
        return stack.last()->onRouterEvent(event, data);
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

QVariant RouterStackManager::sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        for (const auto &page: stack) {
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

void RouterStackManager::sendEventAll(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        for (const auto &widget: stack) {
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

void RouterStackManager::postEventCur(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        if (stack.isEmpty()) {
            return;
        }
        return qApp->postEvent(stack.last(), new QRouterPageEvent(event, data));
    }  catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }
}

void RouterStackManager::postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        for (const auto &page: stack) {
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

void RouterStackManager::postEventToRoot(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        qApp->postEvent(container->rootPageEventReceiver(), new QRouterPageEvent(event, data));
    } catch (QRouterRuntimeException& e) {
        if (!ignoreContainerNotInstalled) {
#ifdef QT_DEBUG
            qFatal(e.message.toLatin1().data());
#endif
            throw e;
        }
    }
}

void RouterStackManager::postEventAll(const QString& event, const QVariant& data, bool ignoreContainerNotInstalled) {
    try {
        for (const auto &widget: stack) {
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

AbstractRouterWidget* RouterStackManager::reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data) {
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

void RouterStackManager::removePageInstance(AbstractRouterWidget *widget) {
    if (widget->singletonInstance()) {
        keepSingletonPageInstance.insert(widget->metaObject()->className(), widget);
    } else {
        widget->deleteLater();
    }
}