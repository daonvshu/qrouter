#include "routeranimwidget.h"

#include <qpainter.h>
#include <qlayout.h>
#include <qmath.h>

namespace {
void activateLayouts(QWidget* widget) {
    widget->ensurePolished();
    if (auto* layout = widget->layout()) {
        layout->activate();
    }
    const auto children = widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (auto* child : children) {
        if (!child->isHidden()) {
            activateLayouts(child);
        }
    }
}
}

RouterAnimWidget::RouterAnimWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

QPixmap RouterAnimWidget::capture(QWidget* widget, QPointF* drawOffset) {
    activateLayouts(widget);
    if (widget->size().isEmpty()) {
        return {};
    }

    const qreal dpr = widget->devicePixelRatioF();
    const QPoint origin = widget->mapTo(widget->window(), QPoint());
    const QPointF deviceOrigin(origin.x() * dpr, origin.y() * dpr);
    // Preserve window-relative pixel alignment so snapshot text stays on the same raster grid.
    const QPointF shift = (deviceOrigin - QPointF(qFloor(deviceOrigin.x()), qFloor(deviceOrigin.y()))) / dpr;
    QPixmap snapshot(QSize(qCeil((widget->width() + shift.x()) * dpr),
                           qCeil((widget->height() + shift.y()) * dpr)));
    snapshot.setDevicePixelRatio(dpr);
    snapshot.fill(Qt::transparent);

    QPainter painter(&snapshot);
    painter.translate(shift);
    widget->render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
    if (drawOffset) {
        *drawOffset = -shift;
    }
    return snapshot;
}

void RouterAnimWidget::setPixmap(const QPixmap& value, const QPointF& drawOffset) {
    pixmap = value;
    pixmapOffset = drawOffset;
    update();
}

qreal RouterAnimWidget::opacity() const {
    return currentOpacity;
}

void RouterAnimWidget::setOpacity(qreal value) {
    currentOpacity = qBound(qreal(0.0), value, qreal(1.0));
    update();
}

void RouterAnimWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setOpacity(currentOpacity);
    painter.drawPixmap(pixmapOffset, pixmap);
}
