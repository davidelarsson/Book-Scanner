#include "listeningdialog.h"
#include "ui_listeningdialog.h"
#include "configuredialog.h"
#include "helpdialog.h"
#include <QMessageBox>
#include <iostream>

ListeningDialog::ListeningDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListeningDialog)
{
    ui->setupUi(this);
}

ListeningDialog::~ListeningDialog()
{
    delete ui;
}

void ListeningDialog::on_configureButton_clicked()
{
    ConfigureDialog dialog;
    dialog.setModal(true);
    if(dialog.exec() != dialog.Accepted)
        return;

    ip = dialog.ip;
    port = dialog.port;
    ui->addressLabel->setText(QString(ip).append(":%1").arg(port));
    if(!ui->listenButton->isEnabled())
        ui->listenButton->setEnabled(true);
    if(isListening)
    {
        stopListening();
        startListening();
    }

}

void ListeningDialog::startListening()
{
    isListening = true;
    server = new Server(ip, port);
    connect(server, SIGNAL(connectionEstablished()), this, SLOT(connectionEstablished()));
}

void ListeningDialog::stopListening()
{
    server->disconnect();
    //delete server;
    //server = NULL;
    // Stop listening server
    isListening = false;
}

void ListeningDialog::connectionEstablished()
{
    accept();
}


void ListeningDialog::on_quitButton_clicked()
{
    exit(0);
}


void ListeningDialog::on_listenButton_clicked()
{
    if(!isListening)
    {
        ui->stateLabel->setText("Waiting for incoming connection...");
        ui->listenButton->setText("Stop listening");
        startListening();
       // this->accept();
    } else
    {
        ui->stateLabel->setText("Not listening");
        ui->listenButton->setText("Start listening");
        stopListening();
    }
}


void ListeningDialog::on_helpButton_clicked()
{
    HelpDialog dialog;
    dialog.exec();
}

Server *ListeningDialog::getServer()
{
    return server;
}

