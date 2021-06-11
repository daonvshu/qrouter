#include "qrouter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QRouter w;
    w.show();
    return a.exec();
}
