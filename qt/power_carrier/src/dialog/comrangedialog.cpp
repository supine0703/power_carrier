#include "comrangedialog.h"
#include "ui_comrangedialog.h"

#include "configurations.h"



ComRangeDialog::ComRangeDialog(QWidget* parent, int upperLimit)
    : QDialog(parent)
    , ui(new Ui::ComRangeDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    this->m_comMin = ui->minSpinBox->value();
    this->m_comMax = ui->maxSpinBox->value();
    if (SETTINGS_CONTAINS({_COM_RANGE_MIN_, _COM_RANGE_MAX_}))
    {
        auto min = SETTINGS().value(_COM_RANGE_MIN_).toInt();
        auto max = SETTINGS().value(_COM_RANGE_MAX_).toInt();
        if (0 < min &&  min <= max && max <= upperLimit)
        {
            this->m_comMin = min;
            this->m_comMax = max;
            this->ready();
            ui->minSpinBox->setMaximum(this->m_comMax);
            ui->maxSpinBox->setMinimum(this->m_comMin);
        }
    }
    else
    {
        SETTINGS().setValue(_COM_RANGE_MIN_, this->m_comMin);
        SETTINGS().setValue(_COM_RANGE_MAX_, this->m_comMax);
    }
    ui->maxSpinBox->setMaximum(upperLimit);
}


ComRangeDialog::~ComRangeDialog()
{
    delete ui;
}


int ComRangeDialog::comMin() const
{
    return this->m_comMin;
}


int ComRangeDialog::comMax() const
{
    return this->m_comMax;
}


void ComRangeDialog::ready() const
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
    SETTINGS().setValue(_COM_RANGE_MIN_, this->m_comMin);
    SETTINGS().setValue(_COM_RANGE_MAX_, this->m_comMax);
}

