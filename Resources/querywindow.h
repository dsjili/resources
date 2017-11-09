#ifndef QUERYWINDOW_H
#define QUERYWINDOW_H

#include <QDialog>

namespace Ui {
class QueryWindow;
}

class QueryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit QueryWindow(QWidget *parent = 0);
    ~QueryWindow();

public slots:
    void updateText(QString text);
protected slots:

private slots:

private:
    Ui::QueryWindow *ui;
};

#endif // QUERYWINDOW_H
