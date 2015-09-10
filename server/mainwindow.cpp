#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>
#include "configuredialog.h"
#include "listeningdialog.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
 //   ConfigureDialog configureDialog;
 //   configureDialog.setModal(true);
    QSettings settings("MadSoft", "BookScanner");
 //   settings.setValue("isConfigured", false);

    std::cout << QString("Listening on %1:%2").arg(settings.value("ip").toString()).arg(settings.value("port").toUInt()).toStdString();
    std::cout.flush();

    if(!settings.value("isConfigured").toBool())
    {
        ConfigureDialog configureDialog;
        configureDialog.setModal(true);
        configureDialog.ip = "127.0.0.1";
        configureDialog.port = 10000;
        if(configureDialog.exec() != configureDialog.Accepted)
            exit(0);
        ip = configureDialog.ip;
        port = configureDialog.port;
        settings.setValue("isConfigured", true);
        settings.setValue("ip", ip);
        settings.setValue("port", port);
    } else
    {
        ip = settings.value("ip").toString();
        port = settings.value("port").toInt();
    }

    // Start listening!
    on_actionDebugWindow_triggered();
    startListening();
    ui->previewArea->setText("Listening...");
    ui->snapButton->setEnabled(false);
    ui->zoomSlider->setEnabled(false);
    ui->focusButton->setEnabled(false);
    ui->zoomLabel->setEnabled(false);
    ui->saveButton->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startListening()
{
    server = new Server(ip, port);
    connect(server, SIGNAL(connectionEstablished()), this, SLOT(connectionEstablished()));
    emit debug(QString("Listening on %1:%2").arg(ip).arg(port));
}

void MainWindow::stopListening()
{
    server->disconnect();
//    server->terminateServer();

//    delete server;
//    server = NULL;
}

void MainWindow::connectionEstablished()
{
    emit debug("Connection established!");
    ui->previewArea->setText("No preview");
    connect(server, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(server, SIGNAL(debug(QString)), this, SLOT(debugMessage(QString)));
    connect(server, SIGNAL(imageUpdated()), this, SLOT(imageUpdated()));
    connect(server, SIGNAL(maxZoomReceived()), this, SLOT(maxZoomReceived()));
    connect(this, SIGNAL(askForImage()), server, SLOT(askForImage()));

    ui->snapButton->setEnabled(true);
    ui->focusButton->setEnabled(true);

    server->askForMaxZoom();

}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(debugVisible)
        debugDialog->close();
}

void MainWindow::on_actionAbout_Book_Scanner_triggered()
{
    QMessageBox::about(this, "About BookScanner",
                        "A book scanning server program");

}

void MainWindow::clientDisconnected()
{
    debugMessage("Disconnected!");
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}


void MainWindow::on_actionDebugWindow_triggered()
{
    if(!debugVisible)
    {
        debugVisible = true;
        debugDialog = new DebugDialog();
        debugDialog->setModal(false);
        connect(debugDialog, SIGNAL(isClosing()), this, SLOT(debugWindowClosed()));
        connect(this, SIGNAL(debug(QString)), debugDialog, SLOT(addMessage(QString)));
        debugDialog->show();
    } else
    {
        debugDialog->close();
        debugVisible = false;
    }
}

void MainWindow::debugMessage(QString str)
{
    if(!debugVisible)
        return;

    emit debug(str);
}

void MainWindow::on_actionQuit_triggered()
{
    exit(0);
}

void MainWindow::debugWindowClosed()
{
    debugVisible = false;
}

void MainWindow::on_snapButton_clicked()
{
    emit debug("SNAP!");
    ui->snapButton->setEnabled(false);
    ui->zoomLabel->setEnabled(false);
    ui->zoomSlider->setEnabled(false);
    ui->focusButton->setEnabled(false);
    emit askForImage();
}

void MainWindow::imageUpdated()
{
    image = server->getImage();
    QSize s = ui->previewArea->size();
    pixmap = QPixmap::fromImage(image->scaled(s, Qt::KeepAspectRatio));
    if(autoSave)
    {
        saveImage();
        if(autoIncrement)
            nextPage();
    }
    ui->previewArea->setPixmap(pixmap);
    ui->snapButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
    ui->focusButton->setEnabled(true);
    if(server->getMaxZoom() != 0)
    {
        ui->zoomLabel->setEnabled(true);
        ui->zoomSlider->setEnabled(true);
    }
}

void MainWindow::maxZoomReceived()
{
    debug(QString("Max zoom is: %1").arg(server->getMaxZoom()));
    ui->zoomSlider->setMaximum(server->getMaxZoom());
    ui->zoomLabel->setEnabled(true);
    ui->zoomSlider->setEnabled(true);
}

void MainWindow::saveImage()
{
    if(!image)
        return;

    QString name = QString(ui->fileName->text())
            .append(QString("%1").arg(ui->pageNumber->value(), 4, 10, QChar('0')))
            .append(".jpeg");

    if(image->save(name))
        emit debug("Image saved!");
    else
        emit debug("Image NOT saved!");
    emit debug(QString("Saved image as: ").append(name));
}

void MainWindow::nextPage()
{
    ui->pageNumber->setValue(ui->pageNumber->value() + 1);
}

void MainWindow::on_saveButton_clicked()
{
    saveImage();
}

void MainWindow::on_autoSave_toggled(bool checked)
{
    if(checked)
    {
        emit debug("Automatic saving enabled!");
        ui->snapButton->setText("Snap and save!");
        ui->saveButton->setEnabled(false);
        autoSave = true;
    } else
    {
        emit debug("Automatic saving disabled!");
        ui->snapButton->setText("Snap!");
        ui->saveButton->setEnabled(true);
        autoSave = false;
    }
}

void MainWindow::on_autoIncrement_toggled(bool checked)
{
    autoIncrement = checked;
}

void MainWindow::on_autoFocus_toggled(bool checked)
{
    ui->focusButton->setEnabled(!checked);
    server->setAutoFocus(checked);
}

void MainWindow::on_focusButton_clicked()
{
    server->focus();
}

void MainWindow::on_action_Configure_triggered()
{
    ConfigureDialog configureDialog;
    configureDialog.setModal(true);
    configureDialog.port = port;
    configureDialog.ip = ip;
    if(configureDialog.exec() != configureDialog.Accepted)
        return;

    ip = configureDialog.ip;
    port = configureDialog.port;
    QSettings settings("MadSoft", "BookScanner");
    settings.setValue("ip", ip);
    settings.setValue("port", port);
    stopListening();
    startListening();
    ui->previewArea->setText("Listening...");

}

void MainWindow::on_zoomSlider_sliderReleased()
{
    emit debug(QString("Zoom set to %1").arg(ui->zoomSlider->value()));
    server->setZoom(ui->zoomSlider->value());
}
