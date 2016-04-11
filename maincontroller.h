#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QMutex>
#include "mainview.h"
#include "stringserver.h"

class MainController : public QObject
{
    Q_OBJECT

public:
    explicit MainController(MainView *view, int port, bool parseJSON, QObject *parent = 0);
    ~MainController();

signals:
    void readyToSend();
    void notReadyToSend();

public slots:
    Q_INVOKABLE bool sendString(QString msg);


private slots:
    void onMessageAvailable(QByteArray ba);
    void onClientConnected(void);
    void onClientDisconnected(void);
    void setJsonProperty(QString object, QString property, QString value);
    void setProperty(QString object, QString property, QString value);

private:
    MainView *m_view;
    StringServer *m_stringServer;
    qint32 m_clients;
    QMutex m_mutex;
    bool m_parseJSON;
};

#endif // MAINCONTROLLER_H
