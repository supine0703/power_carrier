#include "comrangedialog.h"
#include "ui_comrangedialog.h"

#include "configurations.h"

PROJECT_USING_NAMESPACE;

ComRangeDialog::ComRangeDialog(QWidget *parent, int upperLimit)
    : QDialog(parent)
    , ui(new Ui::ComRangeDialog)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());
    this->m_comMin = CFGS().value(_COM_RANGE_MIN_).toInt();
    this->m_comMax = CFGS().value(_COM_RANGE_MAX_).toInt();
    ui->minSpinBox->setValue(m_comMin);
    ui->minSpinBox->setMaximum(m_comMax);
    ui->maxSpinBox->setValue(m_comMax);
    ui->maxSpinBox->setMinimum(m_comMin);
    ui->maxSpinBox->setMaximum(upperLimit);
}

ComRangeDialog::~ComRangeDialog()
{
    delete ui;
}

int ComRangeDialog::comMin()
{
    return this->m_comMin;
}

int ComRangeDialog::comMax()
{
    return this->m_comMax;
}

void power_carrier::ComRangeDialog::ready()
{
    ui->minSpinBox->setValue(this->m_comMin);
    ui->maxSpinBox->setValue(this->m_comMax);
}

void ComRangeDialog::on_minSpinBox_valueChanged(int arg1)
{
    ui->maxSpinBox->setMinimum(arg1);
}


void ComRangeDialog::on_maxSpinBox_valueChanged(int arg1)
{
    ui->minSpinBox->setMaximum(arg1);
}


void ComRangeDialog::on_buttonBox_accepted()
{
    this->m_comMin = ui->minSpinBox->value();
    this->m_comMax = ui->maxSpinBox->value();
    CFGS().setValue(_COM_RANGE_MIN_, this->m_comMin);
    CFGS().setValue(_COM_RANGE_MAX_, this->m_comMax);
    emit comRangeChanged(this->m_comMin, this->m_comMax);
}

