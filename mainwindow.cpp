#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    ui->userPhone->setInputMask("+7 (000) 000-00-00;_");
    ui->ownerPhone->setInputMask("+7 (000) 000-00-00;_");
    ui->employeePhone->setInputMask("+7 (000) 000-00-00;_");
    ui->loginPassword->setEchoMode(QLineEdit::Password);
    ui->signupPassword->setEchoMode(QLineEdit::Password);
    setFixedSize(800, 600);

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
    case AccountType::Employee: ui->signupStackedWidget->setCurrentIndex(0); uploadWarehouseList(); break;
    case AccountType::User: ui->signupStackedWidget->setCurrentIndex(1); break;
    case AccountType::Owner: ui->signupStackedWidget->setCurrentIndex(2); break;
    default: break;
    }
}

void MainWindow::returnToMain() {
    this->show();
    ui->loginPassword->clear();
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

    QString password = ui->loginPassword->text();
    QString fullname = ui->usersTable->item(selectedRow, 0)->text();
    QSqlQuery query(dbconn);

    if (type == AccountType::User) {
        query.prepare("SELECT user_id FROM users WHERE fullname = :fullname");
        query.bindValue(":fullname", fullname);
        if (query.exec() && query.next()) {
            if (checkAccountPassword(password, type)) {
            userMenu->loadUserMenu(query.value(0).toInt());
            userMenu->show(); this->hide();
            } else {
                QMessageBox::critical(this, "Ошибка", "Направильный пароль");
                return;
            }
        }
    } else if (type == AccountType::Owner) {
        query.prepare("SELECT warehouse_owner_id FROM warehouse_owner WHERE fullname = :fullname");
        query.bindValue(":fullname", fullname);
        if (query.exec() && query.next()) {
            if (checkAccountPassword(password, type)) {
            wuMenu->loadWarehouseOwnerMenu(query.value(0).toInt());
            wuMenu->show(); this->hide();
            } else {
                QMessageBox::critical(this, "Ошибка", "Направильный пароль");
                return;
            }
        }
    } else if (type == AccountType::Employee) {
        query.prepare("SELECT employee_id FROM employee WHERE fullname = :fullname");
        query.bindValue(":fullname", fullname);
        if (query.exec() && query.next()) {
            if (checkAccountPassword(password, type)) {
            eMenu->loadEmployeeMenu(query.value(0).toInt());
            eMenu->show(); this->hide();
            } else {
                QMessageBox::critical(this, "Ошибка", "Направильный пароль");
                return;
            }
        }
    }
}

void MainWindow::addNewUser() {
    AccountType type = stringToAccountType(ui->signupAccountType->currentText());
    QSqlQuery query(dbconn);
    QVariant lastId;

    if (type == AccountType::User) {
        QString fullname = ui->userFullname->text();
        QString phone = ui->userPhone->text();
        QString email = ui->userEmail->text();

        if (!validateUserInput(fullname, phone, email, "пользователя")) return;

        query.prepare("INSERT INTO users (fullname, phone, email) VALUES (?, ?, ?)");
        query.addBindValue(fullname);
        query.addBindValue(phone);
        query.addBindValue(email);
        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }

        lastId = query.lastInsertId();
        userMenu->loadUserMenu(lastId.toInt());
        userMenu->show(); this->hide();
    }
    else if (type == AccountType::Owner) {
        QString fullname = ui->ownerName->text();
        QString phone = ui->ownerPhone->text();
        QString email = ui->ownerEmail->text();
        QString tax_number = ui->ownerTaxNumber->text();

        if (!validateUserInput(fullname, phone, email, "владельца", tax_number)) return;

        query.prepare("INSERT INTO warehouse_owner (fullname, contact_phone, contact_email, tax_number) VALUES (?, ?, ?, ?)");
        query.addBindValue(fullname);
        query.addBindValue(phone);
        query.addBindValue(email);
        query.addBindValue(tax_number);
        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }

        lastId = query.lastInsertId();
        wuMenu->loadWarehouseOwnerMenu(lastId.toInt());
        wuMenu->show(); this->hide();
    }
    else if (type == AccountType::Employee) {
        QString fullname = ui->employeeFullname->text();
        QString phone = ui->employeePhone->text();
        QString email = ui->employeeEmail->text();

        if (!validateUserInput(fullname, phone, email, "сотрудника")) return;

        query.prepare(R"(INSERT INTO employee (fullname, phone, email, experience, status, warehouse_id)
                         SELECT ?, ?, ?, 0, ?, warehouse_id FROM warehouse WHERE address = ?)");
        query.addBindValue(fullname);
        query.addBindValue(phone);
        query.addBindValue(email);
        query.addBindValue("стажер");
        query.addBindValue(ui->employeeWarehouse->currentText());
        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }

        lastId = query.lastInsertId();
        eMenu->loadEmployeeMenu(lastId.toInt());
        eMenu->show(); this->hide();
    }

    loadUsersTable();
}


bool MainWindow::validateUserInput(const QString& fullname, const QString& phone, const QString& email, const QString& role, const QString& tax_number) {
    if (fullname.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", QString("Поле ФИО %1 не должно быть пустым").arg(role));
        return false;
    }

    if (phone.length() != 18 && email.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "Укажите хотя бы телефон или email");
        return false;
    }
    if (tax_number.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение", "ИНН не должен быть пустым");
        return false;
    }

    if (tax_number != "-1" && (tax_number.length() != 12 || !tax_number.contains(QRegularExpression("^\\d{12}$")))) {
        QMessageBox::warning(this, "Предупреждение", "ИНН должен состоять из 12 цифр");
        return false;
    }

    return true;
}

void MainWindow::uploadWarehouseList(){
    QSqlQuery query(dbconn);
    query.prepare("select address from warehouse order by warehouse_id");
    if (!query.exec()){
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }
    while (query.next()){
        ui->employeeWarehouse->addItem(query.value(0).toString());
    }
}
