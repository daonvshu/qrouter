﻿#pragma once

#include <qobject.h>
#include <qhash.h>

#include "qrouterexception.h"
#include "qrouterpageevent.h"
#include "routerstackmanager.h"

#include "container/stackroutercontainer.h"
#include "container/layoutroutercontainer.h"

class QRouter {
public:
    /**
    * install a page stack container for 'contextId'
    * @param container page stack container
    * @param contextId page stack id
    * @return
    */
    static QRouter& install(AbstractRouterContainer* container, int contextId = 0);

    /**
     * Use the specified 'stackedWidget' to manage page stack
     * @param stackContainer page stack container
     * @param contextId page stack id
     * @return
     */
    static QRouter& install(QStackedWidget* stackContainer, int contextId = 0);

    /**
     * Use the specified 'layout'  to manage page stack
     * @param boxLayout page stack container
     * @param contextId page stack id
     * @return
     */
    static QRouter& install(QBoxLayout* boxLayout, int contextId = 0);

    /**
     * Get page stack manager by id
     * @param contextId
     * @return
     */
    static RouterStackManager& of(int contextId = 0);

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
     * @param container container instance
     * @return -1 if not register by 'install'
     */
    static int getIdByContainer(QWidget* container);

private:
    QRouter();

private:
    static QRouter router;

    QHash<int, RouterStackManager> managers;
};