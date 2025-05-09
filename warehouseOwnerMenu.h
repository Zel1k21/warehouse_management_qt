#ifndef WAREHOUSEOWNERMENU_H
#define WAREHOUSEOWNERMENU_H
#include "modules.h"
#include "ui_warehouseOwnerMenu.h"

namespace Ui {
    class WarehouseOwnerMenu;
}

class WarehouseOwnerMenu: public QMainWindow
{
    Q_OBJECT

public:
    explicit WarehouseOwnerMenu(QSqlDatabase connection, int userId = -1, QWidget *parent = nullptr);
    ~WarehouseOwnerMenu();
    void loadWarehouseOwnerMenu(int userId);
    void handleLogout();

signals:
    void backToLogin();

private:
    Ui::WarehouseOwnerMenu *ui;
    QSqlDatabase dbconn;
    int currentUserId;
    int selectedWarehouse;
    void handleInfoTypeSwitch();
    void loadTable(QTableWidget* table, const QString& sql);
    void fillUserData();
    void fillWarehouseData();
};

#endif // WAREHOUSEOWNERMENU_H
