#include <QtWidgets>

#include "pages.h"

ConfigurationPage::ConfigurationPage(QWidget *parent)
    : QWidget(parent)
{
    /////////////////////////////////////////////////
    /// \brief weatherSettingsGroups
    /// Weather Settings Tab
    /////////////////////////////////////////////////

    QGroupBox *weatherSettingsGroups = new QGroupBox;

        QVBoxLayout *wholeLayout = new QVBoxLayout;

            QGroupBox *unitsGroup = new QGroupBox(tr(" Units:"));

                QVBoxLayout *unitsLayout = new QVBoxLayout;

                    QHBoxLayout *temperatureLayout = new QHBoxLayout;

                        QLabel *temperatureLabel = new QLabel(tr("Temperature:"));
                        m_temperatureCombo = new QComboBox(this);
                        m_temperatureCombo->addItem(tr("Farenheit"));
                        m_temperatureCombo->addItem(tr("Celcius"));

                        // Set the ui to display the saved settings...
                        m_temperatureCombo->setCurrentIndex(
                                    m_temperatureCombo->findText(
                                        m_settings.value("temperatureUnits").toString() ));

                    temperatureLayout->addWidget(temperatureLabel);
                    temperatureLayout->addWidget(m_temperatureCombo);


                    QHBoxLayout *distanceLayout = new QHBoxLayout;

                        QLabel *distanceLabel = new QLabel(tr("Distance:"));
                        m_distanceCombo = new QComboBox(this);
                        m_distanceCombo->addItem(tr("Miles"));
                        m_distanceCombo->addItem(tr("Kilometers"));

                        m_distanceCombo->setCurrentIndex(
                                    m_distanceCombo->findText(
                                        m_settings.value("distanceUnits").toString() ));

                    distanceLayout->addWidget(distanceLabel);
                    distanceLayout->addWidget(m_distanceCombo);

                    QHBoxLayout *speedLayout = new QHBoxLayout;

                        QLabel *speedLabel = new QLabel(tr("Speed:"));
                        m_speedCombo = new QComboBox(this);
                        m_speedCombo->addItem(tr("mph"));
                        m_speedCombo->setItemData(0, tr("miles per hour"), Qt::ToolTipRole);
                        m_speedCombo->addItem(tr("kph"));
                        m_speedCombo->setItemData(1, tr("kilometers per hour"), Qt::ToolTipRole);
                        m_speedCombo->addItem(tr("knots"));
                        m_speedCombo->setItemData(2, tr("nautical miles per hour"), Qt::ToolTipRole);
                        m_speedCombo->addItem(tr("m/s"));
                        m_speedCombo->setItemData(3, tr("meters per second"), Qt::ToolTipRole);
                        m_speedCombo->addItem(tr("ft/s"));
                        m_speedCombo->setItemData(4, tr("feet per second"), Qt::ToolTipRole);

                        m_speedCombo->setCurrentIndex(
                                    m_speedCombo->findText(
                                        m_settings.value("speedUnits").toString() ));

                    speedLayout->addWidget(speedLabel);
                    speedLayout->addWidget(m_speedCombo);

                    QHBoxLayout *pressureLayout = new QHBoxLayout;

                        QLabel *pressureLabel = new QLabel(tr("Pressure:"));
                        m_pressureCombo = new QComboBox(this);
                        m_pressureCombo->addItem(tr("inHg"));
                        m_pressureCombo->setItemData(0, tr("inches of mercury"), Qt::ToolTipRole);
                        m_pressureCombo->addItem(tr("atm"));
                        m_pressureCombo->setItemData(1, tr("standard atmospheres"), Qt::ToolTipRole);
                        m_pressureCombo->addItem(tr("mb"));
                        m_pressureCombo->setItemData(2, tr("millibars"), Qt::ToolTipRole);

                        m_pressureCombo->setCurrentIndex(
                                    m_pressureCombo->findText(
                                        m_settings.value("pressureUnits").toString() ));

                    pressureLayout->addWidget(pressureLabel);
                    pressureLayout->addWidget(m_pressureCombo);

                unitsLayout->addLayout(temperatureLayout);
                unitsLayout->addLayout(distanceLayout);
                unitsLayout->addLayout(speedLayout);
                unitsLayout->addLayout(pressureLayout);

            unitsGroup->setLayout(unitsLayout);
            // TODO: line up the UI boxes in the location group
            QGroupBox *locationGroup = new QGroupBox(tr(" Location:"));

                QVBoxLayout *locationLayout = new QVBoxLayout;

                    QHBoxLayout *zipCodeLayout = new QHBoxLayout;

                        // TODO: Add input filter to restrict zip code
                        QLabel *zipCodeLabel = new QLabel(tr("Zip Code:"));
                        m_zipCodeLineEdit = new QLineEdit(this);

                        m_zipCodeLineEdit->setText(m_settings.value("zipCode").toString());

                    zipCodeLayout->addWidget(zipCodeLabel);
                    zipCodeLayout->addWidget(m_zipCodeLineEdit);
                    zipCodeLayout->setSpacing(2);

                    QHBoxLayout *latLonLayout = new QHBoxLayout;

                        QLabel *latLonLabel = new QLabel(tr("Lat/Lon:"));
                        m_latLineEdit = new QLineEdit(this);
                        m_latLineEdit->setDisabled(true);
                        m_lonLineEdit = new QLineEdit(this);
                        m_lonLineEdit->setDisabled(true);

                        m_latLineEdit->setText(m_settings.value("latitude").toString());
                        m_lonLineEdit->setText(m_settings.value("longitude").toString());

                    latLonLayout->addWidget(latLonLabel);
                    latLonLayout->addWidget(m_latLineEdit);
                    latLonLayout->addWidget(m_lonLineEdit);
                    latLonLayout->setSpacing(2);

                locationLayout->addItem(zipCodeLayout);
                locationLayout->addItem(latLonLayout);

            locationGroup->setLayout(locationLayout);

            QGroupBox *connectionGroup = new QGroupBox(tr(" Connection:"));

                QHBoxLayout *updateIntervalLayout = new QHBoxLayout;

                    QLabel *updateIntervalLabel = new QLabel(tr("Update Interval:"));
                    updateIntervalLabel->setToolTip(tr("Period to wait before requesting new weather Data"));
                    m_updateIntervalCombo = new QComboBox(this);
                    m_updateIntervalCombo->addItem(tr("5 Minutes"));
                    m_updateIntervalCombo->addItem(tr("15 Minutes"));
                    m_updateIntervalCombo->addItem(tr("30 Minutes"));
                    m_updateIntervalCombo->addItem(tr("45 Minutes"));
                    m_updateIntervalCombo->addItem(tr("1 Hour"));

                    m_updateIntervalCombo->setCurrentIndex(
                                m_updateIntervalCombo->findText(
                                    m_settings.value("fcastUpdateInterval").toString() ));

                updateIntervalLayout->addWidget(updateIntervalLabel);
                updateIntervalLayout->addWidget(m_updateIntervalCombo);

            connectionGroup->setLayout(updateIntervalLayout);

        wholeLayout->addWidget(unitsGroup);
        wholeLayout->addWidget(locationGroup);
        wholeLayout->addWidget(connectionGroup);

    weatherSettingsGroups->setLayout(wholeLayout);

    /////////////////////////////////////////////////
    ///
    /// Radar Settings Tab
    ///
    /////////////////////////////////////////////////

    QGroupBox *radarSettingsGroup = new QGroupBox;

        QVBoxLayout *radarSettingsLayout = new QVBoxLayout;

            QGroupBox *radarElementsGroup = new QGroupBox(tr(" Radar Elements:"));
            
                QVBoxLayout *radarElementsLayout = new QVBoxLayout;

                    QHBoxLayout *refreshRateLayout = new QHBoxLayout;

                        QLabel *refreshRateLabel = new QLabel(tr("Animation Speed:"));

                        m_radarFrequencyCombo = new QComboBox(this);
                        m_radarFrequencyCombo->addItem(tr("1 Hz"));
                        m_radarFrequencyCombo->addItem(tr("2 Hz"));
                        m_radarFrequencyCombo->addItem(tr("5 Hz"));
                        m_radarFrequencyCombo->addItem(tr("10 Hz"));
                        m_radarFrequencyCombo->addItem(tr("25 Hz"));
                        m_radarFrequencyCombo->addItem(tr("100 Hz"));

                        m_radarFrequencyCombo->setCurrentIndex(
                                    m_radarFrequencyCombo->findText(
                                        m_settings.value("radarFrequency").toString() ));

                    refreshRateLayout->addWidget(refreshRateLabel);
                    refreshRateLayout->addWidget(m_radarFrequencyCombo);

                    QHBoxLayout *radarProductLayout = new QHBoxLayout;

                        QLabel *radarProductLabel = new QLabel(tr("Default Radar:"));
                        m_radarProductCombo = new QComboBox(this);
                        m_radarProductCombo->addItem(tr("Base Refelectivity (N0R)"));
                        m_radarProductCombo->addItem(tr("Long Range (N0Z)"));
                        m_radarProductCombo->addItem(tr("Storm Motion (N0S)"));
                        m_radarProductCombo->addItem(tr("Base Velocity (N0V)"));
                        m_radarProductCombo->addItem(tr("One Hour Precipitation (N1P)"));
                        m_radarProductCombo->addItem(tr("Composite Refelectivity (NCR)"));
                        m_radarProductCombo->addItem(tr("Storm Precipitation (NTP)"));

                        m_radarProductCombo->setCurrentIndex(
                                    m_radarProductCombo->findText(
                                        m_settings.value("radarProduct").toString() ));

                    radarProductLayout->addWidget(radarProductLabel);
                    radarProductLayout->addWidget(m_radarProductCombo);

                    QHBoxLayout *radarBackgroundColorLayout = new QHBoxLayout;

                        QLabel *radarBackgroundColorLabel = new QLabel(tr("Radar Background Color:"));
                        m_radarBackgroundColorCombo = new QComboBox(this);
                        m_radarBackgroundColorCombo->addItem(tr("White"));
                        m_radarBackgroundColorCombo->addItem(tr("Black"));

                        m_radarBackgroundColorCombo->setCurrentIndex(
                                    m_radarBackgroundColorCombo->findText(
                                        m_settings.value("radarBackgroundColor").toString() ));

                    radarBackgroundColorLayout->addWidget(radarBackgroundColorLabel);
                    radarBackgroundColorLayout->addWidget(m_radarBackgroundColorCombo);

                    ///////////////////////////
                    QHBoxLayout *radarRefreshIntervalLayout = new QHBoxLayout;

                        QLabel *radarRefreshIntervalLabel = new QLabel(tr("Radar Update Interval:"));
                        radarRefreshIntervalLabel->setToolTip(tr("Period to wait before requesting new radar Data"));
                        m_radarRefreshIntervalCombo = new QComboBox(this);
                        m_radarRefreshIntervalCombo->addItem(tr("5 Minutes"));
                        m_radarRefreshIntervalCombo->addItem(tr("15 Minutes"));
                        m_radarRefreshIntervalCombo->addItem(tr("30 Minutes"));
                        m_radarRefreshIntervalCombo->addItem(tr("45 Minutes"));
                        m_radarRefreshIntervalCombo->addItem(tr("60 Minutes"));

                        m_radarRefreshIntervalCombo->setCurrentIndex(
                                    m_radarRefreshIntervalCombo->findText(
                                        m_settings.value("radarRefreshInterval").toString() ));

                    radarRefreshIntervalLayout->addWidget(radarRefreshIntervalLabel);
                    radarRefreshIntervalLayout->addWidget(m_radarRefreshIntervalCombo);

                    ////////////////////////////
                radarElementsLayout->addItem(refreshRateLayout);
                radarElementsLayout->addItem(radarProductLayout);
                radarElementsLayout->addItem(radarBackgroundColorLayout);
                radarElementsLayout->addItem(radarRefreshIntervalLayout);

            radarElementsGroup->setLayout(radarElementsLayout);

            QGroupBox *radarStationGroup = new QGroupBox(tr(" Radar Station:"));

                QVBoxLayout *radarStationLayout = new QVBoxLayout;

                    m_radarTreeWidget = new RadarTreeWidget(this);

                radarStationLayout->addWidget(m_radarTreeWidget);

            radarStationGroup->setLayout(radarStationLayout);

       radarSettingsLayout->addWidget(radarElementsGroup);
       radarSettingsLayout->addWidget(radarStationGroup);

    radarSettingsGroup->setLayout(radarSettingsLayout);




    /////////////////////////////////////////////////

    QTabWidget *settingsTabWidget = new QTabWidget;

    settingsTabWidget->addTab(weatherSettingsGroups, tr("Weather Settings"));
    settingsTabWidget->addTab(radarSettingsGroup, tr("Radar Settings"));



    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(settingsTabWidget);
    //mainLayout->addWidget(locationGroup);
    //mainLayout->addStretch(1);
    //mainLayout->setContentsMargins(10,10,10,10);
    setLayout(mainLayout);

}
void ConfigurationPage::saveSettings(){

    qDebug() << "ConfigurationPage::saveSettings";

    m_settings.setValue("temperatureUnits", this->m_temperatureCombo->currentText());   // [Farenheit|Celcius]
    m_settings.setValue("distanceUnits", this->m_distanceCombo->currentText());         // [Miles|Kilometers]
    m_settings.setValue("speedUnits", this->m_speedCombo->currentText());               // [mph|kph|knots|m/s|ft/s]
    m_settings.setValue("pressureUnits", this->m_pressureCombo->currentText());         // [inHg|atm|mb]

    m_settings.setValue("zipCode", this->m_zipCodeLineEdit->text());
    m_settings.setValue("latitude", this->m_latLineEdit->text());
    m_settings.setValue("longitude", this->m_lonLineEdit->text());

    m_settings.setValue("fcastUpdateInterval", this->m_updateIntervalCombo->currentText());

    m_settings.setValue("radarFrequency", this->m_radarFrequencyCombo->currentText());
    m_settings.setValue("radarProduct", this->m_radarProductCombo->currentText());
    m_settings.setValue("radarBackgroundColor", this->m_radarBackgroundColorCombo->currentText());
    m_settings.setValue("radarRefreshInterval", this->m_radarRefreshIntervalCombo->currentText());

    if(m_radarTreeWidget->currentItem() != nullptr &&
       m_radarTreeWidget->currentItem()->parent() != nullptr){

        QString radarStation = m_radarTreeWidget->currentItem()->text(m_radarTreeWidget->currentColumn());

        QString radarStationID = radarStation.split(" - ").at(0).toUpper();
        //qDebug() << "radarStation" << radarStation;
        //qDebug() << "radarStationID" << radarStationID;
        m_settings.setValue("radarStation", radarStation);
        m_settings.setValue("radarStationID", radarStationID);
    }

    return;
}

