#include "warehouseDialog.h"

WarehouseDialog::WarehouseDialog(QSqlDatabase dbconn, QWidget *parent)
    : QDialog(parent), dbconn(dbconn) {

    setWindowTitle("Сводка по складам");
    resize(600, 400);

    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Адрес", "Вместимость", "Товаров на складе", "Заполненность"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setColumnWidth(0, 180);
    table->setColumnWidth(2, 150);

    QPushButton *printButton = new QPushButton("Печать", this);
    QPushButton *backButton = new QPushButton("Назад", this);

    connect(printButton, &QPushButton::clicked, this, &WarehouseDialog::printTable);
    connect(backButton, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(printButton);
    buttonLayout->addWidget(backButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    loadData();
}

WarehouseDialog::~WarehouseDialog() {}

void WarehouseDialog::loadData() {
    QSqlQuery query(dbconn);
    if (!query.exec("SELECT address, capacity, net_sent, net_sent_percent FROM warehouseFlowSummery")) {
        QMessageBox::critical(this, "Ошибка запроса", query.lastError().text());
        return;
    }

    int row = 0;
    table->setRowCount(0);
    while (query.next()) {
        table->insertRow(row);
        for (int col = 0; col < 4; ++col) {
            table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        ++row;
    }
}

void WarehouseDialog::printTable() {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Landscape);

    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted) return;

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось начать печать");
        return;
    }

    int rows = table->rowCount();
    int cols = table->columnCount();

    int margin = 20;
    int rowHeight = 25;

    // Получаем доступную ширину страницы
    QRect pageRect = painter.viewport();
    int colWidth = (pageRect.width() - 2 * margin) / cols;

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    int y = margin;

    // Заголовки
    for (int col = 0; col < cols; ++col) {
        QString header = table->horizontalHeaderItem(col) ? table->horizontalHeaderItem(col)->text() : "";
        painter.drawText(margin + col * colWidth, y, colWidth, rowHeight, Qt::AlignLeft, header);
        painter.drawRect(margin + col * colWidth, y, colWidth, rowHeight); // рамка
    }
    y += rowHeight;

    // Данные
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QString text;
            QTableWidgetItem* item = table->item(row, col);
            if (item) text = item->text();
            painter.drawText(margin + col * colWidth, y, colWidth, rowHeight, Qt::AlignLeft, text);
            painter.drawRect(margin + col * colWidth, y, colWidth, rowHeight);
        }
        y += rowHeight;

        // Новая страница, если не влезает
        if (y > pageRect.height() - margin) {
            printer.newPage();
            y = margin + rowHeight;
        }
    }

    painter.end();
}
