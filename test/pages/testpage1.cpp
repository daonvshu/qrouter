#include "testpage1.h"

#include "qrouter.h"

#include "customdata.h"

#include <qdatetime.h>

int TestPage1::typeId = qRegisterMetaType<TestPage1*>();

TestPage1::TestPage1(const QVariant& data, QWidget* parent)
    : AbstractRouterWidget(data, parent)
{
    ui.setupUi(this);

    pageStackId = data.toInt();

    connect(ui.btn_push, &QPushButton::clicked, [&] {
        CustomData data;
        data.sendTime = QDateTime::currentMSecsSinceEpoch();
        data.message = "send data from page1 to page2";
        data.pageStackId = pageStackId;
        QRouter::of(pageStackId).push("TestPage2", QVariant::fromValue(data));
    });
}

void TestPage1::onNavigateResult(const QVariant& data) {

    auto value = data.value<CustomData>();

    ui.data_from_pop->setText(
        QString("from %1 to %2, msg: <%3>")
        .arg(QDateTime::fromMSecsSinceEpoch(value.sendTime).toString("HH:mm:ss.zzz"))
        .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))
        .arg(value.message)
    );
}

void TestPage1::runRouterEvent(const QString& event, const QVariant& data) {
    if (event == "eventtest") {
        QRouter::of(pageStackId).postEventToRoot("printevent", "test page1 receive event!");
    }
}
