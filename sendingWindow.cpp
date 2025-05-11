#include "sendingWindow.h"

SendingWindow::SendingWindow(QSqlDatabase connection, QWidget *parent)
    : ProductWindowUtils(connection, parent), ui(new Ui::SendingWindow)
{
    ui->setupUi(this);

    connect(ui->goBackButton, &QPushButton::clicked, this, &SendingWindow::close);
    connect(ui->productInput, &QComboBox::currentTextChanged, this, &SendingWindow::handleProductChange);
    connect(ui->addSendingButton, &QPushButton::clicked, this, &SendingWindow::addNewSending);
}

SendingWindow::~SendingWindow() {
    delete ui;
}

void SendingWindow::loadSendingWindow(int userId) {
    currentUserId = userId;

    QSqlQuery query(dbconn);
    query.prepare("SELECT address FROM warehouse");

    if (!query.exec()) {
        showError("Ошибка загрузки складов", query.lastError());
        return;
    }

    ui->productWarehouse->clear();
    while (query.next()) {
        ui->productWarehouse->addItem(query.value(0).toString());
    }

    loadProductList();
}

void SendingWindow::closeEvent(QCloseEvent* event) {
    emit goBack();
    QMainWindow::closeEvent(event);
}

void SendingWindow::addNewSending() {
    QString description = ui->productDescription->toPlainText();
    double cost = ui->productCost->text().toDouble();

    QSqlQuery query(dbconn);
    query.prepare("SELECT warehouse_id FROM warehouse WHERE address = :address");
    query.bindValue(":address", ui->productWarehouse->currentText());

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Ошибка", "Склад не найден.");
        return;
    }
    int warehouseId = query.value(0).toInt();

    if (isNewProductPending) {
        QSqlQuery insertProduct(dbconn);
        insertProduct.prepare("INSERT INTO product (name, description, cost, warehouse_id) VALUES (?, ?, ?, ?)");
        insertProduct.addBindValue(pendingNewProductName);
        insertProduct.addBindValue(description);
        insertProduct.addBindValue(cost);
        insertProduct.addBindValue(warehouseId);

        if (!insertProduct.exec()) {
            showError("Ошибка добавления товара", insertProduct.lastError());
            return;
        }

        isNewProductPending = false;
        loadProductList();

        int newIndex = ui->productInput->findText(pendingNewProductName);
        if (newIndex != -1)
            ui->productInput->setCurrentIndex(newIndex);
        else {
            QMessageBox::warning(this, "Ошибка", "Новый товар не найден в списке.");
            return;
        }
    }

    int product_id = getProductIdByName(ui->productInput->currentText());
    if (product_id == -1) return;

    QSqlQuery sendingQuery(dbconn);
    sendingQuery.prepare("INSERT INTO sending (user_id, ammount, sending_date, product_id) VALUES (?, ?, CURRENT_DATE, ?)");
    sendingQuery.addBindValue(currentUserId);
    sendingQuery.addBindValue(ui->productAmmount->text().toDouble());
    sendingQuery.addBindValue(product_id);

    if (!sendingQuery.exec()) {
        showError("Ошибка создания отправки", sendingQuery.lastError());
        return;
    }

    QMessageBox::information(this, "Успешно", "Отправка добавлена.");
}

void SendingWindow::loadProductList() {
    loadProductsIntoComboBox(ui->productInput, true); // метод из ProductWindowUtils
}

void SendingWindow::handleProductChange(const QString &text) {
    if (text == "➕ Добавить новый товар...") {
        bool ok;
        QString newProduct = QInputDialog::getText(this, "Новый товар", "Введите название товара", QLineEdit::Normal, "", &ok);

        if (ok && !newProduct.trimmed().isEmpty()) {
            QSqlError error;
            if (ProductUtils::checkProductExists(dbconn, newProduct, &error)) {
                QMessageBox::information(this, "Информация", "Товар с таким названием уже существует.");
                int index = ui->productInput->findText(newProduct);
                ui->productInput->setCurrentIndex(index != -1 ? index : 0);
                return;
            } else if (error.isValid()) {
                showError("Ошибка проверки", error);
                return;
            }

            pendingNewProductName = newProduct;
            isNewProductPending = true;

            int insertIndex = ui->productInput->count() - 2;
            ui->productInput->insertItem(insertIndex, newProduct);
            ui->productInput->setCurrentIndex(insertIndex);

            ui->productDescription->clear();
            ui->productCost->clear();
            ui->productDescription->setReadOnly(false);
            ui->productCost->setReadOnly(false);
        } else {
            ui->productInput->setCurrentIndex(0);
        }
    } else {
        QSqlQuery getProductInfo(dbconn);
        getProductInfo.prepare("SELECT description, cost FROM product WHERE name = :name");
        getProductInfo.bindValue(":name", text);

        if (!getProductInfo.exec()) {
            showError("Ошибка загрузки информации о товаре", getProductInfo.lastError());
            return;
        }

        if (getProductInfo.next()) {
            ui->productDescription->setPlainText(getProductInfo.value(0).toString());
            ui->productCost->setText(QString::number(getProductInfo.value(1).toDouble(), 'f', 2));
            isNewProductPending = false;
        } else {
            ui->productDescription->clear();
            ui->productCost->clear();
        }

        ui->productDescription->setReadOnly(true);
        ui->productCost->setReadOnly(true);
    }
}
