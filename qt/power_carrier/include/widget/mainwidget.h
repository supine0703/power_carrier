#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

namespace Ui {
class MainWidget;
}

class QStatusBar;
class SerialPortWidget;
class SQLiteWidget;
class WaitTableWidget;


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget* parent = nullptr);
    ~MainWidget();

    void adjustDisplay();

private:
    void initTopTool();

    void loadSettings();

    void setCentralWidget(QWidget* widget);

    void addLeftToolAndWidget(
        QWidget* widget,
        const QString& iconfont,
        const QString& toolTip = ""
    );


private:
    Ui::MainWidget* ui;
    QStatusBar* statusBar;

    QList<QIcon> iconList;

    SerialPortWidget* spWidget;
    SQLiteWidget* sqlWidget;
    WaitTableWidget* wtWidget;
};

 

#endif // MAINWIDGET_H
