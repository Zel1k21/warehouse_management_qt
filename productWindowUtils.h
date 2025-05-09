#ifndef PRODUCTWINDOWUTILS_H
#define PRODUCTWINDOWUTILS_H
#include "modules.h"
#include "productUtils.h"

class ProductWindowUtils : public QMainWindow {
    Q_OBJECT
protected:
    QSqlDatabase dbconn;
    int currentUserId;

    explicit ProductWindowUtils(QSqlDatabase db, int userId, QWidget *parent = nullptr);
    void showError(const QString &title, const QSqlError &error) const;
    int getProductIdByName(const QString &name) const;
    void loadProductsIntoComboBox(QComboBox *comboBox, bool allowNewItem = false) const;
};

#endif // PRODUCTWINDOWUTILS_H
