#ifndef NEWDBFILEDIALOG_H
#define NEWDBFILEDIALOG_H

#include <QDialog>

namespace Ui {
class NewDBFileDialog;
}

class NewDBFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDBFileDialog(QWidget* parent = nullptr);
    ~NewDBFileDialog();

    const QString& filePath() const;


public slots:
    int exec() override;


private slots:
    void on_nameLineEdit_textChanged(const QString& arg1);

    void on_toolButton_clicked();

    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_buttonBox_accepted();

private:
    Ui::NewDBFileDialog* ui;

    bool fileNameInvalid = false;

    QString f;
};

 

#endif // NEWDBFILEDIALOG_H
