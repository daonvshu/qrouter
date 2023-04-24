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
        return QString();
    }
    return item->metaObject()->className();
}

void QRouter::initStack(const QList<QByteArray>& pages) {
    for (const auto& page: pages) {
        push(page);
    }
}

void QRouter::push(const QByteArray& pageClassName, const QVariant& data) {
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    auto& item = currentContainer();

    auto widget = reflectByName(pageClassName, item.container, data);
    item.stack.append(widget);

    item.container->addWidget(widget);
    item.container->setCurrentWidget(widget);
}

void QRouter::pushReplace(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainer();

    if (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();
    }

    push(pageClassName, data);
}

void QRouter::pushAndClear(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainer();

    while (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();
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
        return false;
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
            widget->deleteLater();

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
        widget->deleteLater();
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
        widget->deleteLater();
    }

    if (!item.stack.isEmpty()) {
        item.container->setCurrentWidget(item.stack.last());
    }
}


QVariant QRouter::sendEventCur(const QString& event, const QVariant& data) {
    auto& item = currentContainer();
    if (item.stack.isEmpty()) {
        return QVariant();
    }

    return item.stack.last()->onRouterEvent(event, data);
}

QVariant QRouter::sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data) {
    auto& item = currentContainer();

    for (const auto& page: item.stack) {
        if (page->metaObject()->className() == pageClassName) {
            return page->onRouterEvent(event, data);
        }
    }

    return QVariant();
}

void QRouter::sendEventAll(const QString& event, const QVariant& data) {
    auto& item = currentContainer();

    for (const auto& widget: item.stack) {
        widget->onRouterEvent(event, data);
    }
}

void QRouter::postEventCur(const QString& event, const QVariant& data) {
    auto& item = currentContainer();
    if (item.stack.isEmpty()) {
        return;
    }

    return qApp->postEvent(item.stack.last(), new QRouterPageEvent(event, data));
}

void QRouter::postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data) {
    auto& item = currentContainer();

    for (const auto& page : item.stack) {
        if (page->metaObject()->className() == pageClassName) {
            qApp->postEvent(page, new QRouterPageEvent(event, data));
            return;
        }
    }
}

void QRouter::postEventToRoot(const QString& event, const QVariant& data) {
    auto& item = currentContainer();

    qApp->postEvent(item.container->parent(), new QRouterPageEvent(event, data));
}

void QRouter::postEventAll(const QString& event, const QVariant& data) {
    auto& item = currentContainer();

    for (const auto& widget : item.stack) {
        qApp->postEvent(widget, new QRouterPageEvent(event, data));
    }
}

AbstractRouterWidget* QRouter::reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data) {
    int type = QMetaType::type(className + '*');
    auto metaObj = QMetaType::metaObjectForType(type);
    Q_ASSERT_X(metaObj != nullptr, "qrouter reflect page class", "cannot reflect by name!");

    auto obj = metaObj->newInstance(Q_ARG(QVariant, data), Q_ARG(QWidget*, parent));
    AbstractRouterWidget* widget = dynamic_cast<AbstractRouterWidget*>(obj);
    Q_ASSERT_X(widget != nullptr, "qrouter reflect page class", "cannot reflect by name!");

    return widget;
}

QRouter::RouterContainerItem& QRouter::currentContainer() {
    int id = m_curContextId.value(QThread::currentThread());
    Q_ASSERT_X(containers.contains(id), "get current containter", "cannot find context id!");

    return containers[id];
}

