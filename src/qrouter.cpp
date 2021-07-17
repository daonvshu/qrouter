#include "qrouter.h"

#include "abstractrouterwidget.h"

#include <qapplication.h>

QEvent::Type QRouterPageEvent::type = static_cast<QEvent::Type>(QEvent::registerEventType());

QRouter QRouter::router;

QRouter::QRouter()
    : m_curContextId(0)
{}

QRouter& QRouter::install(QStackedWidget* stackContainer, int contextId) {
    RouterContainerItem item;
    item.container = stackContainer;
    router.containers.insert(contextId, item);
    return router;
}

QRouter& QRouter::of(int contextId) {
    router.m_curContextId = contextId;
    return router;
}

QStringList QRouter::readStack() {
    QStringList stackNames;

    auto& item = currentContainter();
    for (const auto& i : item.stack) {
        stackNames << i->metaObject()->className();
    }
    return stackNames;
}

AbstractRouterWidget* QRouter::current() {
    auto& item = currentContainter();
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

void QRouter::push(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainter();

    auto widget = reflectByName(pageClassName, item.container, data);
    item.stack.append(widget);

    item.container->addWidget(widget);
    item.container->setCurrentWidget(widget);
}

void QRouter::pushReplace(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainter();

    if (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();
    }

    push(pageClassName, data);
}

void QRouter::pushAndClear(const QByteArray& pageClassName, const QVariant& data) {
    auto& item = currentContainter();

    while (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();
    }

    push(pageClassName, data);
}

void QRouter::pop(const QVariant& data) {
    auto& item = currentContainter();

    if (!item.stack.isEmpty()) {
        if (item.stack.last()->attempClose()) {
            auto widget = item.stack.takeLast();
            item.container->removeWidget(widget);
            widget->deleteLater();

            item.container->setCurrentWidget(item.stack.last());
            item.stack.last()->onNavigateResult(data);
        }
    }
}

void QRouter::popUntil(const QByteArray& untilName) {
    auto& item = currentContainter();

    while (!item.stack.isEmpty() && item.stack.last()->metaObject()->className() != untilName) {
        if (!item.stack.last()->attempClose()) {
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

QVariant QRouter::sendEventCur(const QString& event, const QVariant& data) {
    auto& item = currentContainter();
    if (item.stack.isEmpty()) {
        return QVariant();
    }

    return item.stack.last()->onRouterEvent(event, data);
}

QVariant QRouter::sendEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data) {
    auto& item = currentContainter();

    for (const auto& page: item.stack) {
        if (page->metaObject()->className() == pageClassName) {
            return page->onRouterEvent(event, data);
        }
    }

    return QVariant();
}

void QRouter::sendEventAll(const QString& event, const QVariant& data) {
    auto& item = currentContainter();

    for (const auto& widget: item.stack) {
        widget->onRouterEvent(event, data);
    }
}

void QRouter::postEventCur(const QString& event, const QVariant& data) {
    auto& item = currentContainter();
    if (item.stack.isEmpty()) {
        return;
    }

    QRouterPageEvent pageEvent(event, data);
    return qApp->postEvent(item.stack.last(), &pageEvent);
}

void QRouter::postEventTo(const QByteArray& pageClassName, const QString& event, const QVariant& data) {
    auto& item = currentContainter();

    QRouterPageEvent pageEvent(event, data);
    for (const auto& page : item.stack) {
        if (page->metaObject()->className() == pageClassName) {
            qApp->postEvent(page, &pageEvent);
            return;
        }
    }
}

void QRouter::postEventToRoot(const QString& event, const QVariant& data) {
    auto& item = currentContainter();

    QRouterPageEvent pageEvent(event, data);
    qApp->postEvent(item.container->parent(), &pageEvent);
}

void QRouter::postEventAll(const QString& event, const QVariant& data) {
    auto& item = currentContainter();

    QRouterPageEvent pageEvent(event, data);
    for (const auto& widget : item.stack) {
        qApp->postEvent(widget, &pageEvent);
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

QRouter::RouterContainerItem& QRouter::currentContainter() {
    Q_ASSERT_X(containers.contains(m_curContextId), "get current containter", "cannot find context id!");

    return containers[m_curContextId];
}

