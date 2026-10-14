#pragma once

#include <qpixmap.h>
#include <qwidget.h>

class RouterAnimWidget : public QWidget {
public:
    explicit RouterAnimWidget(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap pixmap;
};
