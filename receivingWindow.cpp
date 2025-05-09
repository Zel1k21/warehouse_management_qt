#include "receivingWindow.h"

ReceivingWindow::ReceivingWindow(QSqlDatabase connection, int userId, QWidget *parent)
    : ProductWindowUtils(connection, userId, parent), ui(new Ui::ReceivingWindow) {
    ui->setupUi(this);

    connect(ui->goBackButton, &QPushButton::clicked, this, &ReceivingWindow::close);
    connect(ui->productInput, &QComboBox::currentTextChanged, this, &ReceivingWindow::handleProductChange);
    connect(ui->addReceivingButton, &QPushButton::clicked, this, &ReceivingWindow::addNewReceiving);
}

ReceivingWindow::~ReceivingWindow() {
    delete ui;
}

void ReceivingWindow::closeEvent(QCloseEvent* event) {
    emit goBack();
    QMainWindow::closeEvent(event);
}

void ReceivingWindow::loadReceivingWindow(int userId) {
    currentUserId = userId;
    loadProductsIntoComboBox(ui->productInput);
}

void ReceivingWindow::handleProductChange(const QString &text){
    QSqlQuery query(dbconn);
    query.prepare("SELECT p.description, p.cost, w.address FROM product p JOIN warehouse w ON w.warehouse_id = p.warehouse_id WHERE p.name = :name");
    query.bindValue(":name", text);

    ui->productWarehouse->clear();

    if (!query.exec()) {
        showError("Ошибка загрузки информации о товаре", query.lastError());
        return;
    }

    bool hasResult = false;
    while (query.next()) {
        if (!hasResult) {
            ui->productDescription->setPlainText(query.value(0).toString());
            ui->productCost->setText(QString::number(query.value(1).toDouble(), 'f', 2));
            hasResult = true;
        }
        ui->productWarehouse->addItem(query.value(2).toString());
    }

    if (!hasResult) {
        ui->productDescription->clear();
        ui->productCost->clear();
    }

    ui->productDescription->setReadOnly(true);
    ui->productCost->setReadOnly(true);
}

void ReceivingWindow::addNewReceiving() {
    int product_id = getProductIdByName(ui->productInput->currentText());
    if (product_id == -1) return;

    QSqlQuery query(dbconn);
    query.prepare("INSERT INTO receiving (user_id, ammount, receiving_date, product_id) VALUES (?, ?, CURRENT_DATE, ?)");
    query.addBindValue(currentUserId);
    query.addBindValue(ui->productAmmount->text().toDouble());
    query.addBindValue(product_id);

    if (!query.exec()) {
        showError("Ошибка создания поставки", query.lastError());
        return;
    }

    QMessageBox::information(this, "Успешно", "Поставка добавлена.");
}
