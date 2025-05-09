#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "modules.h"
#include "usermenu.h"
#include "warehouseOwnerMenu.h"

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

private:
    Ui::MainWindow *ui;
    UserMenu *userMenu;
    WarehouseOwnerMenu *wuMenu;
};
#endif // MAINWINDOW_H
