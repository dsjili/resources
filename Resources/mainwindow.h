#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QtWidgets>
#include <QtSql/QSqlQueryModel>
#include "querywindow.h"
#include "DynLib.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pathButton_clicked();

    void on_queryButton_clicked();

    void on_filterButton_clicked();

    void on_pathText_returnPressed();

    void on_queryText_returnPressed();

    void on_actionExit_triggered();

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_resourcesTable_cellClicked(int row, int column);

    void insertRows(QString name, QString location, QString type);

    void on_browseButton_clicked();

    void updateTabs(int row);
private:
    Ui::MainWindow *ui;
    QueryWindow *qw;
    IResourceCollection *collection = nullptr;
    QProcess *qp;
    int row = 0;

};

#endif // MAINWINDOW_H
