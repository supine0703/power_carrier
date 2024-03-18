#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QPushButton>
#include <QMessageBox>
#include <QStatusBar>
#include <QPainter>
#include <QLabel>

#include "configurations.h"
#include "serialportwidget.h"
#include "sqlitewidget.h"





MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , statusBar(new QStatusBar(this))
    , spWidget(new SerialPortWidget(this))
    , sqlWidget(new SQLiteWidget(this))
{
    ui->setupUi(this);
    ui->mainLayout->addWidget(statusBar);
    ui->leftToolLayout->setAlignment(Qt::AlignTop);
    ui->leftFuncLayout->setAlignment(Qt::AlignLeft);

    initTopTool();


    setCentralWidget(sqlWidget);
    addLeftToolAndWidget(spWidget, QChar(0xe6c2), "开启/关闭 串口侧边栏");


    connect(sqlWidget, &SQLiteWidget::slaveStateChange,
            spWidget, &SerialPortWidget::slaveStateChange);

    connect(spWidget, &SerialPortWidget::slaveStateUpdate,
            sqlWidget, &SQLiteWidget::updateSlaveState);

    connect(spWidget, &SerialPortWidget::slaveNoState,
            sqlWidget, &SQLiteWidget::noSlaveState);

    loadSettings();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::initTopTool()
{
    auto fontToIcon = [](QChar code, int size, QColor color) {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);
        QFont iconFont(QApplication::font());
        iconFont.setPixelSize(size);

        QPainter painter(&pixmap);
        painter.setPen(color);
        painter.setFont(iconFont);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, QString(code));
        painter.end();

        return QIcon(pixmap);
    };

    iconList.append({
        fontToIcon(QChar(0xe615), 48, QColor(220, 0, 0, 255)),
        fontToIcon(QChar(0xe615), 48, QColor(85, 220, 0, 255)),
        fontToIcon(QChar(0xe615), 48, QColor(255, 170, 0, 255)),
        fontToIcon(QChar(0xe657), 48, Qt::black),
        fontToIcon(QChar(0xe603), 48, Qt::black),
        fontToIcon(QChar(0xe604), 48, Qt::black),
        fontToIcon(QChar(0xe632), 48, Qt::black),
        fontToIcon(QChar(0xe67b), 48, Qt::black),
    });


    ui->comToolButton->setIcon(iconList[0]);
    connect(
        ui->comToolButton, &QPushButton::clicked,
        spWidget, &SerialPortWidget::checkedForSerialPort
        );
    connect(spWidget, &SerialPortWidget::serialPortStatusChanged, this,
            [this] (int index) {
        ui->comToolButton->setIcon(iconList[index]);
    });
    connect(spWidget, &SerialPortWidget::selectComChanged, this,
            [this] (int index) {
        ui->comToolButton->setText(QString("COM%1").arg(index));
    });


    ui->createToolButton->setIcon(iconList[3]);
    connect(ui->createToolButton, &QPushButton::clicked,
            sqlWidget, &SQLiteWidget::createSQLiteDB);


    ui->openToolButton->setIcon(iconList[4]);
    connect(ui->openToolButton, &QPushButton::clicked,
            sqlWidget, &SQLiteWidget::openSQLiteDB);


    ui->saveToolButton->setIcon(iconList[5]);


    ui->saveAsToolButton->setIcon(iconList[6]);


    ui->exitToolButton->setIcon(iconList[7]);
    connect(ui->exitToolButton, &QPushButton::clicked, &QApplication::quit);
}

void MainWidget::loadSettings()
{
    auto leftToolIndex = SETTINGS().value(_LEFT_TOOL_INDEX_).toInt();
    if (leftToolIndex != -1)
    {
        auto b = qobject_cast<QPushButton*>(
            ui->leftToolLayout->itemAt(leftToolIndex)->widget()
        );
        auto w = ui->leftFuncLayout->itemAt(leftToolIndex)->widget();
        b->setChecked(true);
        w->setVisible(true);
    }

    ui->comToolButton->setText(
        QString("COM%1").arg(SETTINGS().value(_COM_PORT_NUM_).toInt())
    );
}


void MainWidget::setCentralWidget(QWidget* widget)
{
    ui->centralWidget->deleteLater();
    ui->centralLayout->removeWidget(ui->centralWidget);
    if (!widget)
        widget = new QWidget(this);
    ui->centralWidget = widget;
    ui->centralLayout->addWidget(widget);
}


void MainWidget::addLeftToolAndWidget(
    QWidget* widget,
    const QString& iconfont,
    const QString& toolTip
) {
    Q_ASSERT(widget != nullptr);

    auto button = new QPushButton(iconfont, this);
    button->setToolTip(toolTip);
    button->setCheckable(true);
    widget->setVisible(false);

    connect(button, &QPushButton::clicked, this, [this, button](bool checked) {
        // 实现按键互斥
        for (int ci = ui->leftToolLayout->count() - 1; ci >= 0; ci--) {
            auto b = qobject_cast<QPushButton*>(
                ui->leftToolLayout->itemAt(ci)->widget()
            );
            if (button == b) {
                SETTINGS().setValue(_LEFT_TOOL_INDEX_, b->isChecked() ? ci : -1);
            } else if (b->isChecked()) {
                auto w = ui->leftFuncLayout->itemAt(ci)->widget();
                w->setVisible(false);
                b->setChecked(false);
            }
        }
    });
    connect(button, &QPushButton::clicked, widget, &QWidget::setVisible);

    ui->leftToolLayout->addWidget(button);
    ui->leftFuncLayout->addWidget(widget);
}
