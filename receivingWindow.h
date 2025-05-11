#ifndef RECEIVINGWINDOW_H
#define RECEIVINGWINDOW_H

#include "modules.h"
#include "ui_receivingWindow.h"
#include "productWindowUtils.h"

namespace Ui {
class ReceivingWindow;
}

class ReceivingWindow: public ProductWindowUtils {
    Q_OBJECT

public:
    explicit ReceivingWindow(QSqlDatabase connection, QWidget *parent = nullptr);
    ~ReceivingWindow();
    void loadReceivingWindow(int);
    void loadProdutList();
    void handleProductChange(const QString &text);
    void addNewReceiving();

signals:
    void goBack();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ReceivingWindow *ui;
    QSqlDatabase dbconn;
    int currentUserId;
};

#endif // RECEIVINGWINDOW_H
