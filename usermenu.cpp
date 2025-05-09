#include "usermenu.h"

UserMenu::UserMenu(QSqlDatabase connection, int userId, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::UserMenu),
    dbconn(connection),
    currentUserId(userId),
    sendingWind(new SendingWindow(dbconn, -1, this)),
    receivingWind(new ReceivingWindow(dbconn, -1, this))
{
    ui->setupUi(this);

    setupTable(ui->sendingTable);
    setupTable(ui->receivingTable);

    connect(ui->goBackButton, &QPushButton::clicked, this, &UserMenu::handleLogout);
    connect(ui->updateButton, &QPushButton::clicked, this, [=]() {
        updateUserData();
    });
    connect(ui->newSendingButton, &QPushButton::clicked, this, &UserMenu::addSending);
    connect(ui->newReceivingButton, &QPushButton::clicked, this, &UserMenu::addReceiving);

    connect(sendingWind, &SendingWindow::goBack, this, [=]() {
        this->show();
        loadUserMenu(currentUserId);
    });
    connect(receivingWind, &ReceivingWindow::goBack, this, [=]() {
        this->show();
        loadUserMenu(currentUserId);
    });
}

UserMenu::~UserMenu() {
    delete ui;
}

void UserMenu::loadUserMenu(int userId) {
    currentUserId = userId;
    clearTables();

    if (!loadUserInfo())
        return;

    loadSendingTable();
    loadReceivingTable();
}

void UserMenu::clearTables() {
    ui->sendingTable->clearContents();
    ui->sendingTable->setRowCount(0);
    ui->receivingTable->clearContents();
    ui->receivingTable->setRowCount(0);
}

bool UserMenu::loadUserInfo() {
    QSqlQuery query(dbconn);
    query.prepare(R"(SELECT fullname, phone, email FROM users WHERE user_id = :user_id)");
    query.bindValue(":user_id", currentUserId);

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Ошибка", "Пользователь с таким ID не найден.");
        return false;
    }

    const QString fullname = query.value(0).toString();
    ui->welcomeLabel->setText(QString("Добро пожаловать, %1!").arg(fullname));
    ui->fullnameInput->setText(fullname);
    ui->phoneInput->setText(query.value(1).toString());
    ui->emailInput->setText(query.value(2).toString());

    return true;
}

void UserMenu::setupTable(QTableWidget* table) {
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UserMenu::loadTable(QTableWidget* table, const QString& sql, int userId) {
    QSqlQuery query(dbconn);
    query.prepare(sql);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 3; ++col) {
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
}

void UserMenu::loadSendingTable() {
    ui->sendingTable->setColumnCount(3);
    ui->sendingTable->setHorizontalHeaderLabels({"Название", "Дата", "Количество"});
    ui->sendingTable->setColumnWidth(0, 150);
    ui->sendingTable->setColumnWidth(1, 80);

    const QString sql = R"(
        SELECT p.name, s.sending_date, s.ammount
        FROM sending s
        JOIN product p ON s.product_id = p.product_id
        WHERE s.user_id = :user_id
    )";
    loadTable(ui->sendingTable, sql, currentUserId);
}

void UserMenu::loadReceivingTable() {
    ui->receivingTable->setColumnCount(3);
    ui->receivingTable->setHorizontalHeaderLabels({"Название", "Дата", "Количество"});
    ui->receivingTable->setColumnWidth(0, 150);
    ui->receivingTable->setColumnWidth(1, 80);

    const QString sql = R"(
        SELECT p.name, r.receiving_date, r.ammount
        FROM receiving r
        JOIN product p ON r.product_id = p.product_id
        WHERE r.user_id = :user_id
    )";
    loadTable(ui->receivingTable, sql, currentUserId);
}

void UserMenu::updateUserData() {
    QSqlQuery query(dbconn);
    query.prepare(R"(
        UPDATE users
        SET fullname = ?, phone = ?, email = ?
        WHERE user_id = ?
    )");
    query.addBindValue(ui->fullnameInput->text());
    query.addBindValue(ui->phoneInput->text());
    query.addBindValue(ui->emailInput->text());
    query.addBindValue(currentUserId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
    } else {
        QMessageBox::information(this, "Информация", "Информация успешно обновлена");
    }
}

void UserMenu::addSending() {
    sendingWind->loadSendingWindow(currentUserId);
    sendingWind->show();
    sendingWind->raise();
    sendingWind->activateWindow();
}

void UserMenu::addReceiving() {
    receivingWind->loadReceivingWindow(currentUserId);
    receivingWind->show();
    receivingWind->raise();
    receivingWind->activateWindow();
}

void UserMenu::handleLogout() {
    this->close();
    emit backToLogin();
}
