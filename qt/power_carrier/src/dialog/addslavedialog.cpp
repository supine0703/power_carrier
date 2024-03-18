#include "addslavedialog.h"
#include "ui_addslavedialog.h"

#include <QMessageBox>


AddSlaveDialog::AddSlaveDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::AddSlaveDialog)
    , regex(new QRegularExpression)
    , sure4s(2, true)
{
    ui->setupUi(this);
    ui->stateTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->stateTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);


    this->setFixedSize(this->width(), this->height());
}

AddSlaveDialog::~AddSlaveDialog()
{
    delete ui;
    delete regex;
}


void AddSlaveDialog::emitInfo(InfoMode mode)
{
    QStringList base;
    QStringList word;
    QStringList wtxt;
    base.append(ui->addrLineEdit->text());
    base.append(ui->typeLineEdit->text());
    base.append(ui->nameLineEdit->text());
    base.append(ui->memoPlainTextEdit->toPlainText());
    for (int i = 0, end = ui->stateTableWidget->rowCount(); i < end; i++)
    {
        word.append(ui->stateTableWidget->item(i, 0)->text());
        wtxt.append(ui->stateTableWidget->item(i, 1)->text());
    }
    switch (mode)
    {
    case InfoMode::Insert:
        emit insertInfo(base, word, wtxt);
        break;
    case InfoMode::Update:
        emit updateInfo(base, word, wtxt);
        break;
    }
}


void AddSlaveDialog::sureOk()
{
    auto ok(sure1 && sure2 && sure3 && sure4);
    if (ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled() != ok)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}


void AddSlaveDialog::createStateRow(int row)
{
    ui->stateTableWidget->insertRow(row);
    auto initItem = [this, row](int col) {
        sure4s.insert((row * 2) | col, false);
        auto _it = new QTableWidgetItem();
        _it->setTextAlignment(Qt::AlignCenter);
        _it->setFlags(
            Qt::ItemIsSelectable |
            Qt::ItemIsEnabled |
            Qt::ItemIsEditable
        );
        ui->stateTableWidget->setItem(row, col, _it);
        // return _it;
    };

    initItem(0);
    initItem(1);
}


int AddSlaveDialog::exec()
{
    this->on_resetPushButton_clicked();
    return QDialog::exec();
}


int AddSlaveDialog::exec(QStringList base, QStringList word, QStringList wtxt)
{
    this->on_resetPushButton_clicked();

    ui->addrLineEdit->setText(base[0]);
    ui->typeLineEdit->setText(base[1]);
    ui->nameLineEdit->setText(base[2]);
    ui->memoPlainTextEdit->setPlainText(base[3]);


    ui->stateTableWidget->removeRow(1);
    sure4s.clear();
    sure4s.resize(2, true);
    for (int i = 1, end = word.length(); i < end; i++)
    {
        this->createStateRow(i);
        ui->stateTableWidget->item(i, 0)->setText(word[i]);
        ui->stateTableWidget->item(i, 1)->setText(wtxt[i]);
    }

    this->on_addrLineEdit_editingFinished();
    this->on_typeLineEdit_editingFinished();
    this->on_nameLineEdit_editingFinished();
    for (int c = ui->stateTableWidget->columnCount() - 1; c >= 0; c--)
        for (int r = ui->stateTableWidget->rowCount() - 1; r > 0; r--)
            this->on_stateTableWidget_itemChanged(
                ui->stateTableWidget->item(r, c)
            );

    ui->addrLineEdit->setEnabled(false);
    auto exec = QDialog::exec();
    ui->addrLineEdit->setEnabled(true);
    return exec;
}


void AddSlaveDialog::on_addrLineEdit_editingFinished()
{
    regex->setPattern("^[0-9a-eA-E][0-9a-fA-F]?$");
    auto txt(ui->addrLineEdit->text());
    auto flg(regex->match(txt).hasMatch());
    ui->addrWarningLabel->setHidden(flg);
    if(flg && txt.length() == 1)
        ui->addrLineEdit->setText(QString("0%1").arg(txt));
    ui->addrLineEdit->setStyleSheet(flg ? "" : "color:red;");
    sure1 = flg;
    sureOk();
}


void AddSlaveDialog::on_typeLineEdit_editingFinished()
{
    regex->setPattern("^[0-9a-zA-Z]{1,2}$");
    auto flg(regex->match(ui->typeLineEdit->text()).hasMatch());
    ui->typeWarningLabel->setHidden(flg);
    ui->typeLineEdit->setStyleSheet(flg ? "" : "color:red;");
    sure2 = flg;
    sureOk();
}


