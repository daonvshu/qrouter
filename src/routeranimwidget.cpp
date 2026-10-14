#include "routeranimwidget.h"

#include <qpainter.h>

RouterAnimWidget::RouterAnimWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void RouterAnimWidget::setPixmap(const QPixmap& value) {
    pixmap = value;
    update();
}

void RouterAnimWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.drawPixmap(rect(), pixmap);
}
