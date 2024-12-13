#include "CoolMark.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CoolMark w;
    w.show();
    return a.exec();
}
