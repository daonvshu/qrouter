#include "qrouter.h"

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

    auto& item = containers[m_curContextId];
    for (const auto& i : item.stack) {
        stackNames << i->metaObject()->className();
    }
    return stackNames;
}

void QRouter::push(const QByteArray& pageClassName, const QVariant& data) {
    Q_ASSERT_X(containers.contains(m_curContextId), "qrouter push", "cannot find context id!");

    auto& item = containers[m_curContextId];
    auto widget = reflectByName(pageClassName, item.container, data);
    item.stack.append(widget);

    item.container->addWidget(widget);
    item.container->setCurrentWidget(widget);
}

void QRouter::pushReplace(const QByteArray& pageClassName, const QVariant& data) {
    Q_ASSERT_X(containers.contains(m_curContextId), "qrouter push and replace", "cannot find context id!");

    auto& item = containers[m_curContextId];
    if (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();
    }

    push(pageClassName, data);
}

void QRouter::pop(const QVariant& data) {
    Q_ASSERT_X(containers.contains(m_curContextId), "qrouter pop", "cannot find context id!");

    auto& item = containers[m_curContextId];
    if (!item.stack.isEmpty()) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();

        item.container->setCurrentWidget(item.stack.last());
        item.stack.last()->onNavigateResult(data);
    }
}

void QRouter::popUntil(const QByteArray& untilName) {
    Q_ASSERT_X(containers.contains(m_curContextId), "qrouter pop until", "cannot find context id!");

    auto& item = containers[m_curContextId];
    while (!item.stack.isEmpty() && item.stack.last()->metaObject()->className() != untilName) {
        auto widget = item.stack.takeLast();
        item.container->removeWidget(widget);
        widget->deleteLater();
    }

    if (!item.stack.isEmpty()) {
        item.container->setCurrentWidget(item.stack.last());
    }
}

AbstractRouterWidget* QRouter::reflectByName(const QByteArray& className, QWidget* parent, const QVariant& data) {
    int type = QMetaType::type(className + '*');
    auto metaObj = QMetaType::metaObjectForType(type);

    auto obj = metaObj->newInstance(Q_ARG(QVariant, data), Q_ARG(QWidget*, parent));
    AbstractRouterWidget* widget = dynamic_cast<AbstractRouterWidget*>(obj);
    Q_ASSERT_X(widget != nullptr, "qrouter reflect page class", "cannot reflect by name!");

    return widget;
}
