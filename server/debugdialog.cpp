#include "debugdialog.h"
#include "ui_debugdialog.h"

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::closeEvent(QCloseEvent *e)
{
    emit isClosing();
}

void DebugDialog::on_pushButton_clicked()
{
    this->close();
}

void DebugDialog::addMessage(const QString &str)
{
    ui->debugOutput->appendPlainText(str.toUtf8());
}
