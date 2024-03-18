#include "sqlitewidget.h"
#include "ui_sqlitewidget.h"

#include <QDataWidgetMapper>
#include <QInputMethod>
#include <QFileDialog>
#include <QMessageBox>
#include <QtSql>


#include "configurations.h"
#include "sqltablemodel.h"
#include "newdbfiledialog.h"
#include "addslavedialog.h"


#define SQL_CREATE "\
CREATE TABLE slaves (\n\
  addr  INTEGER,\n\
  type  TEXT    NOT NULL,\n\
  name  TEXT    NOT NULL,\n\
  word  INTEGER DEFAULT 255,\n\
  wtxt  TEXT    DEFAULT 未响应,\n\
  info  TEXT,\n\
  memo  TEXT,\n\
  PRIMARY KEY(addr),\n\
  CHECK(0 <= addr AND addr <= 239),\n\
  CHECK(LENGTH(type) <= 2),\n\
  CHECK(LENGTH(name) <= 8),\n\
  CHECK(LENGTH(info) <= 13)\n\
);\
;;\
CREATE TABLE keywords (\n\
  addr INTEGER,\n\
  word INTEGER NOT NULL,\n\
  wtxt TEXT,\n\
  wtip TEXT,\n\
  PRIMARY KEY(addr, word),\n\
  FOREIGN KEY(addr) REFERENCES slaves(addr),\n\
  CHECK(0 <= word AND word <= 254)\n\
);\
;;\
CREATE TRIGGER update_slave_state\n\
AFTER UPDATE OF word ON slaves\n\
FOR EACH ROW\n\
BEGIN\n\
  UPDATE slaves\n\
  SET wtxt = COALESCE(\n\
    (SELECT wtxt FROM keywords WHERE addr = NEW.addr AND word = NEW.word),\n\
    CASE\n\
      WHEN NEW.word = 255 THEN '未响应'\n\
      ELSE 'unknown:' || NEW.word\n\
    END\n\
  )\n\
  WHERE addr = NEW.addr;\n\
END;\
"

#define DB_CONNECT_1 "createDB"
#define DB_CONNECT_2 "openDB"


