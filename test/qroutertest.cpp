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

    QRouter::install(ui.stackedWidget, 0);

    connect(ui.stackedWidget, &QStackedWidget::currentChanged, this, &QRouterTest::printPageStack);

    QRouter::of().push("TestPage1");


    //test send event
    connect(ui.btn_send1, &QPushButton::clicked, [&] {
        if (ui.send_by_post->isChecked()) {
            auto ptr = QRouter::getInstance<TestPage1>();
            Q_ASSERT(ptr != nullptr);
            QRouter::of().postEventTo("TestPage1", "eventtest");
        } else {
            QRouter::of().sendEventTo("TestPage1", "eventtest");
        }
    });

    connect(ui.btn_send_cur, &QPushButton::clicked, [&] {
        if (ui.send_by_post->isChecked()) {
            QRouter::of().postEventCur("eventtest");
        } else {
            QRouter::of().sendEventCur("eventtest");
        }
    });

    connect(ui.btn_send_all, &QPushButton::clicked, [&] {
        if (ui.send_by_post->isChecked()) {
            QRouter::of().postEventAll("eventtest");
        } else {
            QRouter::of().sendEventAll("eventtest");
        }
    });
}

void QRouterTest::printPageStack() {
    QString stackString;
    QDebug(&stackString) << QRouter::of().readStack();

    ui.log_view->append(QString("[%1] page stack: %2\n")
        .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))
        .arg(stackString));
}

bool QRouterTest::event(QEvent* event) {
    if (event->type() == QRouterPageEvent::type) {
        ui.log_view->append(QString("[%1] receive router event: %2\n")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))
            .arg(static_cast<QRouterPageEvent*>(event)->data.toString()));
        return true;
    }
    return QWidget::event(event);
}
