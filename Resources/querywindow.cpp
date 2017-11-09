#include "querywindow.h"
#include "ui_querywindow.h"

QueryWindow::QueryWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryWindow)
{
    ui->setupUi(this);
}

QueryWindow::~QueryWindow()
{
    delete ui;
}

void QueryWindow::updateText(QString text) {
    ui->queryTextBrowser->append(text);
}
