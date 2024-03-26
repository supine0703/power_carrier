#ifndef WAITTABLEWIDGET_H
#define WAITTABLEWIDGET_H

#include <QWidget>

namespace Ui {
class WaitTableWidget;
}

class WaitTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaitTableWidget(QWidget* parent = nullptr);
    ~WaitTableWidget();

    void append(const QString& txt);
    void remove(int row);

private:
    Ui::WaitTableWidget* ui;
};

#endif // WAITTABLEWIDGET_H
