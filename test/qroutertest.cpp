#include "qroutertest.h"

#include "../src/qrouter.h"

#include "pages/customdata.h"

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

    });

    connect(ui.btn_send_cur, &QPushButton::clicked, [&] {

    });

    connect(ui.btn_send_all, &QPushButton::clicked, [&] {

    });
}

void QRouterTest::printPageStack() {
    QString stackString;
    QDebug(&stackString) << QRouter::of().readStack();

    ui.log_view->append(QString("[%1] page stack: %2\n").arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")).arg(stackString));
}
