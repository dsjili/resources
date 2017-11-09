#include "mainwindow.h"
#include "ui_mainwindow.h"

typedef const char * TUtf8;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->resourcesTable->verticalHeader()->setVisible(0);
    ui->resourcesTable->hideColumn(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pathButton_clicked()
{
    if (collection == nullptr) {
        ui->logBox->append("Open or create New database");
        return;
    }

    // Here we get the text from the text box
    QString path = ui->pathText->text();

    // Here we recursively add all resources in path to the collection and database
    // TODO

    TId id;
    collection->AddFile(path.toStdString().data(), id);
    char * buf = (char*)id;
    ui->logBox->append(buf);

    // Here we do a next thing
    // TODO

    ui->logBox->append(path);
}

void MainWindow::on_queryButton_clicked()
{
    if (collection == nullptr) {
        ui->logBox->append("Open or create New database");
        return;
    }

    /*
    TResourceList list;
    collection->ResourceList(list);
    for(TResourceList::iterator it = list.begin(); it != list.end(); ++it) {
        QString p = (*it)->Path();
        ui->logBox->append(p);
    }
    */

    insertRows("david.txt", "./", "1");

    QString query = ui->queryText->text();
    ui->logBox->append(query);
    qw = new QueryWindow(this);
    qw->updateText(query);
    //qw->show();
}

void MainWindow::on_filterButton_clicked()
{
    if (collection == nullptr) {
        ui->logBox->append("Open or create New database");
        return;
    }
    QString filters = ui->filterBox1->currentText() + ", " +
            ui->filterBox2->currentText() + ", " +
            ui->filterBox3->currentText();

    insertRows(ui->filterBox1->currentText(), ui->filterBox2->currentText(), ui->filterBox3->currentText());

    ui->logBox->append(filters);
    qw = new QueryWindow(this);
    qw->updateText(filters);
    //qw->show();
}

void MainWindow::on_pathText_returnPressed()
{
    on_pathButton_clicked();
}

void MainWindow::on_queryText_returnPressed()
{
    on_queryButton_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionNew_triggered()
{
    bool ok;
    QString fn = QInputDialog::getText(this, tr("New Database"),
                                       tr("Enter database filename:"), QLineEdit::Normal,
                                       tr("database.db"), &ok);
    if (ok) {
        if (collection != nullptr) DestroyResourceCollection(collection);
        // Create collection here
        collection = CreateResourceCollection(fn.toUtf8());
        if (collection != nullptr) {
            ui->logBox->append("collection created succesfully");
        }
    }

}

void MainWindow::on_actionOpen_triggered()
{
    QString fn = QFileDialog::getOpenFileName(this, "Open Database File","./", tr("Database Files (*.db)"));

    if (fn != nullptr) {
        if (collection != nullptr) DestroyResourceCollection(collection);
        // Create collection here
        collection = CreateResourceCollection(fn.toUtf8());
        if (collection != nullptr) {
            ui->logBox->append("collection created succesfully");
        }
    }
}


void MainWindow::on_resourcesTable_cellClicked(int row, int column)
{
    updateTabs(row);
}

void MainWindow::insertRows(QString name, QString location, QString type)
{
    ui->resourcesTable->insertRow(row);
    QTableWidgetItem *name_i = new QTableWidgetItem(name);
    QTableWidgetItem *location_i = new QTableWidgetItem(location);
    QTableWidgetItem *type_i = new QTableWidgetItem(type);
    ui->resourcesTable->setItem(row,1,name_i);
    ui->resourcesTable->setItem(row,2,location_i);
    ui->resourcesTable->setItem(row++,3,type_i);
}

void MainWindow::on_browseButton_clicked()
{
    QString fn = QFileDialog::getExistingDirectory(this, "Select Directory", "./");

    if (fn != nullptr) ui->pathText->setText(fn);
}

void MainWindow::updateTabs(int row)
{
    ui->propTabText->clear();
    ui->detTabText->clear();
    ui->propTabText->appendPlainText(ui->resourcesTable->item(row,1)->text());
    ui->propTabText->appendPlainText(ui->resourcesTable->item(row,2)->text());
    ui->propTabText->appendPlainText(ui->resourcesTable->item(row,3)->text());

    ui->detTabText->appendPlainText(ui->resourcesTable->item(row,1)->text());
    ui->detTabText->appendPlainText(ui->resourcesTable->item(row,2)->text());
    ui->detTabText->appendPlainText(ui->resourcesTable->item(row,3)->text());
}
