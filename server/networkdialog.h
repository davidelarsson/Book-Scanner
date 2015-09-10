#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>

namespace Ui {
class NetworkDialog;
}

class NetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkDialog(QWidget *parent = 0);
    ~NetworkDialog();

private:
    Ui::NetworkDialog *ui;
};

#endif // CONFIGUREDIALOG_H
