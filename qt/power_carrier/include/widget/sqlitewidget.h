#ifndef SQLITEWIDGET_H
#define SQLITEWIDGET_H

#include <QWidget>


namespace Ui {
class SQLiteWidget;
}

class QPushButton;
class QDataWidgetMapper;
class QItemSelectionModel;
class QSortFilterProxyModel;

 

class NewDBFileDialog;
class AddSlaveDialog;
class SqlTableModel;


class SQLiteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SQLiteWidget(QWidget* parent = nullptr);
    ~SQLiteWidget();


public slots:
    void createSQLiteDB();
    void openSQLiteDB();

    void updateSlaveState(quint8 addr, quint8 word);
    void noSlaveState(quint8 addr, quint8 word);


private slots:
    void on_spaceSelPushButton_clicked();

    void on_changeAndSavePushButton_clicked();

    void on_cancelPushButton_clicked();

    void on_addPushButton_clicked();

    void on_changePushButton_clicked();

    void on_delPushButton_clicked();


    void on_iL5MPlainTextEdit_textChanged();

private:
    void loadSetting();

    void openTable(const QString& filePath);

    void updateButtonList(int addr);

    void InsertASlaveInfo(QStringList base, QStringList word, QStringList wtxt);

    void UpdateASlaveInfo(QStringList base, QStringList word, QStringList wtxt);

private:
    Ui::SQLiteWidget* ui;

    NewDBFileDialog* newDBFileDialog;
    AddSlaveDialog* addSlaveDialog;

    SqlTableModel* tableModel;
    QSortFilterProxyModel* proxyModel;
    QItemSelectionModel* selectModel;

    QDataWidgetMapper* dataMapper;

    QList<QPushButton*> buttonList;
    QString memo = "";

    int dbCount = 0;

signals:
    void slaveStateChange(quint8 ,quint8);
};

 

#endif // SQLITEWIDGET_H
