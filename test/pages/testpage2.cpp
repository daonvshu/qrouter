#include "testpage2.h"

#include "../../src/qrouter.h"

#include "customdata.h"

#include <qdatetime.h>

int TestPage2::typeId = qRegisterMetaType<TestPage2*>();

TestPage2::TestPage2(const QVariant& data, QWidget* parent)
    : AbstractRouterWidget(data, parent) {
    ui.setupUi(this);

    auto value = data.value<CustomData>();

    ui.data_from_last_page->setText(
        QString("from %1 to %2, msg: <%3>")
        .arg(QDateTime::fromMSecsSinceEpoch(value.sendTime).toString("HH:mm:ss.zzz"))
        .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))
        .arg(value.message)
    );

    connect(ui.btn_pop, &QPushButton::clicked, [&] {
        CustomData data;
        data.sendTime = QDateTime::currentMSecsSinceEpoch();
        data.message = "close page2";
        QRouter::of().pop(QVariant::fromValue(data));
    });
}
