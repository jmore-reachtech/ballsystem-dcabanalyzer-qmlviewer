#include <QDebug>

#include "stringserver.h"

StringServer::StringServer(QObject *parent, int port) : QObject(parent)
  ,m_server(new QTcpServer)
{
    this->m_server->listen(QHostAddress::Any, port);

    qDebug() << "Server listening on port " << port << endl;

    connect(m_server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
}

StringServer::~StringServer()
{
    if(m_server) {
        delete(m_server);
    }
}

bool StringServer::Send(QString msg)
{
    int count = m_clients.size();

    for(int i = 0; i < count; i++) {
       if(m_clients[i]->state() == QAbstractSocket::ConnectedState) {
           m_clients[i]->write(msg.append("\r\n").toLatin1());
       }
    }

    return true;
}

void StringServer::onClientConnected()
{
    qDebug() << "Handling new connection" << endl;

    QTcpSocket *s = m_server->nextPendingConnection();
    connect(s, SIGNAL(readyRead()), this,SLOT(onClientReadyRead()));
    connect(s, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

    m_clients.append(s);
    emit ClientConnected();
}

void StringServer::onClientReadyRead()
{
    int count = m_clients.size();

    for(int i = 0; i < count; i++) {
        while (m_clients[i]->bytesAvailable() && m_clients[i]->canReadLine()) {
            QByteArray ba = m_clients[i]->readLine();
            //qDebug() << "Data available from client:" << ba << endl;
            emit MessageAvailable(ba);
        }
    }

}

void StringServer::onClientDisconnected()
{
    int count = m_clients.size();

    for(int i = 0; i < count; i++) {
       if(m_clients[i]->state() == QAbstractSocket::UnconnectedState) {
           qDebug() << "Removing client: " << i << endl;
           disconnect(m_clients[i], SIGNAL(readyRead()), this,SLOT(onClientReadyRead()));
           disconnect(m_clients[i], SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
           m_clients[i]->deleteLater();
           m_clients.removeAt(i);
           emit ClientDisconnected();
           break;
       }
    }
}
