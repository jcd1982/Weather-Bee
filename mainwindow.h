#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
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

    QSignalMapper *signalMapper;
    QMenu *radarTypeMenu;

    void createMenu();
    void initSettings();
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *exitAction;

    settings m_overlay_settings;
    CurrentConditions* c;

    SettingsDialog *settingsDialog;

    void connectSignals();

    Ui::MainWindow *ui;

    WeatherStation* m_radar_station;

    QButtonGroup* m_radio_group_radar_type;
    QButtonGroup* m_overlays_group;

    std::map<QString, QList<QString>> m_map_station_list_by_state;

    QTimer* m_animation_timer;
    uint m_animation_interval;
    bool m_is_animated;

private slots:

    void slotOverlayChecked();
    void slotDrawRadar();
    void slotRadarChanged(RadarType rt);
    void slotWeatherStationSelected(QString stationId);
    void slotAnimateRadar();
    void slotSetRadarOpacity(int opacity);
    void slotDrawBaseImage();
    void slotRadarMenu(QPoint pos);
    void slotSelectRadarType();

    void slotTabChanged(int i);

};

#endif // MAINWINDOW_H
