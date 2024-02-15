#include "mainwindow.h"
#include "controlwidget.h"

#include <QScreen>
#include <QApplication>
#include <QMenuBar>


PROJECT_USING_NAMESPACE;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    this->initMenuBar();

    // let window show in the center.
    auto Geometry = [this](int width, int height) {
        QSize screenSize = QApplication::primaryScreen()->size();
        int x = (screenSize.width() - width) >> 1;
        int y = (screenSize.height() - height) / 3;
        this->setGeometry(x, y, width, height);
    };
    auto cW = new ControlWidget(this);
    this->setCentralWidget(cW);
    Geometry(cW->width(), cW->height());
}

void MainWindow::initMenuBar()
{
}
