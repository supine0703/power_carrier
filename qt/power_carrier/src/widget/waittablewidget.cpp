#include "waittablewidget.h"
#include "ui_waittablewidget.h"

WaitTableWidget::WaitTableWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WaitTableWidget)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->
        setSectionResizeMode(QHeaderView::ResizeToContents);
}

WaitTableWidget::~WaitTableWidget()
{
    delete ui;
}

void WaitTableWidget::append(const QString& txt)
{
    ui->tableWidget->insertRow(0);
    auto _it = new QTableWidgetItem(txt);
    _it->setTextAlignment(Qt::AlignCenter);
    _it->setFlags(Qt::ItemIsSelectable |Qt::ItemIsEnabled);
    ui->tableWidget->setItem(0, 0, _it);
}

void WaitTableWidget::remove(int row)
{
    ui->tableWidget->removeRow(ui->tableWidget->rowCount() - row - 1);
}
