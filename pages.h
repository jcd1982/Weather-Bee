#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QSettings>
#include <QtWidgets>
#include <QScopedPointer>

#include "radartreewidget.h"

class ConfigurationPage : public QWidget
{
    Q_OBJECT

public:
    ConfigurationPage(QWidget *parent = 0);
    void saveSettings();
private:
    QSettings m_settings;
    QComboBox *m_temperatureCombo;
    QComboBox *m_distanceCombo;
    QComboBox *m_speedCombo;
    QComboBox *m_pressureCombo;
    QLineEdit *m_zipCodeLineEdit;
    QLineEdit *m_latLineEdit;
    QLineEdit *m_lonLineEdit;
    QComboBox *m_updateIntervalCombo;
    QComboBox *m_radarRefreshIntervalCombo;

    QLineEdit *zipCodeLineEdit;
    QLineEdit *latLineEdit;
    QLineEdit *lonLineEdit;
    RadarTreeWidget *m_radarTreeWidget;

    QComboBox *m_radarFrequencyCombo;
    QComboBox *m_radarProductCombo;
    QComboBox *m_radarBackgroundColorCombo;
};

class QueryPage : public QWidget
{
public:
    QueryPage(QWidget *parent = 0);
private:
    QSettings settings;
};

class UpdatePage : public QWidget
{
public:
    UpdatePage(QWidget *parent = 0);
private:
    QSettings settings;
};

#endif
