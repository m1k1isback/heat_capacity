#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    // Устанавливаем современный стиль
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    // Устанавливаем локаль
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));

    MainWindow w;
    w.show();

    return a.exec();
}
