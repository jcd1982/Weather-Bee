#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "weatherstation.h"
#include "scnetworkaccessmanager.h"
#include "currentconditions.h"
#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    void createMenu();
    void initSettings();
    void connectSignals();

    QSignalMapper *signalMapper;
    QMenu *radarTypeMenu;

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *exitAction;

    RadarSettings m_radar_settings;
    CurrentConditions* c;

    SettingsDialog *settingsDialog;

    Ui::MainWindow *ui;

    WeatherStation* m_radar_station;

    std::map<QString, QList<QString>> m_map_station_list_by_state;

    QTimer* m_animation_timer;
    uint m_animation_interval;
    bool m_is_animated;

private slots:

    void slotDrawRadar();
    void slotRadarChanged(RadarType rt);
    void slotWeatherStationSelected(QString stationId);
    void slotAnimateRadar();
    void slotSetRadarOpacity(int opacity);
    void slotDrawBaseImage();
    void slotRadarMenu(QPoint pos);
    void slotSelectRadarType();
    void slotTabChanged(int i);

    void slotOverlayChanged(int overlay);

};

#endif // MAINWINDOW_H
