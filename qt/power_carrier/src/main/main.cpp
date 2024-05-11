#include "mainwidget.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QString fontName = QString("%1,%2,%3").arg(
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/icon/iconfont/iconfont.ttf"
            )
        ).at(0),
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/font/font_family/JetBrainsMono-Medium.ttf"
            )
        ).at(0),
        QFontDatabase::applicationFontFamilies(
            QFontDatabase::addApplicationFont(
                ":/font/font_family/YouSheYuFeiTeJianKangTi-2.ttf"
            )
        ).at(0)
    );
    QApplication::setFont(QFont(fontName, 13));
    QApplication::setWindowIcon(QIcon(":/icon/img/AppIcon.ico"));

    MainWidget w;
    w.show();
    w.adjustDisplay();
    // AGSL ags(R"(E:\Project\.Github\power_carrier\qt\power_carrier\GAS\gas.txt)");
    // ags.scanner();
    // ags.parser();

    return a.exec();
}
