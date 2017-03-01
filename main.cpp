#include <QApplication>
#include <QMetaType>

#include "VariablesDialog.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    //VariablesDialog d;
    //d.show();

    return a.exec();
}
