#ifndef SCREEN_H
#define SCREEN_H

#include <QObject>
#include <QImage>
#include <QQuickView>
#include <QTimer>
#include <QSettings>
#include "systemdefs.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#define BRIGHTNESS "/sys/class/backlight/backlight.22/brightness"

class Screen : public QObject
{
    Q_OBJECT
public:
    explicit Screen(QQuickView *view, int screenSaverTimeout, int screenOriginalBrightness, int screenDimBrightness, QObject *parent = 0);

signals:

public slots:
     bool save(const QString &path, int quality);
     QString getImageHexString(const QString &path);
     bool createImageFromHexString(const QString &hex, const QString &path);
     void setOriginalBrightness();
     bool isDim();
     bool isScreenSaverEnabled();
     void setBrightness(int val);

private slots:
    void onScreenSaverTimerTimeout();

private:
    QQuickView *m_view;
    QTimer *m_screenSaverTimer;
    int m_screenSaverTimeout;
    int m_screenOriginalBrightness;
    int m_screenDimBrightness;
    bool m_dim;
    bool m_screenSaverEnabled;

    bool eventFilter(QObject *obj, QEvent *event);

};

#endif // SCREEN_H
