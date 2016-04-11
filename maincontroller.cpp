#include <QQuickItem>
#include <QQmlContext>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>

#include "maincontroller.h"

MainController::MainController(MainView *view, int port, bool parseJSON, QObject *parent) : QObject(parent)
  , m_view(view)
  , m_stringServer(new StringServer(this, port))
{
    m_clients = 0;
    m_parseJSON = parseJSON;
    m_view->rootContext()->setContextProperty("controller",this);

    connect(m_stringServer, SIGNAL(MessageAvailable(QByteArray))
            , this, SLOT(onMessageAvailable(QByteArray)));
    connect(m_stringServer, SIGNAL(ClientConnected()), this, SLOT(onClientConnected()));
    connect(m_stringServer, SIGNAL(ClientDisconnected()), this, SLOT(onClientDisconnected()));

     m_view->show();
}

MainController::~MainController()
{
    if (m_stringServer) {
        delete m_stringServer;
    }
}

bool MainController::sendString(QString msg)
{
    qDebug() << "[QML]: Sent: " << QTime::currentTime().toString("hh:mm:ss:zzz");
    m_stringServer->Send(msg);
    qDebug() << "[QML]: " << msg;
    return true;
}

void MainController::onMessageAvailable(QByteArray ba)
{
    ba.replace('\r', "");
    ba.replace('\n', "");
    if (ba.length() == 0) {
        return;
    }

    // Our protocol is obj.prop=value, so split message
    if (ba.contains("=") && ba.contains("."))
    {
        QList<QByteArray> values = ba.split('=');
        QList<QByteArray> items = values[0].split('.');

        if (values.count() != 2 && items.count() != 2) {
            qDebug() << "Message syntax error.";
            return;
        }
        else
        {
            qDebug() << "[sbRIO]: " << items[0] << "." << items[1] << ": " << values[1];
            if (m_parseJSON)
                setJsonProperty(items[0], items[1], values[1]);
            else
                setProperty(items[0], items[1], values[1]);
        }
    }
    else{
        qDebug() << "Invalid Message.";
    }

}

void MainController::onClientConnected()
{
    m_mutex.lock();
    m_clients++;
    m_mutex.unlock();
    emit readyToSend();
    qDebug() << "Clients connected: " << m_clients << endl;
}

void MainController::onClientDisconnected()
{
    m_mutex.lock();
    m_clients--;
    m_mutex.unlock();

    if (m_clients == 0)
    {
        emit notReadyToSend();
    }

    qDebug() << "Clients connected: " << m_clients << endl;
}


void MainController::setJsonProperty(QString object, QString property, QString value)
{
    QQuickItem *obj =  m_view->rootObject()->findChild<QQuickItem*>(object);
    if (!obj) {
        qDebug() << "Item [" << object << "] not found" << endl;
        return;
    }

    //Now lets parse the json
    QJsonDocument doc(QJsonDocument::fromJson(value.toUtf8()));
    QVariant jsonVariant;

    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            jsonVariant = doc.toVariant();
        }
        else
        {
            qDebug() << "Document is not an object" << endl;
            return;
        }
    }
    else
    {
        qDebug() << "Invalid JSON...\n" << value << endl;
        return;
    }

    bool found = obj->setProperty(property.toLatin1(), jsonVariant);

    if (!found) {
        qDebug() << "Property [" << property << "] invalid" << endl;
    }
}


void MainController::setProperty(QString object, QString property, QString value)
{
    QQuickItem *obj =  m_view->rootObject()->findChild<QQuickItem*>(object);
    if (!obj) {
        qDebug() << "Item [" << object << "] not found" << endl;
        return;
    }

    bool found = obj->setProperty(property.toLatin1(), value);

    if (!found) {
        qDebug() << "Property [" << property << "] invalid" << endl;
    }
}