SQLiteWidget::SQLiteWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SQLiteWidget)
    , newDBFileDialog(new NewDBFileDialog(this))
    , addSlaveDialog(new AddSlaveDialog(this))
    , tableModel(new SqlTableModel(this))
    , proxyModel(new QSortFilterProxyModel(this))
    , selectModel(new QItemSelectionModel(proxyModel, this))
    , dataMapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);
    ui->buttonsLayout->setAlignment(Qt::AlignTop);

    ui->listTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    proxyModel->setSourceModel(tableModel);
    dataMapper->setModel(tableModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    ui->listTableView->setModel(proxyModel);
    ui->listTableView->setSelectionModel(selectModel);


    connect(ui->filterLineEdit, &QLineEdit::textChanged, this,
            [this](auto text) {
        proxyModel->setFilterRegularExpression(text);
        proxyModel->setFilterKeyColumn(-1); // 设置搜索范围为所有列
    });

    connect(selectModel, &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection& selected, auto deselected) {
        Q_UNUSED(deselected);
        auto have(!selected.isEmpty());
        ui->delPushButton->setEnabled(have);
        ui->changePushButton->setEnabled(have);
        ui->changeAndSavePushButton->setEnabled(have);
        if (have) {
            auto first(selected.indexes().first());
            auto row(first.row());
            auto data(first.data().toString().toInt(nullptr, 16));
            this->dataMapper->setCurrentIndex(row);
            this->updateButtonList(data);

            auto textIntToHex = [](QLineEdit* p) {
                p->setText(
                    QString("%1").arg(
                        p->text().toInt(), 2, 16, QChar('0')
                    ).toUpper()
                );
            };
            textIntToHex(ui->iL1RLineEdit);
            textIntToHex(ui->iR1LLineEdit);
        } else {
            this->updateButtonList(-1);
            auto x2("XX");
            auto x8("XXXXXXXX");
            ui->iL1RLineEdit->setText(x2);
            ui->iL2RLineEdit->setText(x2);
            ui->iL3RLineEdit->setText(x8);
            ui->iL4RLineEdit->setText(x8);
            ui->iL5MPlainTextEdit->clear();
            ui->iR1LLineEdit->setText(x2);
            ui->iR1RLineEdit->setText(x8);
        }
    });


    connect(addSlaveDialog, &AddSlaveDialog::insertInfo, this, &SQLiteWidget::InsertASlaveInfo);

    connect(addSlaveDialog, &AddSlaveDialog::updateInfo, this, &SQLiteWidget::UpdateASlaveInfo);

    this->loadSetting();

    /*
    QSqlDatabase db;
    if (QSqlDatabase::contains())
    {
        db = QSqlDatabase::database();
        qDebug() << "have";
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(QString("%1/%2.db").arg(DB_PATH, "123"));

        if (!db.open())
        {
            qDebug() << "Error: Failed to connect database." << db.lastError();

        }
        else
        {
            // 在数据库中创建一个表
            QSqlQuery query;
            if (query.exec("CREATE TABLE students (id INT PRIMARY KEY, name TEXT, age INT)")) {
                // 向表中插入数据
                query.prepare("INSERT INTO students (id, name, age) VALUES (:id, :name, :age)");
                query.bindValue(":id", 1);
                query.bindValue(":name", "Alice");
                query.bindValue(":age", 20);
                query.exec();

                // 查询数据
                query.prepare("SELECT * FROM students");
                query.exec();

                // 输出查询结果
                QSqlRecord record = query.record();
                while (query.next()) {
                    int id = query.value(record.indexOf("id")).toInt();
                    QString name = query.value(record.indexOf("name")).toString();
                    int age = query.value(record.indexOf("age")).toInt();
                    qDebug() << "ID:" << id << "Name:" << name << "Age:" << age;
                }
            }
            db.close();
        }
    }
    */
}

SQLiteWidget::~SQLiteWidget()
{
    delete ui;
}


void SQLiteWidget::loadSetting()
{
    if (SETTINGS_CONTAINS({_DB_LAST_OPEN_}))
    {
        auto filePath(SETTINGS().value(_DB_LAST_OPEN_).toString());
        if (!filePath.isEmpty() && QFile(filePath).exists())
        {
            this->openTable(filePath);
        }
    }
}


