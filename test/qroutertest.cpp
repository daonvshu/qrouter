#include "qroutertest.h"

#include "qrouter.h"

#include "pages/customdata.h"

#include "pages/testpage1.h"

#include <qdatetime.h>
#include <qdebug.h>

QRouterTest::QRouterTest(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.widget->installEventFilter(this);

    QRouter::install(ui.stackedWidget, 0);
    QRouter::install(ui.container_layout, 1);

    connect(ui.stackedWidget, &QStackedWidget::currentChanged, this, &QRouterTest::printPageStack);

    QRouter::of(0).push("TestPage1", 0);
    QRouter::of(1).push("TestPage1", 1);

    //test send event
    connect(ui.btn_send1, &QPushButton::clicked, [&] {
        if (ui.send_by_post->isChecked()) {
            auto ptr = QRouter::getInstance<TestPage1>();
            Q_ASSERT(ptr != nullptr);
            QRouter::of(0).postEventTo("TestPage1", "eventtest");
            QRouter::of(1).postEventTo("TestPage1", "eventtest");
        } else {
            QRouter::of(0).sendEventTo("TestPage1", "eventtest");
            QRouter::of(1).sendEventTo("TestPage1", "eventtest");
        }
    });

    connect(ui.btn_send_cur, &QPushButton::clicked, [&] {
        if (ui.send_by_post->isChecked()) {
            QRouter::of(0).postEventCur("eventtest");
            QRouter::of(1).postEventCur("eventtest");
        } else {
            QRouter::of(0).sendEventCur("eventtest");
            QRouter::of(1).sendEventCur("eventtest");
        }
    });

    connect(ui.btn_send_all, &QPushButton::clicked, [&] {
        if (ui.send_by_post->isChecked()) {
            QRouter::of(0).postEventAll("eventtest");
            QRouter::of(1).postEventAll("eventtest");
        } else {
            QRouter::of(0).sendEventAll("eventtest");
            QRouter::of(1).sendEventAll("eventtest");
        }
    });
}

void QRouterTest::printPageStack() {
    QString stackString;
    QDebug(&stackString) << QRouter::of().readStack();

    ui.log_view->append(QString("[%1] page stack: %2\n")
        .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"), stackString));
}

bool QRouterTest::event(QEvent* event) {
    if (event->type() == QRouterPageEvent::type) {
        ui.log_view->append(QString("[%1] receive router event: %2\n")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"),
                 dynamic_cast<QRouterPageEvent*>(event)->data.toString()));
        return true;
    }
    return QWidget::event(event);
}

bool QRouterTest::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui.widget) {
        if (event->type() == QRouterPageEvent::type) {
            ui.log_view->append(QString("[%1][layout base] receive router event: %2\n")
                                    .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"),
                                         dynamic_cast<QRouterPageEvent*>(event)->data.toString()));
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}
