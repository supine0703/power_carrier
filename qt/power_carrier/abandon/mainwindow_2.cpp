#include "mainwindow_2.h"
#include "configurations.h"
#include "serialportwidget.h"
#include "sqlitewidget.h"

#include "mainwidget.h"

#include <QApplication>
#include <QLayout>
#include <QMenuBar>
#include <QScreen>
#include <QToolBar>
#include <QFrame>
#include <QTableView>



MainWindow_2::MainWindow_2(QWidget* parent)
    : QMainWindow{parent}
    // , spWidget(new SerialPortWidget(this))
    // , leftToolBar(new QToolBar(this))
{


    // this->initCentreWidget();
    // this->initToolBar();
    // this->initMenuBar();

    // let window show in the center.
    auto Geometry = [this](int width, int height) {
        QSize screenSize = QApplication::primaryScreen()->size();
        int x = (screenSize.width() - width) >> 1;
        int y = (screenSize.height() - height) * 0.4;
        this->setGeometry(x, y, width, height);
    };

    auto w = new MainWidget(this);
    Geometry(w->width(), w->height());

    // Geometry(1200, 626);


    // this->loadSetting();
}

void MainWindow_2::initCentreWidget()
{
    this->setCentralWidget(centralWidget = new QWidget(this));
    centralLayout = new QHBoxLayout(centralWidget);
    centralLayout->setAlignment(Qt::AlignLeft);
    centralLayout->addWidget(spWidget);

    auto vLine = new QFrame(this);
    vLine->setFrameShape(QFrame::VLine);
    vLine->setFrameShadow(QFrame::Plain);
    centralLayout->addWidget(vLine);
    
    centralLayout->addWidget(new SQLiteWidget(this));
}

void MainWindow_2::initToolBar()
{
    this->addToolBar(Qt::LeftToolBarArea, leftToolBar);

    leftToolBar->setMovable(false);
    leftToolBar->setFloatable(false);
    leftToolBar->setOrientation(Qt::Vertical);
    leftToolBar->setAllowedAreas(Qt::LeftToolBarArea);
    leftToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);

    // auto spAssistant = new QAction(QChar(0xe6c2), this);
    // spAssistant->setFont(ICON_FONT());
    QFont iconfont = ICON_FONT();
    iconfont.setPixelSize(36);
    leftToolBar->setFont(iconfont);
    spAssistant = leftToolBar->addAction(QChar(0xe6c2));
    spAssistant->setCheckable(true);
    spAssistant->setToolTip("开启/关闭 串口辅助侧边栏");

    connect(spAssistant, &QAction::triggered, this, [this](bool visiable) {
        spWidget->setVisible(visiable);
        SETTINGS().setValue(_SP_ASSISTANT_HIDDEN_, !visiable);
    });
}

void MainWindow_2::initMenuBar()
{
}

void MainWindow_2::loadSetting()
{
    auto spVisiable = !SETTINGS().value(_SP_ASSISTANT_HIDDEN_).toBool();
    spAssistant->setChecked(spVisiable);
    spWidget->setVisible(spVisiable);
}
