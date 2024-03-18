#ifndef MAINWINDOW_2_H
#define MAINWINDOW_2_H

#include <QMainWindow>

PROJECT_BEGIN_NAMESPACE

class SerialPortWidget;

class MainWindow_2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_2(QWidget* parent = nullptr);

private:
    void initCentreWidget();
    void initToolBar();
    void initMenuBar();

    void loadSetting();

private:
    QWidget* centralWidget;
    QLayout* centralLayout;

    QToolBar* leftToolBar;
    QAction* spAssistant;

    SerialPortWidget* spWidget;

signals:
};

PROJECT_END_NAMESPACE

#endif // MAINWINDOW_2_H
