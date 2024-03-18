#include "newdbfiledialog.h"
#include "ui_newdbfiledialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "configurations.h"





NewDBFileDialog::NewDBFileDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::NewDBFileDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
}

NewDBFileDialog::~NewDBFileDialog()
{
    delete ui;
}


const QString& NewDBFileDialog::filePath() const
{
    return f;
}


int NewDBFileDialog::exec()
{
    if (!ui->nameLineEdit->text().isEmpty())
        ui->nameLineEdit->setText("");
    else
        this->on_nameLineEdit_textChanged("");
    this->on_pushButton_3_clicked();
    return QDialog::exec();
}


void NewDBFileDialog::on_nameLineEdit_textChanged(const QString& arg1)
{
    static const QString invalidChars("\\/:*?\"<>|");
    bool invalid(false);
    for (QChar c : arg1)
        if (invalidChars.contains(c))
        {
            invalid = true;
            break;
        }

    if (fileNameInvalid ^ invalid)
    {
        fileNameInvalid = invalid;
        if (invalid)
            ui->nameLineEdit->setStyleSheet("color:red");
        else
            ui->nameLineEdit->setStyleSheet("");
    }
    auto bs = ui->buttonBox->buttons();
    bs[0]->setEnabled(!(invalid || arg1.isEmpty()));
}


void NewDBFileDialog::on_toolButton_clicked()
{
    auto openPath  = QFileDialog::getExistingDirectory(this, "选择目录", SHARE_PATH);
    if (!openPath.isEmpty())
        ui->pathLineEdit->setText(openPath);
}


void NewDBFileDialog::on_pushButton_1_clicked()
{
    SETTINGS().setValue(_DB_DEFAULT_PATH_, ui->pathLineEdit->text());
}


void NewDBFileDialog::on_pushButton_2_clicked()
{
    SETTINGS().setValue(_DB_DEFAULT_PATH_, DB_PATH);
}


void NewDBFileDialog::on_pushButton_3_clicked()
{
    ui->pathLineEdit->setText(SETTINGS().value(_DB_DEFAULT_PATH_).toString());
}


void NewDBFileDialog::on_buttonBox_accepted()
{
    auto path(ui->pathLineEdit->text());
    auto name(ui->nameLineEdit->text());
    if (QFileInfo(name).suffix() != "db")
        name += ".db";
    this->f = QString("%1/%2").arg(path, name);
}

