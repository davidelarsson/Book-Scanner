#ifndef LISTENINGDIALOG_H
#define LISTENINGDIALOG_H

#include <QDialog>
#include "server.h"

namespace Ui {
    class ListeningDialog;
}

class ListeningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListeningDialog(QWidget *parent = 0);
    ~ListeningDialog();
    Server *getServer();

private slots:
    void on_configureButton_clicked();
    void on_listenButton_clicked();
    void on_quitButton_clicked();
    void on_helpButton_clicked();
    void connectionEstablished();


private:
    Ui::ListeningDialog *ui;
    QString ip = NULL;
    Server *server = NULL;
    bool isListening = false;
    int port;

    void startListening();
    void stopListening();
};

#endif // LISTENINGDIALOG_H
