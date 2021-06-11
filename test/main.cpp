#include "qroutertest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QRouterTest w;
    w.show();
    return a.exec();
}
