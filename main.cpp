#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

void initSettings();

int main(int argc, char *argv[])
{
    QApplication weatherApp(argc, argv);
    weatherApp.setApplicationName("Weather Bee");
    weatherApp.setOrganizationName("Dundee");
    weatherApp.setOrganizationDomain("dundeesoftware.net");
    weatherApp.setApplicationVersion("0.0.1");

#ifdef HIGH_DPI
    weatherApp.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif

    MainWindow mainWindow;
    mainWindow.setWindowTitle(weatherApp.applicationName());
    mainWindow.show();

    return weatherApp.exec();
}
