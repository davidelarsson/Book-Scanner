#include "configuredialog.h"
#include "ui_configuredialog.h"
#include <QNetworkInterface>

ConfigureDialog::ConfigureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureDialog)
{
    ui->setupUi(this);
    populateIPs();
}

ConfigureDialog::~ConfigureDialog()
{
    delete ui;
}

int ConfigureDialog::exec()
{
    ui->portSpinner->setValue(port);
    ui->ip->setCurrentIndex(ui->ip->findText(ip));
    return QDialog::exec();
}

void ConfigureDialog::populateIPs()
{
    addresses = QNetworkInterface::allAddresses();
    for(int i = 0; i < addresses.count(); i++)
        ui->ip->addItem(addresses[i].toString());
}


void ConfigureDialog::on_buttonBox_accepted()
{
    if (ui->ip->currentText() == NULL)
            this->reject();

    ip = ui->ip->currentText();
    port = ui->portSpinner->value();
    this->accept();
}

void ConfigureDialog::on_buttonBox_rejected()
{
    this->reject();
}
