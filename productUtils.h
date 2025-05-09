#ifndef PRODUCTUTILS_H
#define PRODUCTUTILS_H
#include "modules.h"

class ProductUtils {
public:
    static QStringList getAllProductNames(QSqlDatabase db, QSqlError *error = nullptr);
    static int getProductIdByName(QSqlDatabase db, const QString &name, QSqlError *error = nullptr);
    static bool checkProductExists(QSqlDatabase db, const QString &name, QSqlError *error = nullptr);
};

#endif // PRODUCTUTILS_H
