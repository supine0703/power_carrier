#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H

#include <QSqlQueryModel>

PROJECT_BEGIN_NAMESPACE

class SqlTableModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit SqlTableModel(QObject* parent = nullptr);

public:
    QVariant data(const QModelIndex& index, int role) const override;
};

PROJECT_END_NAMESPACE

#endif // SQLTABLEMODEL_H
