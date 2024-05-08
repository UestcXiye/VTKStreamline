#include "VTKStreamline.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VTKStreamline w;
    w.show();
    return a.exec();
}
