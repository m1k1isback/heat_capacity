#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));

    MainWindow w;
    w.show();
    return a.exec();
}
