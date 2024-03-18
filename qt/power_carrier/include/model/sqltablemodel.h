#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H

#include <QSqlQueryModel>

 

class SqlTableModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit SqlTableModel(QObject* parent = nullptr);

public:
    QVariant data(const QModelIndex& index, int role) const override;
};

 

#endif // SQLTABLEMODEL_H
