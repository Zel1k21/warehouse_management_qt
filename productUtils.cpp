#include "productUtils.h"

QStringList ProductUtils::getAllProductNames(QSqlDatabase db, QSqlError *error) {
    QStringList names;
    QSqlQuery query(db);
    query.prepare("SELECT name FROM product ORDER BY product_id");

    if (!query.exec()) {
        if (error) *error = query.lastError();
        return names;
    }

    while (query.next())
        names << query.value(0).toString();

    return names;
}

int ProductUtils::getProductIdByName(QSqlDatabase db, const QString &name, QSqlError *error) {
    QSqlQuery query(db);
    query.prepare("SELECT product_id FROM product WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec() || !query.next()) {
        if (error) *error = query.lastError();
        return -1;
    }

    return query.value(0).toInt();
}

bool ProductUtils::checkProductExists(QSqlDatabase db, const QString &name, QSqlError *error) {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM product WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        if (error) *error = query.lastError();
        return false;
    }

    return query.next() && query.value(0).toInt() > 0;
}
