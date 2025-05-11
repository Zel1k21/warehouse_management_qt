#include "warehouseOwnerMenu.h"

WarehouseOwnerMenu::WarehouseOwnerMenu(QSqlDatabase connection, QWidget *parent)
    :QMainWindow(parent), ui(new Ui::WarehouseOwnerMenu), dbconn(connection)
{
    ui->setupUi(this);
    connect(ui->goBackButton, &QPushButton::clicked, this, &WarehouseOwnerMenu::handleLogout);
    connect(ui->switchToProduct, &QPushButton::clicked, this, [=](){
        ui->infoTypeWidget->setCurrentIndex(1);
        handleInfoTypeSwitch();
    });
    connect(ui->switchToEmployee, &QPushButton::clicked, this, [=](){
        ui->infoTypeWidget->setCurrentIndex(0);
        handleInfoTypeSwitch();
    });
    connect(ui->warehouseSelection, &QComboBox::currentIndexChanged, this, &WarehouseOwnerMenu::handleInfoTypeSwitch);
    connect(ui->saveButton, &QPushButton::clicked, this, &WarehouseOwnerMenu::updateInputData);
}

WarehouseOwnerMenu::~WarehouseOwnerMenu()
{
    delete ui;
}

void WarehouseOwnerMenu::loadWarehouseOwnerMenu(int userId){
    currentUserId = userId;
    ui->warehouseSelection->clear();

    QSqlQuery query(dbconn);
    query.prepare("select address from warehouse where owner_id = :id order by warehouse_id");
    query.bindValue(":id", currentUserId);

    if(!query.exec()){
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    bool hasWarehouse = false;
    while (query.next()){
        hasWarehouse = true;
        ui->warehouseSelection->addItem(query.value(0).toString());
    }

    if (!hasWarehouse){
        query.prepare("insert into warehouse (owner_id, address) values (:o_id, 'dafault address')");
        query.bindValue(":o_id", currentUserId);
        if (!query.exec()) {
            QMessageBox::critical(this, "Ошибка при создании склада", query.lastError().text());
            return;
        }
        loadWarehouseOwnerMenu(currentUserId);
        return;
    }
    handleInfoTypeSwitch();
}

void WarehouseOwnerMenu::handleInfoTypeSwitch(){
    QString sql;
    QSqlQuery query(dbconn);
    sql = "select warehouse_id from warehouse where address = :address";
    query.prepare(sql);
    query.bindValue(":address",ui->warehouseSelection->currentText());

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }
    if (query.next())
        selectedWarehouse = query.value(0).toInt();

    if (ui->infoTypeWidget->currentIndex() == 1) {
        ui->productTable->clear();
        ui->productTable->setRowCount(0);
        ui->productTable->setColumnCount(5);
        ui->productTable->setHorizontalHeaderLabels({"Название", "Стоимость", "На складе", "Отправлено", "Получено"});
        sql = "select name, cost, quantity, total_sent, total_received "
              "from ProductWithQuantity where warehouse_id = :w_id and quantity > 0";
        loadTable(ui->productTable, sql);
    } else if (ui->infoTypeWidget->currentIndex() == 0) {
        ui->employeeTable->clear();
        ui->employeeTable->setRowCount(0);
        ui->employeeTable->setColumnCount(4);
        ui->employeeTable->setHorizontalHeaderLabels({"ФИО", "Опыт", "Должность", "Телефон"});
        sql = "select fullname, experience, status, phone "
              "from employee where warehouse_id = :w_id";
        loadTable(ui->employeeTable, sql);
    }
    ui->productTable->setColumnWidth(0, 160);
    ui->employeeTable->setColumnWidth(0, 180);
    ui->productTable->horizontalHeader()->setStretchLastSection(true);
    ui->employeeTable->horizontalHeader()->setStretchLastSection(true);

    fillUserData();
    fillWarehouseData();
}

void WarehouseOwnerMenu::loadTable(QTableWidget* table, const QString& sql){
    QSqlQuery query(dbconn);
    query.prepare(sql);
    query.bindValue(":w_id", selectedWarehouse);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < table->columnCount(); ++col) {
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
}

void WarehouseOwnerMenu::fillUserData() {
    QSqlQuery getUserData(dbconn);
    QString sql = "select fullname, tax_number, contact_phone, contact_email from warehouse_owner where warehouse_owner_id = :o_id";
    getUserData.prepare(sql);
    getUserData.bindValue(":o_id", currentUserId);

    if (!getUserData.exec()) {
        QMessageBox::critical(this, "Ошибка", getUserData.lastError().text());
        return;
    }
    if (getUserData.next()) {
        ui->nameInput->setText(getUserData.value(0).toString());
        ui->taxNumberInput->setText(getUserData.value(1).toString());
        ui->phoneInput->setText(getUserData.value(2).toString());
        ui->emailInput->setText(getUserData.value(3).toString());
    } else {
        QMessageBox::warning(this, "Нет данных", "Пользователь с указанным ID не найден.");
    }
}

void WarehouseOwnerMenu::fillWarehouseData(){
    QSqlQuery getWarehouseData(dbconn);
    QString sql = "select address, capacity from warehouse where warehouse_id = ?";
    getWarehouseData.prepare(sql);
    getWarehouseData.addBindValue(selectedWarehouse);

    if (!getWarehouseData.exec()) {
        QMessageBox::critical(this, "Ошибка", getWarehouseData.lastError().text());
        return;
    }

    if (getWarehouseData.next()) {
        ui->addressInput->setText(getWarehouseData.value(0).toString());
        ui->capacityInput->setText(getWarehouseData.value(1).toString());
    }
}

void WarehouseOwnerMenu::handleLogout() {
    this->close();
    emit backToLogin();
}

void WarehouseOwnerMenu::updateInputData(){
    QSqlQuery query(dbconn), userDataUpdate(dbconn);
    query.prepare("update warehouse set address = ?, capacity = ? "
                  "where warehouse_id = ?");
    query.addBindValue(ui->addressInput->text());
    query.addBindValue(ui->capacityInput->text());
    query.addBindValue(selectedWarehouse);

    userDataUpdate.prepare("update warehouse_owner set fullname = ?, contact_phone = ?, contact_email = ?, tax_number = ? "
                  "where warehouse_owner_id = ?");
    userDataUpdate.addBindValue(ui->nameInput->text());
    userDataUpdate.addBindValue(ui->phoneInput->text());
    userDataUpdate.addBindValue(ui->emailInput->text());
    userDataUpdate.addBindValue(ui->taxNumberInput->text());
    userDataUpdate.addBindValue(currentUserId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
    }

    if (!userDataUpdate.exec()) {
        QMessageBox::critical(this, "Ошибка", userDataUpdate.lastError().text());
    }
}
