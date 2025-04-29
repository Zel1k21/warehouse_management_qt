#ifndef USERMENU_H
#define USERMENU_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidget>

namespace Ui {
class UserMenu;
}

class UserMenu: public QMainWindow{
    Q_OBJECT

public:
    explicit UserMenu(QSqlDatabase db, int userId = -1, QWidget *parent = nullptr);
    ~UserMenu();
    void loadUserMenu(int userId);

private:
    Ui::UserMenu *ui;
    QSqlDatabase dbconn;
    int currentUserId;
};

#endif // USERMENU_H
