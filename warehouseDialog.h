#ifndef WAREHOUSEDIALOG_H
#define WAREHOUSEDIALOG_H
#include "modules.h"

class QTableWidget;

class WarehouseDialog : public QDialog {
    Q_OBJECT

public:
    explicit WarehouseDialog(QSqlDatabase dbconn, QWidget *parent = nullptr);
    ~WarehouseDialog();

private:
    QSqlDatabase dbconn;
    QTableWidget *table;

    void loadData();
    void printTable();
};

#endif // WAREHOUSEDIALOG_H
