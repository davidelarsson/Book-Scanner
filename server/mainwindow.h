#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "debugdialog.h"
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void closeEvent(QCloseEvent *e);
    ~MainWindow();

signals:
    void debug(const QString &str);
    void askForImage();

public slots:
    void debugWindowClosed();
    void debugMessage(QString str);

private slots:
    void on_actionAbout_Book_Scanner_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionDebugWindow_triggered();
    void on_actionQuit_triggered();
    void on_snapButton_clicked();
    void imageUpdated();
    void maxZoomReceived();
    void clientDisconnected();
    void on_saveButton_clicked();
    void on_autoSave_toggled(bool checked);
    void on_autoIncrement_toggled(bool checked);
    void on_autoFocus_toggled(bool checked);
    void on_focusButton_clicked();
    void connectionEstablished();

    void on_action_Configure_triggered();

    void on_zoomSlider_sliderReleased();

private:
    void saveImage();
    void nextPage();
    void startListening();
    void stopListening();
    QString ip;
    quint32 port;
    Ui::MainWindow *ui;
    bool debugVisible = false;
    bool autoIncrement = true;
    DebugDialog *debugDialog;
    Server *server = NULL;
    QImage *image = NULL;
    QPixmap pixmap;
    bool autoSave = false;
    bool isConfigured = false;
    bool isConnected = false;
};

#endif // MAINWINDOW_H
