#include "productWindowUtils.h"

ProductWindowUtils::ProductWindowUtils(QSqlDatabase db, QWidget *parent)
    : QMainWindow(parent), dbconn(db) {}

void ProductWindowUtils::showError(const QString &title, const QSqlError &error) const {
    QMessageBox::critical(nullptr, title, error.text());
}

int ProductWindowUtils::getProductIdByName(const QString &name) const {
    QSqlError error;
    int id = ProductUtils::getProductIdByName(dbconn, name, &error);
    if (id == -1) {
        QMessageBox::warning(nullptr, "Ошибка", "Не удалось найти товар: " + name + "\n" + error.text());
    }
    return id;
}

void ProductWindowUtils::loadProductsIntoComboBox(QComboBox *comboBox, bool allowNewItem) const {
    QSqlError error;
    QStringList names = ProductUtils::getAllProductNames(dbconn, &error);

    comboBox->blockSignals(true);
    comboBox->clear();

    if (names.isEmpty()) {
        comboBox->addItem("Нет товаров");
    } else {
        comboBox->addItems(names);
    }

    if (allowNewItem) {
        comboBox->insertSeparator(comboBox->count());
        comboBox->addItem("➕ Добавить новый товар...");
    }

    comboBox->blockSignals(false);
}
