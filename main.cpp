#include "serverwidget.h"
#include "clientwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerWidget sw;
    ClientWidget cw;

    sw.show();
    cw.show();

    return a.exec();
}
