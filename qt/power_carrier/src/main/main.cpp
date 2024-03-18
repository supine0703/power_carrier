// #include "mainwindow_2.h"
#include "mainwidget.h"

#include <QApplication>
#include <QFontDatabase>


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QString fontName = QString("%1,%2,%3").arg(
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/icon/res/iconfont/iconfont.ttf"
            )
        ).at(0),
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/font/res/fonts/JetBrainsMono-Medium.ttf"
            )
        ).at(0),
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/font/res/fonts/YouSheYuFeiTeJianKangTi-2.ttf"
            )
        ).at(0)
    );
    QApplication::setFont(QFont(fontName, 13));

    PROJECT_NAMESPACE_NAME::MainWidget w;
    w.show();

    return a.exec();
}