void SQLiteWidget::createSQLiteDB()
{
    auto connectDB(DB_CONNECT_1);
    if (newDBFileDialog->exec() == QMessageBox::Accepted)
    {
        auto filePath(newDBFileDialog->filePath());
        QFile file(filePath);
        if (!file.exists() ||
            (QMessageBox::warning(
                this, "警告", "文件已存在，是否覆盖？",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes
            ))
        {
            file.remove();
            QSqlDatabase db(
                QSqlDatabase::contains(connectDB) ?
                QSqlDatabase::database(connectDB) :
                QSqlDatabase::addDatabase("QSQLITE", connectDB)
            );
            db.setDatabaseName(filePath);
            if (db.open())
            {
                QSqlQuery sql(db);
                auto sqlCreate = [&sql]() {
                    bool flg = true;
                    for (const auto& s : QString(SQL_CREATE).split(";;"))
                        if (!sql.exec(s))
                        {
                            flg = false;
                            break;
                        }
                    return flg;
                };
                if (sqlCreate())
                {
                    if (QMessageBox::question(
                            this, "提问",
                            "数据库创建成功，是否立即载入？"
                        ) == QMessageBox::Yes)
                    {
                        this->openTable(filePath);
                    }
                }
                else
                    QMessageBox::critical(this, "错误", sql.lastError().text());
                db.close();
            }
            else
                QMessageBox::critical(this, "错误", db.lastError().text());
            // QSqlDatabase::removeDatabase(connectDB);
        }
    }
}


void SQLiteWidget::openSQLiteDB()
{
    auto filePath = QFileDialog::getOpenFileName(
        this, "打开数据库文件", DB_PATH, "SQLite (*.db)");
    if (!filePath.isEmpty())
    {
        this->openTable(filePath);
    }
}


void SQLiteWidget::openTable(const QString& filePath)
{
    SETTINGS().setValue(_DB_LAST_OPEN_, "");
    this->on_cancelPushButton_clicked();
    this->on_spaceSelPushButton_clicked();

    auto connectDB(DB_CONNECT_2);
    QSqlDatabase db(
        QSqlDatabase::contains(connectDB) ?
        QSqlDatabase::database(connectDB) :
        QSqlDatabase::addDatabase("QSQLITE", connectDB)
    );

    if (db.isOpen())
        db.close();

    db.setDatabaseName(filePath);
    if (db.open())
    {
        dbCount++;
        QSqlQuery sql(db);
        if (!sql.exec(QString("UPDATE slaves SET word = 255")))
        {
            QMessageBox::critical(this, "错误", sql.lastError().text());
            return;
        }

        tableModel->setQuery("SELECT * FROM slaves", db);
        if (tableModel->lastError().isValid())
        {
            QMessageBox::critical(
                this, "错误",
                "数据库表格打开失败：" + tableModel->lastError().text()
            );
            return;
        }

        proxyModel->sort(0, Qt::AscendingOrder);
        ui->listTableView->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

        static bool havenLoad = false;
        if (!havenLoad)
        {
            havenLoad = true;
            auto record = tableModel->record();
            tableModel->setHeaderData(record.indexOf("addr"), Qt::Horizontal, "地址");
            tableModel->setHeaderData(record.indexOf("type"), Qt::Horizontal, "类型");
            tableModel->setHeaderData(record.indexOf("name"), Qt::Horizontal, "名称");
            tableModel->setHeaderData(record.indexOf("wtxt"), Qt::Horizontal, "状态");
            dataMapper->clearMapping();
            dataMapper->addMapping(ui->iL1RLineEdit, record.indexOf("addr"));
            dataMapper->addMapping(ui->iL2RLineEdit, record.indexOf("type"));
            dataMapper->addMapping(ui->iL3RLineEdit, record.indexOf("name"));
            dataMapper->addMapping(ui->iL4RLineEdit, record.indexOf("info"));
            dataMapper->addMapping(ui->iR1LLineEdit, record.indexOf("word"));
            dataMapper->addMapping(ui->iR1RLineEdit, record.indexOf("wtxt"));
            dataMapper->addMapping(ui->iL5MPlainTextEdit, record.indexOf("memo"));
            ui->listTableView->setColumnHidden(record.indexOf("word"), true);
            ui->listTableView->setColumnHidden(record.indexOf("info"), true);
            ui->listTableView->setColumnHidden(record.indexOf("memo"), true);
        }

        ui->infoGroupBox->setEnabled(true);
        SETTINGS().setValue(_DB_LAST_OPEN_, filePath);

        /*
        auto tab = new QSqlTableModel(this, db);
        tab->setTable("slaves");
        tab->setEditStrategy(QSqlTableModel::OnManualSubmit);
        tab->setSort(tab->fieldIndex("ip"), Qt::AscendingOrder);
        if (!(tab->select()))
        {
            QMessageBox::critical(
                this, "错误", "数据库表格打开失败："+tab->lastError().text());
        }
        // statusBar->ShowMessage(QString("数量：%1").arg(tab.rowCount()));
        tab->setHeaderData(tab->fieldIndex("addr"), Qt::Horizontal, "地址");
        tab->setHeaderData(tab->fieldIndex("type"), Qt::Horizontal, "类型");
        tab->setHeaderData(tab->fieldIndex("name"), Qt::Horizontal, "名称");
        // tab->setHeaderData(tab->fieldIndex("state"), Qt::Horizontal, "状态字");
        tab->setHeaderData(tab->fieldIndex("wtxt"), Qt::Horizontal, "状态");
        auto sel = new QItemSelectionModel(tab, this);
        ui->tableView->setModel(tab);
        ui->tableView->setSelectionModel(sel);
        ui->tableView->setColumnHidden(tab->fieldIndex("word"), true);
        ui->tableView->setColumnHidden(tab->fieldIndex("info"), true);
        ui->tableView->setColumnHidden(tab->fieldIndex("memo"), true);
        */

        dbCount--;
        if (!dbCount)
            db.close();
    }
    else
        QMessageBox::critical(this, "错误", db.lastError().text());
}


void SQLiteWidget::updateButtonList(int addr)
{
    Q_ASSERT(-1 <= addr && addr <= 255);
    for (auto& b : buttonList)
        b->deleteLater();
    buttonList.clear();

    if (addr == -1)
        return;

    QString select("SELECT * FROM keywords WHERE addr = %1");
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if(db.open())
    {
        dbCount++;
        QSqlQuery sql(db);
        if (!sql.exec(select.arg(addr)))
            QMessageBox::critical(this, "错误", sql.lastError().text());
        else
        {
            static const QString txt("%1(%2)");
            while (sql.next())
            {
                auto b = new QPushButton(this);
                auto addr = sql.value("addr").toString().toInt();
                auto word = sql.value("word").toString().toInt();
                b->setText(txt.arg(
                    sql.value("wtxt").toString(),
                    QString("%1").arg(word, 2, 16, QChar('0')).toUpper()
                ));
                b->setToolTip(sql.value("wtip").toString());
                connect(b, &QPushButton::clicked, this, [addr, word, this]() {
                    emit this->slaveStateChange(addr, word);
                });
                buttonList.append(b);
                ui->buttonsLayout->addWidget(b);
            }
        }
        dbCount--;
        if (!dbCount)
            db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
}


void SQLiteWidget::updateSlaveState(quint8 addr, quint8 word)
{
    // qDebug() << QString::number(addr, 16) << QString::number(word, 16);
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if (db.isOpen())
    {
        dbCount++;
        QSqlQuery sql(db);
        if (!sql.exec(
                QString("UPDATE slaves SET word = %2 WHERE addr = %1")
                    .arg(addr).arg(word)
            ))
        {
            QMessageBox::critical(this, "错误", sql.lastError().text());
            return;
        }

        if (word == 0xff)
            QMessageBox::warning(
                this, "警告",
                QString("地址%1的从机: FF - 未响应").arg(
                    QString::number(addr, 16).toUpper()
                )
            );

        auto row(selectModel->currentIndex().row());

        tableModel->setQuery(tableModel->query().executedQuery(), db);
        if (tableModel->lastError().isValid())
        {
            QMessageBox::critical(
                this, "错误",
                tableModel->lastError().text()
            );
            return;
        }

        ui->listTableView->selectRow(row);

        dbCount--;
        if (!dbCount)
            db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
}


void SQLiteWidget::noSlaveState(quint8 addr, quint8 word)
{
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if(db.open())
    {
        dbCount++;
        QSqlQuery sql(db);
        if (!sql.exec(
                QString("SELECT wtxt FROM keywords WHERE addr = %1 AND word = %2")
                    .arg(addr).arg(word)
                ))
        {
            QMessageBox::critical(this, "错误", sql.lastError().text());
            return;
        }
        sql.first();
        QMessageBox::warning(
            this, "警告",
            QString("地址%1的从机没有状态：%2 - %3").arg(
                QString::number(addr, 16).toUpper(),
                QString::number(word, 16).toUpper(),
                sql.value("wtxt").toString()
            )
        );
        dbCount--;
        if (!dbCount)
            db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
}


void SQLiteWidget::InsertASlaveInfo(QStringList base, QStringList word, QStringList wtxt)
{
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if(db.open())
    {
        dbCount++;
        QSqlQuery sql(db);
        auto runSql = [this, &sql](QString txt) {
            if (!sql.exec(txt)) {
                QMessageBox::critical(this, "错误", sql.lastError().text());
                return false;
            }
            return true;
        };
        auto addr(base[0].toInt(nullptr, 16));
        if (!runSql(
                QString(
                    "INSERT INTO slaves (addr, type, name, memo) "
                    "VALUES (%1, '%2', '%3', '%4')")
                .arg(addr)
                .arg(base[1], base[2], base[3])
            ))
            return;

        for (int i = 0, end = word.length(); i < end; i++)
            if (!runSql(
                    QString(
                        "INSERT INTO keywords (addr, word, wtxt, wtip) "
                        "VALUES (%1, %2, '%3', '%4')")
                    .arg(addr)
                    .arg(word[i].toInt(nullptr, 16))
                    .arg(wtxt[i], "")
                ))
                return;

        tableModel->setQuery(tableModel->query().executedQuery(), db);
        if (tableModel->lastError().isValid())
        {
            QMessageBox::critical(
                this, "错误",
                tableModel->lastError().text()
            );
            return;
        }

        dbCount--;
        if (!dbCount)
            db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
}


void SQLiteWidget::UpdateASlaveInfo(QStringList base, QStringList word, QStringList wtxt)
{
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if(db.open())
    {
        dbCount++;
        QSqlQuery sql(db);
        auto runSql = [this, &sql](QString txt) {
            if (!sql.exec(txt)) {
                QMessageBox::critical(this, "错误", sql.lastError().text());
                return false;
            }
            return true;
        };
        auto addr(base[0].toInt(nullptr, 16));
        if (!runSql(
                QString(
                    "UPDATE slaves SET type = '%2', name = '%3', memo = '%4'"
                    ", word = 255 WHERE addr = %1")
                    .arg(addr)
                    .arg(base[1], base[2], base[3])
            ))
            return;

        if (!runSql(QString("DELETE FROM keywords WHERE addr = %1").arg(addr)))
            return;

        for (int i = 0, end = word.length(); i < end; i++)
            if (!runSql(
                    QString(
                        "INSERT INTO keywords (addr, word, wtxt, wtip) "
                        "VALUES (%1, %2, '%3', '%4')")
                        .arg(addr)
                        .arg(word[i].toInt(nullptr, 16))
                        .arg(wtxt[i], "")
                ))
                return;

        auto row(selectModel->currentIndex().row());

        tableModel->setQuery(tableModel->query().executedQuery(), db);
        if (tableModel->lastError().isValid())
        {
            QMessageBox::critical(
                this, "错误",
                tableModel->lastError().text()
            );
            return;
        }

        ui->listTableView->selectRow(row);

        dbCount--;
        if (!dbCount)
            db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
}


void SQLiteWidget::on_spaceSelPushButton_clicked()
{
    ui->listTableView->clearSelection();
}


void SQLiteWidget::on_changeAndSavePushButton_clicked()
{
    auto flg = !ui->cancelPushButton->isEnabled();
    ui->cancelPushButton->setEnabled(flg);
    ui->listGroupBox->setEnabled(!flg);
    ui->iL5MPlainTextEdit->setReadOnly(!flg);
    if (flg) // now is change
    {
        ui->iL5MPlainTextEdit->setFocusPolicy(Qt::StrongFocus);
        ui->changeAndSavePushButton->setText("保存");
        memo = ui->iL5MPlainTextEdit->toPlainText();
    }
    else // now is save
    {
        ui->iL5MPlainTextEdit->setFocusPolicy(Qt::NoFocus);
        ui->changeAndSavePushButton->setText("修改");
        auto db(QSqlDatabase::database(DB_CONNECT_2));
        if(db.open())
        {
            QSqlQuery sql(db);
            auto addr(ui->iL1RLineEdit->text().toInt(nullptr, 16));
            if (!sql.exec(
                    QString("UPDATE slaves SET memo = '%2' WHERE addr = %1")
                        .arg(addr)
                        .arg(ui->iL5MPlainTextEdit->toPlainText())
                ))
            {
                QMessageBox::critical(this, "错误", sql.lastError().text());
                return;
            }

            auto row(selectModel->currentIndex().row());

            tableModel->setQuery(tableModel->query().executedQuery(), db);
            if (tableModel->lastError().isValid())
            {
                QMessageBox::critical(
                    this, "错误",
                    tableModel->lastError().text()
                    );
                return;
            }

            ui->listTableView->selectRow(row);

            db.close();
        }
        else
            QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
    }

}


void SQLiteWidget::on_cancelPushButton_clicked()
{
    ui->changeAndSavePushButton->setText("修改");
    ui->iL5MPlainTextEdit->setPlainText(memo);
    ui->iL5MPlainTextEdit->setReadOnly(true);
    ui->cancelPushButton->setEnabled(false);
    ui->listGroupBox->setEnabled(true);
}


void SQLiteWidget::on_addPushButton_clicked()
{
    if (addSlaveDialog->exec() == QMessageBox::Accepted)
        addSlaveDialog->emitInfo(AddSlaveDialog::Insert);
}


void SQLiteWidget::on_changePushButton_clicked()
{
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if(db.open())
    {
        QSqlQuery sql(db);
        auto runSql = [this, &sql](QString txt) {
            if (!sql.exec(txt)) {
                QMessageBox::critical(this, "错误", sql.lastError().text());
                return false;
            }
            return true;
        };

        QStringList base;
        QStringList word;
        QStringList wtxt;

        auto index = selectModel->currentIndex();
        auto addr = index.sibling(index.row(), 0).data().toString();

        // auto addr(base[0].toInt(nullptr, 16));
        if (!runSql(QString("SELECT addr, type, name, memo FROM slaves "
                            "WHERE addr = %1").arg(addr.toInt(nullptr, 16))))
            return;

        sql.first();
        if (!sql.isValid())
        {
            QMessageBox::warning(this, "警告", "更新时查询失败");
            return;
        }
        base.append(addr);
        base.append(sql.value("type").toString());
        base.append(sql.value("name").toString());
        base.append(sql.value("memo").toString());

        if (!runSql(QString("SELECT word, wtxt FROM keywords "
                            "WHERE addr = %1").arg(addr.toInt(nullptr, 16))))
            return;


        while (sql.next())
        {
            word.append(
                QString::number(sql.value("word").toString().toInt(), 16)
                );
            wtxt.append(sql.value("wtxt").toString());
        }
        if (addSlaveDialog->exec(base, word, wtxt) == QMessageBox::Accepted)
            addSlaveDialog->emitInfo(AddSlaveDialog::Update);
        db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());
}


void SQLiteWidget::on_delPushButton_clicked()
{
    auto row(selectModel->currentIndex().row());
    auto addr(ui->iL1RLineEdit->text().toInt(nullptr, 16));
    if (QMessageBox::question(
            this, "提示",
            QString("请确认是否删除：\n第 %1 行，地址 %2 的从机信息？")
                .arg(row + 1).arg(ui->iL1RLineEdit->text())
        ) == QMessageBox::No)
        return;
    auto db(QSqlDatabase::database(DB_CONNECT_2));
    if(db.open())
    {
        QSqlQuery sql(db);

        auto delSql = QString("DELETE FROM %2 WHERE addr = %1").arg(addr);
        if (!(sql.exec(delSql.arg("keywords")) && sql.exec(delSql.arg("slaves"))))
        {
            QMessageBox::critical(this, "错误", sql.lastError().text());
            return;
        }

        tableModel->setQuery(tableModel->query().executedQuery(), db);
        if (tableModel->lastError().isValid())
        {
            QMessageBox::critical(
                this, "错误",
                tableModel->lastError().text()
            );
            return;
        }

        db.close();
    }
    else
        QMessageBox::critical(this, "错误", "db:" + db.lastError().text());

}


void SQLiteWidget::on_iL5MPlainTextEdit_textChanged()
{
    if (!ui->cancelPushButton->isEnabled())
        ui->iL5MPlainTextEdit->undo();
}


