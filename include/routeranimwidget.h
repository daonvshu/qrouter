#pragma once

#include <qpixmap.h>
#include <qwidget.h>

class RouterAnimWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit RouterAnimWidget(QWidget* parent = nullptr);

    static QPixmap capture(QWidget* widget, QPointF* drawOffset = nullptr);

    void setPixmap(const QPixmap& pixmap, const QPointF& drawOffset = {});
    qreal opacity() const;
    void setOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap pixmap;
    QPointF pixmapOffset;
    qreal currentOpacity = 1.0;
};
