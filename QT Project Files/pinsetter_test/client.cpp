#include <QtNetwork>
#include <iostream>

#include "client.h"
#include "namedpipe.h"

#define TCP_SEND_PORT 12321

Client::Client( QObject *parent ) :
        QObject(parent)
{

    // find out which IP to connect to
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    std::cout << "ipAddress: " << (char *)ipAddress.toLatin1().data() << "  port: " << TCP_SEND_PORT << std::endl;

    tcpSocket.connectToHost(ipAddress, (quint16)TCP_SEND_PORT, QTcpSocket::ReadWrite);

    std::cout << "connects" << std::endl;
    connect( &tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)) );
    connect( &tcpSocket, SIGNAL( connected() ), this, SLOT( tcpConnectionUp() ) );
    connect( &tcpSocket, SIGNAL( disconnected() ), this, SLOT( tcpConnectionDown() ) );
    connect( &tcpSocket, SIGNAL( bytesWritten(qint64) ), this, SLOT( tcpWritten(qint64) ) );

}

void Client::tcpConnectionUp()
{
    std::cout << "tcpConnectionUp" << std::endl;
//        ReadPipe();
    NamedPipe mypipe(this);
}

void Client::tcpConnectionDown()
{
    std::cout << "tcpConnectionDown" << std::endl;
}

void Client::tcpWritten(qint64 bytes)
{
    std::cout << "tcpWritten: " << bytes << std::endl;
    NamedPipe mypipe(this);
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
        std::cout << "ERROR: " <<
                (char *)tcpSocket.errorString().toLatin1().data() << std::endl;

}

void Client::sendData(QString data)
{
    QByteArray block;
    QDataStream outData(&block, QIODevice::WriteOnly);
    outData.setVersion(QDataStream::Qt_4_6);

    outData << (quint16)0;	// placeholder which we will reset once we know the size of the payload
    outData << data;
    outData.device()->seek(0);
    outData << (quint16)(block.size() - sizeof(quint16));

    std::cout << "OUTPUT sent" << std::endl;
    tcpSocket.write( block );

}

