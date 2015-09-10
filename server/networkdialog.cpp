#include "networkdialog.h"
#include "ui_networkdialog.h"

NetworkDialog::NetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDialog)
{
    ui->setupUi(this);
}

NetworkDialog::~NetworkDialog()
{
    delete ui;
}
