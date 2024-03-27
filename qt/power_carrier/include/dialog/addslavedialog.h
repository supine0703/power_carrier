#ifndef ADDSLAVEDIALOG_H
#define ADDSLAVEDIALOG_H

#include <QDialog>

namespace Ui {
class AddSlaveDialog;
}

class QTableWidgetItem;


class AddSlaveDialog : public QDialog
{
    Q_OBJECT

public:
    enum InfoMode {
        Insert,
        Update
    };


public:
    explicit AddSlaveDialog(QWidget* parent = nullptr);
    ~AddSlaveDialog();

public:
    void emitInfo(InfoMode mode);

private:
    void sureOk();

    void createStateRow(int row);


public slots:
    int exec() override;

    int exec(QStringList base, QStringList word, QStringList wtxt);

private slots:
    void on_addrLineEdit_editingFinished();
    void on_typeLineEdit_editingFinished();
    void on_nameLineEdit_editingFinished();
    void on_stateTableWidget_itemChanged(QTableWidgetItem* item);

    void on_resetPushButton_clicked();
    void on_spacePushButton_clicked();

    void on_addRowPushButton_clicked();
    void on_insertRowPushButton_clicked();
    void on_delRowPushButton_clicked();

private:
    Ui::AddSlaveDialog* ui;

    QRegularExpression* regex;

    bool sure1 = false;
    bool sure2 = false;
    bool sure3 = false;
    bool sure4 = false;
    QList<bool> sure4s;


signals:
    void insertInfo(QStringList, QStringList, QStringList);
    void updateInfo(QStringList, QStringList, QStringList);
};

 

#endif // ADDSLAVEDIALOG_H
