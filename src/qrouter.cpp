#include "qrouter.h"

QRouter QRouter::router;

QRouter::QRouter()
{}

QRouter &QRouter::install(AbstractRouterContainer *container, int contextId) {
    RouterStackManager manager;
    manager.container = container;
    router.managers.insert(contextId, manager);
    return router;
}

QRouter& QRouter::install(QStackedWidget* stackContainer, int contextId) {
    return install(new StackRouterContainer(stackContainer), contextId);
}

QRouter &QRouter::install(QBoxLayout *boxLayout, int contextId) {
    return install(new LayoutRouterContainer(boxLayout), contextId);
}

RouterStackManager& QRouter::of(int contextId) {
    if (!router.managers.contains(contextId)) {
        throw QRouterRuntimeException(QString("cannot find container with context id:%1, the container may not be installed.").arg(contextId));
    }
    return router.managers[contextId];
}

int QRouter::getIdByContainer(QWidget *container) {
    for (auto i = router.managers.begin(); i != router.managers.end(); ++i) {
        if (i.value().container->parent() == container) {
            return i.key();
        }
    }
    return -1;
}