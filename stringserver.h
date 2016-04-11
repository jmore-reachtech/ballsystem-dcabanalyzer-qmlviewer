#ifndef STRINGSERVER_H
#define STRINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class StringServer : public QObject
{
    Q_OBJECT

public:
    explicit StringServer(QObject *parent = 0, int port = 4000);
    ~StringServer();

signals:
    void MessageAvailable(QByteArray ba);
    void ClientConnected(void);
    void ClientDisconnected(void);

public slots:
    bool Send(QString msg);

private slots:
    void onClientConnected(void);
    void onClientReadyRead(void);
    void onClientDisconnected(void);

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
};

#endif // STRINGSERVER_H
