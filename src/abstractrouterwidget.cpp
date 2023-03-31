#include "abstractrouterwidget.h"

#include "qrouter.h"

AbstractRouterWidget::AbstractRouterWidget(const QVariant& data, QWidget* parent)
    : QWidget(parent)
    , navigateData(data)
    , firstShow(true)
{}

void AbstractRouterWidget::onNavigateResult(const QVariant&) {
}

bool AbstractRouterWidget::attemptClose() {
    return true;
}

QVariant AbstractRouterWidget::readAttemptCloseData() {
    return QVariant();
}

QVariant AbstractRouterWidget::onRouterEvent(const QString& event, const QVariant& data) {
    runRouterEvent(event, data);

    return executeRouterEvent(event, data);
}

bool AbstractRouterWidget::singletonInstance() {
    return false;
}

QVariant AbstractRouterWidget::getNavigateData(bool clear) {
    auto data = navigateData;
    if (clear) {
        navigateData = QVariant();
    }
    return data;
}

void AbstractRouterWidget::runRouterEvent(const QString&, const QVariant&) {
}

QVariant AbstractRouterWidget::executeRouterEvent(const QString&, const QVariant&) {
    return QVariant();
}

void AbstractRouterWidget::showEvent(QShowEvent*) {
    if (firstShow) {
        onViewLoad();
        firstShow = false;
    } else {
        onReshow();
    }
}

void AbstractRouterWidget::resizeEvent(QResizeEvent*) {
    onResizeWindow();
}

void AbstractRouterWidget::hideEvent(QHideEvent*) {
    onHidden();
}

bool AbstractRouterWidget::event(QEvent* event) {
    if (event->type() == QRouterPageEvent::type) {
        auto e = static_cast<QRouterPageEvent*>(event);
        runRouterEvent(e->event, e->data);
        return true;
    }
    return QWidget::event(event);
}


void AbstractRouterWidget::onViewLoad() {
}

void AbstractRouterWidget::onReshow() {
}

void AbstractRouterWidget::onHidden() {
}

void AbstractRouterWidget::onResizeWindow() {
}
