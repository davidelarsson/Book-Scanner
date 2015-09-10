#include "server.h"
#include <QMessageBox>
#include <iostream>
#include <QtEndian>
#include <QBuffer>

Server::Server(QObject *parent) :
    QObject(parent)
{
}

Server::Server(QString address, int port)
{
    server = new QTcpServer();
    image = new QImage();
    if(!server->listen(QHostAddress(address), port))
        debug("Could not start listening!");
    else
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    maxZoom = 0;
}

Server::~Server()
{
   // socket->close();
   // server->close();
}

void Server::newConnection()
{
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

    /* No more connections, pleeeease */
    server->close();
    std::cout << "Connection established!";
    emit connectionEstablished();
}

void Server::terminateServer()
{
    socket->disconnectFromHost();
}

void Server::readyRead()
{
    emit debug("We have data to read!");
    quint8 message;

    // command (1 byte) + size of image (4 bytes)
    const int headerSize = 5;

    // Start of a new package
    if(state == LISTENING)
    {
        if(socket->bytesAvailable() < headerSize)
            return;

        buffer.append(socket->read(headerSize));

        // First byte of header is the command
        message = *buffer.constData();

        // And the next four bytes is an unsigned int denoting the size of the package
        size = qFromBigEndian<quint32>((const uchar *) buffer.constData() + 1);

        switch(message)
        {

        case 47: // JPEG image
            emit debug("We got a JPEG image!");
            emit debug(QString("Size of image is: %1").arg(size));
            state = READING_IMAGE;

            // Done with the header, now fill buffer with the actual image data
            buffer.clear();
            break;

        case 77: // Max zoom
            emit debug("We got max zoom");
            state = READING_MAX_ZOOM;
            emit debug(QString("Package size is: %1").arg(size)); // Always four bytes

            // Get the four-byte max zoom into the buffer
            buffer.clear();
            break;

        default:
            emit debug("Unknown message!");
            socket->close();
            break;
        }
    }

    buffer.append(socket->read(size));

    // Do we have the whole package?
    if(buffer.size() < size)
    {
        emit debug(QString("NOT done yet. We have read: %1 out of %2 bytes").arg(buffer.size()).arg(size));
        return;
    }
    emit debug(QString("Package DONE! We have read: %1 bytes").arg(buffer.size()));

    switch(state)
    {
    case READING_IMAGE:

        if(image->loadFromData(buffer, "JPEG"))
        {
            emit debug("Image loaded from socket!");
            emit imageUpdated();
        } else
        {
            emit debug("Image load FAILED!");
        }
        buffer.clear();
        state = LISTENING;
        break;

    case READING_MAX_ZOOM:
        maxZoom = qFromBigEndian<quint32>((const uchar *) buffer.constData());
        buffer.clear();
        emit debug(QString("Max zoom is: %1").arg(maxZoom));
        emit maxZoomReceived();
        state = LISTENING;
        break;

    default:
        break;

    }
}

void Server::askForImage()
{
    QByteArray data;
    data.append(42);
    socket->write(data);
    socket->flush();
}

void Server::setAutoFocus(bool enabled)
{
    QByteArray data;
    if(enabled)
        data.append(52); // Enable autofocus
    else
        data.append(53); // Disable autofocus

    socket->write(data);
    socket->flush();
}

void Server::focus()
{
    QByteArray data;
    data.append(62); // Focus
    socket->write(data);
    socket->flush();
}

void Server::setZoom(int zoom)
{

    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << (quint8) 72;  // Set zoom
    ds << (quint32) 4;  // Always a four-byte int as data
    ds << (quint32) zoom;
    socket->write(data);
    socket->flush();
}

void Server::askForMaxZoom()
{
    QByteArray data;
    data.append(82); // Ask for max zoom
    socket->write(data);
    socket->flush();

}

void Server::onDisconnect()
{
    emit disconnected();
}

QImage *Server::getImage()
{
    return image;
}

int Server::getMaxZoom()
{
    return maxZoom;
}
