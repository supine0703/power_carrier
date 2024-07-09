#include "sqltablemodel.h"


SqlTableModel::SqlTableModel(QObject* parent)
    : QSqlQueryModel{parent}
{
}

QVariant SqlTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    else if (index.column() == 0 && role == Qt::DisplayRole)
    {
        int value = QSqlQueryModel::data(index, Qt::DisplayRole).toInt();
        return QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
    }
    else
    {
        return QSqlQueryModel::data(index, role);
    }
}
