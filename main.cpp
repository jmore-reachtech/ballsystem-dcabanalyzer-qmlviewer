#include <QGuiApplication>
#include "mainview.h"
#include "maincontroller.h"
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include "systemdefs.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName("Reach Technology");
    QGuiApplication::setOrganizationDomain("reachtech.com");
    QGuiApplication::setApplicationName("Qml-Viewer");
    QGuiApplication::setApplicationVersion(APP_VERSION);
    MainView view;

    QFileInfo settingsFile;

    QStringList args = app.arguments();
    foreach (QString item, args) {
        if(item == "--version" || item == "-v") {
            qDebug() << "QML Viewer " << APP_VERSION;
            return 0;
        }
    }

    QString sb(QGuiApplication::applicationDirPath());
    sb.append(QDir::separator());
    sb.append("settings.conf");
    // check to see if we have a settings file where we started from
    // if not fall back to system hard coded path
    QFileInfo file(sb.toLatin1());
    if (file.exists()) {
        qDebug() << "[QML] using local settings file";
        settingsFile.setFile(file.filePath());
    } else {
        qDebug() << "[QML] using system defined settings file";
        settingsFile.setFile(SYSTEM_SETTINGS_FILE);
    }

    QSettings settings(settingsFile.filePath(),QSettings::NativeFormat);

    settings.beginGroup(SYSTEM_SETTINGS_SECTION);
    int port = settings.value("port",4000).toInt();
    bool parseJSON = settings.value("parse_json", true).toBool();

    MainController controller(&view, port, parseJSON);

    view.setSource(QUrl::fromLocalFile(settings.value("main_view").toString()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    if (settings.value("full_screen",false).toBool()) {
        view.showFullScreen();
    }

    if (settings.value("hide_curosr",false).toBool()) {
        view.setCursor(QCursor( Qt::BlankCursor ));
    }

    settings.endGroup();
    return app.exec();
}
