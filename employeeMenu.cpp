#include "employeeMenu.h"

EmployeeMenu::EmployeeMenu(QSqlDatabase connection, QWidget *parent)
    :QMainWindow(parent), ui(new Ui::EmployeeMenu), dbconn(connection)
{
    ui->setupUi(this);
    setFixedSize(800, 600);

    connect(ui->goBackButton, &QPushButton::clicked, this, &EmployeeMenu::handleLogout);
    connect(ui->dataUpdateButton, &QPushButton::clicked, this, &EmployeeMenu::updateUserData);
    connect(ui->searchField, &QLineEdit::textChanged, this, [=]{loadEmployeeMenu(currentUserId);});
}

EmployeeMenu::~EmployeeMenu(){
    delete ui;
}

void EmployeeMenu::handleLogout() {
    this->close();
    emit backToLogin();
}

void EmployeeMenu::loadEmployeeMenu(int userId){
    currentUserId = userId;
    ui->productTable->clear();
    ui->productTable->setRowCount(0);
    ui->productTable->setColumnCount(5);
    ui->productTable->setHorizontalHeaderLabels({"Название", "Стоимость", "На складе", "Получено", "Отправлено"});
    ui->productTable->setColumnWidth(0, 160);
    ui->productTable->horizontalHeader()->setStretchLastSection(true);
    loadProductTable();
    loadUserData();
    fillWarehouseInput();
}

void EmployeeMenu::loadProductTable(){
    QSqlQuery query(dbconn);
    QString search = ui->searchField->text();
    query.prepare("select p.name, p.cost, p.quantity, p.total_received, p.total_sent "
                  "from ProductWithQuantity p join employee e on e.warehouse_id = p.warehouse_id "
                  "where e.employee_id = :e_id and p.quantity > 0 and p.name ILIKE :search order by product_id");
    query.bindValue(":e_id", currentUserId);
    query.bindValue(":search", "%" + search + "%");

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->productTable->insertRow(row);
        for (int col = 0; col < ui->productTable->columnCount(); ++col) {
            ui->productTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
}

void EmployeeMenu::loadUserData(){
    QSqlQuery query(dbconn);
    query.prepare("select e.fullname, e.phone, e.email, w.address from employee e "
                  "join warehouse w on e.warehouse_id = w.warehouse_id where e.employee_id = :e_id ");
    query.bindValue(":e_id", currentUserId);

    if (!query.exec()){
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }
    if (query.next()) {
        const QString fullname = query.value(0).toString();
        ui->welcomeLabel->setText(QString("Добро пожаловать, %1!").arg(fullname));
        ui->nameInput->setText(fullname);
        ui->phoneInput->setText(query.value(1).toString());
        ui->emailInput->setText(query.value(2).toString());
        ui->warehouseInput->setCurrentText(query.value(3).toString());
    } else {
        QMessageBox::warning(this, "Нет данных", "Пользователь с указанным ID не найден.");
    }
}

void EmployeeMenu::fillWarehouseInput(){
    QSqlQuery query(dbconn);
    query.prepare("select address from warehouse");
    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }
    while (query.next()) {
        ui->warehouseInput->addItem(query.value(0).toString());
    }
}

void EmployeeMenu::updateUserData(){
    QSqlQuery query(dbconn);
    query.prepare("update employee set fullname = ?, phone = ?, email = ?, warehouse_id = "
                  "(select warehouse_id from warehouse where address = ?) "
                  "where employee_id = ?");
    query.addBindValue(ui->nameInput->text());
    query.addBindValue(ui->phoneInput->text());
    query.addBindValue(ui->emailInput->text());
    query.addBindValue(ui->warehouseInput->currentText());
    query.addBindValue(currentUserId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
    } else {
        QMessageBox::information(this, "Информация", "Информация успешно обновлена");
    }
}
