#ifndef USERMENU_H
#define USERMENU_H

#include "modules.h"
#include "ui_usermenu.h"
#include "sendingWindow.h"
#include "receivingWindow.h"

namespace Ui {
class UserMenu;
}

class UserMenu: public QMainWindow{
    Q_OBJECT

public:
    explicit UserMenu(QSqlDatabase connection, int userId = -1, QWidget *parent = nullptr);
    ~UserMenu();
    void loadUserMenu(int userId);
    void handleLogout();
    void updateUserData();
    void addSending();
    void addReceiving();

signals:
    void backToLogin();

private:
    Ui::UserMenu *ui;
    QSqlDatabase dbconn;
    int currentUserId;
    SendingWindow *sendingWind;
    ReceivingWindow *receivingWind;

    void clearTables();
    bool loadUserInfo();
    void setupTable(QTableWidget* table);
    void loadTable(QTableWidget* table, const QString& sql, int userId);
    void loadSendingTable();
    void loadReceivingTable();
};

#endif // USERMENU_H
