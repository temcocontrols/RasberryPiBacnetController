#include <QApplication>
#include <QMetaType>

#include "VariablesDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VariablesDialog d;
    d.show();

    return a.exec();
}
