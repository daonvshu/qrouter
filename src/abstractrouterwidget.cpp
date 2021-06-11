#include "abstractrouterwidget.h"

AbstractRouterWidget::AbstractRouterWidget(const QVariant& data, ImplPrivate& pd, QWidget* parent)
    : ImplClass(pd, parent)
    , navigateData(data)
    , firstShow(true)
{}

AbstractRouterWidget::AbstractRouterWidget(const QVariant& data, QWidget* parent)
    : AbstractRouterWidget(data, *new ImplPrivate, parent)
{}

void AbstractRouterWidget::onNavigateResult(const QVariant&) {
}

bool AbstractRouterWidget::attempClose() {
    return true;
}


QVariant AbstractRouterWidget::getNavigateData(bool clear) {
    auto data = navigateData;
    if (clear) {
        navigateData = QVariant();
    }
    return data;
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


void AbstractRouterWidget::onViewLoad() {
}

void AbstractRouterWidget::onReshow() {
}

void AbstractRouterWidget::onHidden() {
}

void AbstractRouterWidget::onResizeWindow() {
}