void AddSlaveDialog::on_nameLineEdit_editingFinished()
{
    regex->setPattern("^[0-9a-zA-Z]{1,8}$");
    auto flg(regex->match(ui->nameLineEdit->text()).hasMatch());
    ui->nameWarningLabel->setHidden(flg);
    ui->nameLineEdit->setStyleSheet(flg ? "" : "color:red;");
    sure3 = flg;
    sureOk();
}


void AddSlaveDialog::on_stateTableWidget_itemChanged(QTableWidgetItem *item)
{
    Q_ASSERT(item->column() == 0 || item->column() == 1);
    static bool running = false;
    if (running)
        return;
    running = true;

    auto txt(item->text());
    bool flg;
    if (item->column() == 0)
    {
        regex->setPattern("^[0-9a-fA-F]{1,2}$");
        flg = regex->match(txt).hasMatch();
        if(flg)
        {
            item->setText(txt = txt.toUpper());
            if(txt.length() == 1)
                item->setText(txt = QString("0%1").arg(txt));

            auto its(ui->stateTableWidget->findItems(txt, Qt::MatchExactly));
            auto count = its.count();
            if (count > 1)
            {
                for (auto it : its)
                    if (it->column() != 0)
                        count--;
                if (count > 1)
                    flg = false;
            }
        }
    }
    else
    {
        regex->setPattern("^\\S{1,8}$");
        flg = regex->match(txt).hasMatch();
    }
    item->setBackground(flg ? Qt::transparent : QColor(255, 215, 255));
    sure4s[(item->row()*2) | item->column()] = sure4 = flg;
    if (sure4)
    {
        if (sure4s.length() <= 2)
            sure4 = false;
        else
            for (auto s : sure4s)
                if (!s)
                {
                    sure4 = false;
                    break;
                }
    }
    ui->stateWarningLabel->setHidden(sure4);
    sureOk();

    running = false;
}


void AddSlaveDialog::on_resetPushButton_clicked()
{
    sure1 = sure2 = sure3 = sure4 = false;
    sure4s.clear();
    sure4s.resize(2, true);

    ui->addrWarningLabel->setVisible(true);
    ui->typeWarningLabel->setVisible(true);
    ui->nameWarningLabel->setVisible(true);
    ui->stateWarningLabel->setVisible(true);

    ui->addrLineEdit->clear();
    ui->typeLineEdit->clear();
    ui->nameLineEdit->clear();
    ui->memoPlainTextEdit->clear();

    for (int i = ui->stateTableWidget->rowCount() - 1; i > 0; i--)
        ui->stateTableWidget->removeRow(i);
    this->createStateRow(1);
}


void AddSlaveDialog::on_spacePushButton_clicked()
{
    ui->stateTableWidget->clearSelection();
    this->on_addrLineEdit_editingFinished();
    this->on_typeLineEdit_editingFinished();
    this->on_nameLineEdit_editingFinished();
    for (int c = ui->stateTableWidget->columnCount() - 1; c >= 0; c--)
        for (int r = ui->stateTableWidget->rowCount() - 1; r > 0; r--)
            this->on_stateTableWidget_itemChanged(
                ui->stateTableWidget->item(r, c)
            );
}


void AddSlaveDialog::on_addRowPushButton_clicked()
{
    this->createStateRow(ui->stateTableWidget->rowCount());
}


void AddSlaveDialog::on_insertRowPushButton_clicked()
{
    if (ui->stateTableWidget->selectionModel()->hasSelection())
    {
        auto row(ui->stateTableWidget->currentRow());
        if (row > 0)
            this->createStateRow(row);
    }
}


void AddSlaveDialog::on_delRowPushButton_clicked()
{
    auto row = ui->stateTableWidget->currentRow();

    if (row == 0)
        QMessageBox::warning(this, "警告", "第1行是必选项，不可删除！");
    else if (ui->stateTableWidget->rowCount() == 2)
    {
        QMessageBox::warning(this, "警告", QString("不可删除，至少需要两项！"));
    }
    else if (
        QMessageBox::question(
            this, "提示",
            QString("请确认是否删除：\n第%1行 %2 - %3").arg(row + 1).arg(
                ui->stateTableWidget->item(row, 0)->text(),
                ui->stateTableWidget->item(row, 1)->text()
            )
        ) == QMessageBox::Yes)
    {
        ui->stateTableWidget->removeRow(row);
        sure4s.remove(row * 2, 2);
        on_spacePushButton_clicked();
    }
}

