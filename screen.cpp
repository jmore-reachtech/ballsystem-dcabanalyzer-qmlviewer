#include "screen.h"
#include <QDebug>

Screen::Screen(QQuickView *view, int screenSaverTimeout, int screenOriginalBrightness, int screenDimBrightness, QObject *parent) :
    QObject(parent)
  ,m_view(view)
  ,m_screenSaverTimer(new QTimer(this))
{
    m_screenSaverEnabled = false;
    m_screenSaverTimeout = screenSaverTimeout;\
    m_screenOriginalBrightness = screenOriginalBrightness;
    m_screenDimBrightness = screenDimBrightness;
    m_dim = false;

    if (m_screenSaverTimeout > 0)
    {
        m_screenSaverEnabled = true;
        //set the original brighness in case the device was shutdown in dim mode
        setBrightness(m_screenOriginalBrightness);
        connect(m_screenSaverTimer, SIGNAL(timeout()),this,SLOT(onScreenSaverTimerTimeout()));
        view->installEventFilter(this);
        m_screenSaverTimer->start(m_screenSaverTimeout * 60 * 1000);
    }
}


bool Screen::save(const QString &path, int quality)
{
    QImage image = m_view->grabWindow();
    return image.save(path, 0, quality);
}

QString Screen::getImageHexString(const QString &path)
{
    QFile imageFile;
    QFileInfo file(path);
    if (file.exists()) {
        imageFile.setFileName(path);
        if (!imageFile.open(QIODevice::ReadOnly)) return "";

        QByteArray blob = imageFile.readAll();
        imageFile.close();
        return QString(blob.toHex());
    }

    return "";
}

bool Screen::createImageFromHexString(const QString &hex, const QString &path)
{
    QByteArray blob = QByteArray::fromHex(hex.toLatin1());
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file to be written";
        return false;
    }
    file.write(blob);
    file.close();

    return true;
}


void Screen::setOriginalBrightness()
{
    setBrightness(m_screenOriginalBrightness);
    m_dim = false;
    m_screenSaverTimer->stop();
    m_screenSaverTimer->start(m_screenSaverTimeout * 60 * 1000);
}


bool Screen::isDim()
{
    return m_dim;
}


bool Screen::isScreenSaverEnabled()
{
    return m_screenSaverEnabled;
}


void Screen::onScreenSaverTimerTimeout()
{
    setBrightness(m_screenDimBrightness);
    m_dim = true;
}


void Screen::setBrightness(int val)
{
    QFile brightness_file(BRIGHTNESS);
    brightness_file.open(QIODevice::ReadWrite);
    QTextStream out(&brightness_file);

    out << QString::number(val).toLatin1() << endl;
    brightness_file.close();
}


bool Screen::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    switch (event->type()) {
    case QEvent::KeyRelease:
    case QEvent::KeyPress:
    case QEvent::MouseButtonPress:
    case QEvent::TouchBegin:
    case QEvent::MouseMove:
        if (m_dim)
            return true;
    case QEvent::MouseButtonRelease:
        if (m_dim)
        {
            setOriginalBrightness();
            return true;
        }
        else
        {
            m_screenSaverTimer->stop();
            m_screenSaverTimer->start(m_screenSaverTimeout * 60 * 1000);
        }
        break;
    default:
        break;
    }

    return false;
}
