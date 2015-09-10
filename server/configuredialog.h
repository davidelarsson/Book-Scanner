#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QHostAddress>

namespace Ui {
class ConfigureDialog;
}

class ConfigureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureDialog(QWidget *parent = 0);
    int exec();
    ~ConfigureDialog();
    int port;
    QString ip;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ConfigureDialog *ui;
    void populateIPs();

    QList<QHostAddress> addresses;
};

#endif // CONFIGUREDIALOG_H
