#include "qrouter.h"

QRouter QRouter::router;

QRouter::QRouter()
{}

QRouter& QRouter::install(QStackedWidget* stackContainer, int contextId) {
    RouterStackManager manager;
    manager.container = stackContainer;
    router.managers.insert(contextId, manager);
    return router;
}

RouterStackManager& QRouter::of(int contextId) {
    if (!router.managers.contains(contextId)) {
        throw QRouterRuntimeException(QString("cannot find container with context id:%1, the container may not be installed.").arg(contextId));
    }
    return router.managers[contextId];
}

int QRouter::getIdByContainer(QStackedWidget *container) {
    for (auto i = router.managers.begin(); i != router.managers.end(); ++i) {
        if (i.value().container == container) {
            return i.key();
        }
    }
    return -1;
}