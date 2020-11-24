#include "mainwindow.h"

#include <QApplication>
#include "Language.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
     InitKeyInfomation();
    MainWindow w;
    w.show();
    return a.exec();
}
