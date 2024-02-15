#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // QApplication::setFont(QFont(FONT_NAME, FONT_SIZE));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
