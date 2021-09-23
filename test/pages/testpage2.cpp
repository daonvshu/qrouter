#include "testpage2.h"

#include "qrouter.h"

#include "customdata.h"

#include <qdatetime.h>
#include <qmessagebox.h>

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

    connect(ui.btn_pop, &QPushButton::clicked, this, &TestPage2::popWithData);
}

bool TestPage2::attemptClose() {
    if (ui.block_pop->isChecked()) {
        QMessageBox::warning(0, "warning", "page close blocked!");
        return false;
    }
    return true;
}

QVariant TestPage2::readAttemptCloseData() {
    CustomData data;
    data.sendTime = QDateTime::currentMSecsSinceEpoch();
    data.message = "close page2";
    return QVariant::fromValue(data);
}

void TestPage2::runRouterEvent(const QString& event, const QVariant& data) {
    if (event == "eventtest") {
        QRouter::of().postEventToRoot("printevent", "test page2 receive event!");
    }
}

void TestPage2::popWithData() {
    if (ui.pop_data_extra->isChecked()) {
        auto d = readAttemptCloseData().value<CustomData>();
        d.message += " with extra";
        QRouter::of().pop(QVariant::fromValue(d));
    } else {
        QRouter::of().pop();
    }
}
