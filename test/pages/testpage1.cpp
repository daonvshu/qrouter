#include "testpage1.h"

#include "../../src/qrouter.h"

#include "customdata.h"

#include <qdatetime.h>

int TestPage1::typeId = qRegisterMetaType<TestPage1*>();

TestPage1::TestPage1(const QVariant& data, QWidget* parent)
    : AbstractRouterWidget(data, parent)
{
    ui.setupUi(this);

    connect(ui.btn_push, &QPushButton::clicked, [&] {
        CustomData data;
        data.sendTime = QDateTime::currentMSecsSinceEpoch();
        data.message = "send data from page1 to page2";
        QRouter::of().push("TestPage2", QVariant::fromValue(data));
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
