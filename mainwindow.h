#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modules.h"
#include "usermenu.h"
#include "warehouseOwnerMenu.h"
#include "employeeMenu.h"
#include "accountType.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSqlDatabase dbconn;

private slots:
    void dbConnect();
    void loadUsersTable();
    void login();
    void addNewUser();
    void setupUsersTable();
    void populateAccountTypes(QComboBox *comboBox);
    void connectSignals();
    void onSignupTypeChanged(int index);
    void returnToMain();
    void uploadWarehouseList();
    bool validateUserInput(const QString& fullname, const QString& phone, const QString& email, const QString& role, const QString& tax_number = "-1");

private:
    Ui::MainWindow *ui;
    UserMenu *userMenu;
    WarehouseOwnerMenu *wuMenu;
    EmployeeMenu *eMenu;
};
#endif // MAINWINDOW_H
