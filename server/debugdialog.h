#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();
    void closeEvent(QCloseEvent *e);

signals:
    void isClosing();
    void testAddMessage(QString str);

public slots:
    void addMessage(const QString &str);

private slots:
    void on_pushButton_clicked();

private:
    Ui::DebugDialog *ui;
};

#endif // DEBUGDIALOG_H
