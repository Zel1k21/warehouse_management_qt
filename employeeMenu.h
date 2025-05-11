#ifndef EMPLOYEEMENU_H
#define EMPLOYEEMENU_H

#include "modules.h"
#include "ui_employeeMenu.h"

namespace Ui {
class EmployeeMenu;
}

class EmployeeMenu: public QMainWindow{
    Q_OBJECT

public:
    explicit EmployeeMenu(QSqlDatabase connection, QWidget *parent = nullptr);
    ~EmployeeMenu();
    void loadEmployeeMenu(int userId);

signals:
    void backToLogin();

private:
    Ui::EmployeeMenu *ui;
    QSqlDatabase dbconn;
    int currentUserId;
    void handleLogout();
    void loadProductTable();
    void loadUserData();
    void fillWarehouseInput();
    void updateUserData();
};
#endif // EMPLOYEEMENU_H