/*

UpdatePage::UpdatePage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *updateGroup = new QGroupBox(tr("Package selection"));
    QCheckBox *systemCheckBox = new QCheckBox(tr("Update system"));
    QCheckBox *appsCheckBox = new QCheckBox(tr("Update applications"));
    QCheckBox *docsCheckBox = new QCheckBox(tr("Update documentation"));

    QGroupBox *packageGroup = new QGroupBox(tr("Existing packages"));

    QListWidget *packageList = new QListWidget;
    QListWidgetItem *qtItem = new QListWidgetItem(packageList);
    qtItem->setText(tr("Qt"));
    QListWidgetItem *qsaItem = new QListWidgetItem(packageList);
    qsaItem->setText(tr("QSA"));
    QListWidgetItem *teamBuilderItem = new QListWidgetItem(packageList);
    teamBuilderItem->setText(tr("Teambuilder"));

    QPushButton *startUpdateButton = new QPushButton(tr("Start update"));

    QVBoxLayout *updateLayout = new QVBoxLayout;
    updateLayout->addWidget(systemCheckBox);
    updateLayout->addWidget(appsCheckBox);
    updateLayout->addWidget(docsCheckBox);
    updateGroup->setLayout(updateLayout);

    QVBoxLayout *packageLayout = new QVBoxLayout;
    packageLayout->addWidget(packageList);
    packageGroup->setLayout(packageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(updateGroup);
    mainLayout->addWidget(packageGroup);
    //mainLayout->addSpacing(12);
    mainLayout->addWidget(startUpdateButton);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

QueryPage::QueryPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *packagesGroup = new QGroupBox(tr("Look for packages"));

    QLabel *nameLabel = new QLabel(tr("Name:"));
    QLineEdit *nameEdit = new QLineEdit;

    QLabel *dateLabel = new QLabel(tr("Released after:"));
    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());

    QCheckBox *releasesCheckBox = new QCheckBox(tr("Releases"));
    QCheckBox *upgradesCheckBox = new QCheckBox(tr("Upgrades"));

    QSpinBox *hitsSpinBox = new QSpinBox;
    hitsSpinBox->setPrefix(tr("Return up to "));
    hitsSpinBox->setSuffix(tr(" results"));
    hitsSpinBox->setSpecialValueText(tr("Return only the first result"));
    hitsSpinBox->setMinimum(1);
    hitsSpinBox->setMaximum(100);
    hitsSpinBox->setSingleStep(10);

    QPushButton *startQueryButton = new QPushButton(tr("Start query"));

    QGridLayout *packagesLayout = new QGridLayout;
    packagesLayout->addWidget(nameLabel, 0, 0);
    packagesLayout->addWidget(nameEdit, 0, 1);
    packagesLayout->addWidget(dateLabel, 1, 0);
    packagesLayout->addWidget(dateEdit, 1, 1);
    packagesLayout->addWidget(releasesCheckBox, 2, 0);
    packagesLayout->addWidget(upgradesCheckBox, 3, 0);
    packagesLayout->addWidget(hitsSpinBox, 4, 0, 1, 2);
    packagesGroup->setLayout(packagesLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(packagesGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(startQueryButton);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

*/
