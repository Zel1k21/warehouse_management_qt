#ifndef SENDINGWINDOW_H
#define SENDINGWINDOW_H

#pragma once

#include "modules.h"
#include "ui_sendingWindow.h"
#include "productWindowUtils.h"

namespace Ui {
class SendingWindow;
}

class SendingWindow : public ProductWindowUtils {
    Q_OBJECT

public:
    explicit SendingWindow(QSqlDatabase connection, QWidget *parent = nullptr);
    ~SendingWindow();

    void loadSendingWindow(int userId);
    void handleReturnToMenu();
    void addNewSending();
    void loadProductList();
    void handleProductChange(const QString &);
    void loadSendingTable();

signals:
    void goBack();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::SendingWindow *ui;
    QString pendingNewProductName;
    bool isNewProductPending = false;
};


#endif // SENDINGWINDOW_H
