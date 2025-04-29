#include "usermenu.h"
#include "ui_usermenu.h"

UserMenu::UserMenu(QSqlDatabase connection, int userId, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UserMenu), dbconn(connection), currentUserId(userId)
{
    ui->setupUi(this);
}

UserMenu::~UserMenu() {
    delete ui;
}

void UserMenu::loadUserMenu(int userId){
    currentUserId = userId;
}
