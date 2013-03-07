#include "imgproc.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    imgproc w;
    w.showMaximized();
    return a.exec();
}
