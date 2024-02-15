#ifndef COMRANGEDIALOG_H
#define COMRANGEDIALOG_H

#include <QDialog>

namespace Ui {
class ComRangeDialog;
}

PROJECT_BEGIN_NAMESPACE

class ComRangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComRangeDialog(QWidget *parent = nullptr, int upperLimit = 255);
    ~ComRangeDialog();

    int comMin();

    int comMax();

    void ready();

private:
    Ui::ComRangeDialog *ui;
    int m_comMin, m_comMax;

private slots:
    void on_minSpinBox_valueChanged(int arg1);

    void on_maxSpinBox_valueChanged(int arg1);

    void on_buttonBox_accepted();

signals:
    void comRangeChanged(int, int);
};

PROJECT_END_NAMESPACE

#endif // COMRANGEDIALOG_H
