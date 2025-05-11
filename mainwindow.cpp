#include "mainwindow.h"
#include "ui_mainwindow.h"

enum class AccountType { Invalid, User, Owner, Employee };

AccountType stringToAccountType(const QString &str) {
    if (str == "Пользователь") return AccountType::User;
    if (str == "Владелец склада") return AccountType::Owner;
    if (str == "Сотрудник") return AccountType::Employee;
    return AccountType::Invalid;
}

QStringList getAccountTypeStrings() {
    return {"Выберите тип", "Пользователь", "Владелец склада", "Сотрудник"};
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userMenu(new UserMenu(dbconn, this))
    , wuMenu(new WarehouseOwnerMenu(dbconn, this))
    , eMenu(new EmployeeMenu(dbconn, this))
{
    ui->setupUi(this);
    dbConnect();

    setupUsersTable();
    populateAccountTypes(ui->loginAccountType);
    populateAccountTypes(ui->signupAccountType);

    connectSignals();

    ui->loginAccountType->setCurrentIndex(0);
    ui->signupAccountType->setCurrentIndex(0);
}

MainWindow::~MainWindow() {
    if (dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void MainWindow::dbConnect() {
    if (!dbconn.isOpen()) {
        dbconn = QSqlDatabase::addDatabase("QPSQL");
        dbconn.setDatabaseName("warehouse_db");
        dbconn.setHostName("193.33.153.154");
        dbconn.setPort(54321);
        dbconn.setUserName("postgres");
        dbconn.setPassword("z8w(XRG?e6f4Fqn90O-");
        dbconn.open();
    }
}

void MainWindow::setupUsersTable() {
    ui->usersTable->clearContents();
    ui->usersTable->setRowCount(0);
    ui->usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::populateAccountTypes(QComboBox *comboBox) {
    QStringList types = getAccountTypeStrings();
    comboBox->addItems(types);
    QModelIndex index = comboBox->model()->index(0, 0);
    comboBox->model()->setData(index, QVariant(0), Qt::UserRole - 1);
}

void MainWindow::connectSignals() {
    connect(ui->signupAccountType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSignupTypeChanged);
    connect(ui->loginAccountType, &QComboBox::currentTextChanged,
            this, &MainWindow::loadUsersTable);
    connect(ui->searchField, &QLineEdit::textChanged,
            this, &MainWindow::loadUsersTable);
    connect(ui->loginButton, &QPushButton::clicked,
            this, &MainWindow::login);
    connect(userMenu, &UserMenu::backToLogin,
            this, &MainWindow::returnToMain);
    connect(wuMenu, &WarehouseOwnerMenu::backToLogin,
            this, &MainWindow::returnToMain);
    connect(eMenu, &EmployeeMenu::backToLogin,
            this, &MainWindow::returnToMain);
    connect(ui->signupButton, &QPushButton::clicked,
            this, &MainWindow::addNewUser);
}

void MainWindow::onSignupTypeChanged(int index) {
    QString typeText = ui->signupAccountType->itemText(index);
    AccountType type = stringToAccountType(typeText);
    switch (type) {
    case AccountType::Employee: ui->signupStackedWidget->setCurrentIndex(0); break;
    case AccountType::User: ui->signupStackedWidget->setCurrentIndex(1); break;
    case AccountType::Owner: ui->signupStackedWidget->setCurrentIndex(2); break;
    default: break;
    }
}

void MainWindow::returnToMain() {
    this->show();
    loadUsersTable();
}

void MainWindow::loadUsersTable() {
    AccountType type = stringToAccountType(ui->loginAccountType->currentText());
    QString search = ui->searchField->text();

    ui->usersTable->clearContents();
    ui->usersTable->setRowCount(0);
    QSqlQuery query(dbconn);

    if (type == AccountType::User) {
        ui->usersTable->setColumnCount(1);
        ui->usersTable->setHorizontalHeaderLabels({"ФИО"});
        query.prepare("SELECT fullname FROM users WHERE fullname ILIKE :search ORDER BY user_id");
    } else if (type == AccountType::Owner) {
        ui->usersTable->setColumnCount(1);
        ui->usersTable->setHorizontalHeaderLabels({"ФИО"});
        query.prepare("SELECT fullname FROM warehouse_owner WHERE fullname ILIKE :search order by warehouse_owner_id");
    } else if (type == AccountType::Employee) {
        ui->usersTable->setColumnCount(1);
        ui->usersTable->setHorizontalHeaderLabels({"ФИО"});
        query.prepare("SELECT fullname FROM employee WHERE fullname ILIKE :search order by employee_id");
    } else return;

    query.bindValue(":search", "%" + search + "%");
    if (!query.exec()) return;

    int row = 0;
    while (query.next()) {
        ui->usersTable->insertRow(row);
        for (int col = 0; col < ui->usersTable->columnCount(); ++col) {
            ui->usersTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
    ui->usersTable->setColumnWidth(0, 220);
}

void MainWindow::login() {
    int selectedRow = ui->usersTable->currentRow();
    AccountType type = stringToAccountType(ui->loginAccountType->currentText());
    if (selectedRow < 0) {
        QMessageBox::information(this, "Информация", "Выберите пользователя");
        return;
    }

    QString fullname = ui->usersTable->item(selectedRow, 0)->text();
    QSqlQuery query(dbconn);

    if (type == AccountType::User) {
        query.prepare("SELECT user_id FROM users WHERE fullname = :fullname");
        query.bindValue(":fullname", fullname);
        if (query.exec() && query.next()) {
            userMenu->loadUserMenu(query.value(0).toInt());
            userMenu->show(); this->hide();
        }
    } else if (type == AccountType::Owner) {
        query.prepare("SELECT warehouse_owner_id FROM warehouse_owner WHERE fullname = :fullname");
        query.bindValue(":fullname", fullname);
        if (query.exec() && query.next()) {
            wuMenu->loadWarehouseOwnerMenu(query.value(0).toInt());
            wuMenu->show(); this->hide();
        }
    } else if (type == AccountType::Employee) {
        query.prepare("SELECT employee_id FROM employee WHERE fullname = :fullname");
        query.bindValue(":fullname", fullname);
        if (query.exec() && query.next()) {
            eMenu->loadEmployeeMenu(query.value(0).toInt());
            eMenu->show(); this->hide();
        }
    }
}

void MainWindow::addNewUser() {
    AccountType type = stringToAccountType(ui->signupAccountType->currentText());
    QSqlQuery query(dbconn), newUserId(dbconn);

    if (type == AccountType::User) {
        query.prepare("INSERT INTO users (fullname, phone, email) VALUES (?, ?, ?)");
        query.addBindValue(ui->userFullname->text());
        query.addBindValue(ui->userPhone->text());
        query.addBindValue(ui->userEmail->text());
        newUserId.prepare("select last (user_id) from users");
        if (newUserId.next())
            userMenu->loadUserMenu(newUserId.value(0).toInt());
    } else if (type == AccountType::Owner) {
        query.prepare("INSERT INTO warehouse_owner (fullname, contact_phone, contact_email, tax_number) VALUES (?, ?, ?, ?)");
        query.addBindValue(ui->ownerName->text());
        query.addBindValue(ui->ownerPhone->text());
        query.addBindValue(ui->ownerEmail->text());
        query.addBindValue(ui->ownerTaxNumber->text());
        newUserId.prepare("select last (warehouse_owner_id) from warehouse_owner");
        if (newUserId.next())
            wuMenu->loadWarehouseOwnerMenu(newUserId.value(0).toInt());
    } else if (type == AccountType::Employee) {
        query.prepare("INSERT INTO employee (fullname, phone, email, warehouse_id) SELECT ?, ?, ?, warehouse_id FROM warehouse WHERE address = ?");
        query.addBindValue(ui->employeeFullname->text());
        query.addBindValue(ui->employeePhone->text());
        query.addBindValue(ui->employeeEmail->text());
        query.addBindValue(ui->employeeWarehouse->currentText());
        newUserId.prepare("select last (employee_id) from employee");
        if (newUserId.next())
            eMenu->loadEmployeeMenu(newUserId.value(0).toInt());
    }

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    loadUsersTable();
    //QMessageBox::information(this, "Успех", "Пользователь добавлен");
}
