#ifndef SERVER_H
#define SERVER_H

#include <QObject>

#include <QTcpSocket>
#include <QTcpServer>
#include <QBuffer>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    Server(QString address, int port);
    ~Server();
    QImage *getImage();
    int getMaxZoom();
    void askForMaxZoom();
    void setAutoFocus(bool);
    void setZoom(int zoom);
    void focus();
    void terminateServer();

private:
    QTcpServer *server;
    QTcpSocket *socket;
    QByteArray buffer;
    QImage *image;
    quint32 size;
    quint32 maxZoom;

    enum state_t { LISTENING, READING_IMAGE, READING_MAX_ZOOM, READING_ZOOM };
    state_t state = LISTENING;


signals:
    void debug(QString str);
    void connectionEstablished();
    void disconnected();
    void imageUpdated();
    void maxZoomReceived();

private slots:
    void newConnection();
    void readyRead();
    void onDisconnect();
    void askForImage();

};

#endif // SERVER_H
