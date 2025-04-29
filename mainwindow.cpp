#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userMenu(new UserMenu(dbconn, -1,this))
{
    ui->setupUi(this);
    dbConnect();

    QStringList accountTypes;
    accountTypes << "Выберите тип" << "Пользователь" << "Владелец склада" << "Сотрудник";

    ui->usersTable->clearContents();
    ui->usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Выбор типа аккаунта
    connect(ui->signupAccountType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index){
                if (ui->signupAccountType->itemText(index) == "Сотрудник") {
                    ui->signupStackedWidget->setCurrentIndex(0);
                } else if (ui->signupAccountType->itemText(index) == "Пользователь") {
                    ui->signupStackedWidget->setCurrentIndex(1);
                } else if (ui->signupAccountType->itemText(index) == "Владелец склада") {
                    ui->signupStackedWidget->setCurrentIndex(2);
                }
            });

    // Триггеры обновления таблицы аккаунтов
    connect(ui->loginAccountType, &QComboBox::currentTextChanged,
            this, &MainWindow::loadUsersTable);
    connect(ui->searchField, &QLineEdit::textChanged,
            this, &MainWindow::loadUsersTable);
    connect(ui->loginButton, SIGNAL(clicked(bool)), this, SLOT(login()));

    // Значение типа учетной записи по умолчанию для входа в аккаунт
    ui->loginAccountType->addItems(accountTypes);
    QModelIndex loginIndex = ui->loginAccountType->model()->index(0,0);
    QVariant v(0);
    ui->loginAccountType->model()->setData(loginIndex, v, Qt::UserRole - 1);
    ui->loginAccountType->setCurrentIndex(0);

    // Значение типа учетной записи по умолчанию для регистрации
    ui->signupAccountType->addItems(accountTypes);
    QModelIndex signupIndex = ui->signupAccountType->model()->index(0,0);
    QVariant w(0);
    ui->signupAccountType->model()->setData(signupIndex, w, Qt::UserRole - 1);
    ui->signupAccountType->setCurrentIndex(0);

}

MainWindow::~MainWindow()
{
    if (dbconn.isOpen())
        dbconn.close();
    delete ui;
}

void MainWindow::dbConnect(){
    if (!dbconn.isOpen()){
        dbconn = QSqlDatabase::addDatabase("QPSQL");
        dbconn.setDatabaseName("warehouse_db");
        dbconn.setHostName("localhost");
        dbconn.setUserName("postgres");
        dbconn.setPassword("XpQue2105");
        if (dbconn.open())
            qDebug() << "Connection is open...";
        else {
            qDebug() << "Connection error:" << (dbconn.lastError().text());
        }
    } else {
        qDebug() << "Cant open DB";
        return;
    }
}

// Обновление данных таблицы
void MainWindow::loadUsersTable() {
    QString type = ui->loginAccountType->currentText();
    QString search = ui->searchField->text();

    ui->usersTable->clearContents();
    ui->usersTable->setRowCount(0);
    QSqlQuery query(dbconn);

    ui->usersTable->setColumnCount(1);
    if (type == "Пользователь") {
        ui->usersTable->setHorizontalHeaderLabels(QStringList() << "ФИО");
        query.prepare("SELECT fullname FROM users WHERE fullname ILIKE :search");
    } else if (type == "Владелец склада") {
        ui->usersTable->setHorizontalHeaderLabels(QStringList() << "ФИО");
        query.prepare("SELECT fullname FROM warehouse_owner WHERE fullname ILIKE :search");
    } else if (type == "Сотрудник") {
        ui->usersTable->setHorizontalHeaderLabels(QStringList() << "ФИО" << "Склад");
        query.prepare("SELECT fullname from employee where fullname ILIKE :search");
    } else {
        return;
    }

    query.bindValue(":search", "%" + search + "%");

    if (!query.exec()) {
        qDebug() << "Ошибка запроса:" << query.lastError().text();
        return;
    }

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

void MainWindow::login(){
    int selectedRow = ui->usersTable->currentRow();
    QString type = ui->loginAccountType->currentText();
    if (selectedRow < 0) {
        QMessageBox::information(this, "Информация", "Выберите пользователя");
        return;
    }
    QString originalFullname = ui->usersTable->item(selectedRow, 0)->text();
    QSqlQuery query(dbconn);

    if (type == "Пользователь") {
    query.prepare(R"(select user_id from users where fullname = :fullname)");
    query.bindValue(":fullname", originalFullname);
    bool query_success = query.exec();
    if (!query_success) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }
    if (query.next()) {
        int userId = query.value(0).toInt();
        userMenu->loadUserMenu(userId);
    } else {
        QMessageBox::warning(this, "Пользователь не найден", "Нет пользователя с таким ФИО.");
    }
    userMenu->show();
    userMenu->raise();
    userMenu->activateWindow();
    }
}
