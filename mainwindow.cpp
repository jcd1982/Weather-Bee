#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "initsettingsdialog.h"

#include <QDomDocument>
#include <QtWidgets>

// TODO: bad radar selection: "evx - Eglin AFB"

// TODO: add NOAA Sattelite images http://www.ssd.noaa.gov/PS/PCPN/DATA/RT/....

// TODO: look for dead connections that were not disconnected.

// TODO: init overlay_options

// TODO: add conus mosaic http://radar.weather.gov/ridge/Conus/Loop/northrockies_loop.gif

// TODO: test the signals with the signal counter

// TODO: full review of settings and how things are stored.

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings savedSettings;
    //settings.clear();

    // QSettings default location/filename is platform specific.
    // on Linux it is: '~/.config/<application name>/<organization name>.conf
    if(savedSettings.value("init").toString() != "true"){
        InitSettingsDialog *d = new InitSettingsDialog(this);
        int p = d->exec();
        qDebug() << "Init Exit" << p;
        this->initSettings();
    }

    settingsDialog = new SettingsDialog(this);

    ui->setupUi(this);

    m_radar_settings.topoEnabled = savedSettings.value("topoEnabled").toBool();
    m_radar_settings.cityEnabled = savedSettings.value("cityEnabled").toBool();
    m_radar_settings.countiesEnabled = savedSettings.value("countiesEnabled").toBool();
    m_radar_settings.highwaysEnabled = savedSettings.value("highwaysEnabled").toBool();
    m_radar_settings.legendEnabled = savedSettings.value("legendEnabled").toBool();
    m_radar_settings.rangeEnabled = savedSettings.value("rangeEnabled").toBool();
    m_radar_settings.riversEnabled = savedSettings.value("riversEnabled").toBool();
    m_radar_settings.warningsEnabled = savedSettings.value("warningsEnabled").toBool();
    m_radar_settings.radarOpacity = savedSettings.value("radarOpacity").toReal();
    m_radar_settings.radarRefreshInterval = savedSettings.value("radarRefreshInterval").toString().split(QChar(' ')).at(0).toInt();

    ui->sliderOpacity->setValue(static_cast<int>(m_radar_settings.radarOpacity * 100));

    m_radar_station = new WeatherStation(savedSettings.value("radarStationID").toString(), m_radar_settings, this);

    m_animation_timer = new QTimer(this);
    int freq = savedSettings.value("radarFrequency").toString().split(QChar(' ')).at(0).toInt();
    (freq > 0)?m_animation_interval = 1000 / freq:m_animation_interval = 500;

    connectSignals();

    QString product(savedSettings.value("radarProduct").toString());
    if(product == "Base Refelectivity (N0R)"){
        this->slotRadarChanged(RadarType::N0R);
    }else if(product == "Long Range (N0Z)"){
        this->slotRadarChanged(RadarType::N0Z);
    }else if(product == "Storm Motion (N0S)"){
        this->slotRadarChanged(RadarType::N0S);
    }else if(product == "Base Velocity (N0V)"){
        this->slotRadarChanged(RadarType::N0V);
    }else if(product == "One Hour Precipitation (N1P)"){
        this->slotRadarChanged(RadarType::N1P);
    }else if(product == "Composite Refelectivity (NCR)"){
        this->slotRadarChanged(RadarType::NCR);
    }else if(product == "Storm Precipitation (NTP)"){
        this->slotRadarChanged(RadarType::NTP);
    }else{
        qDebug("Error, final case for radarProduct settings");
        exit(-1);
    }

    ui->radarDisplay->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->radarDisplay->setBaseSize(600, 550);
    ui->radarDisplay->show();
    ui->radarDisplay->setToolTip("Right-Click for more options");
    ui->radarDisplay->setToolTipDuration(1500);

    this->resize(600,700);

    c = new CurrentConditions(savedSettings.value("zipCode").toString(), this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(c);
    ui->tab->setLayout(horizontalLayout);

}

void MainWindow::connectSignals(){

    connect(m_radar_station, SIGNAL(signalRadarProductDownloadDone(RadarType)), this, SLOT(slotDrawRadar()));
    connect(m_radar_station, SIGNAL(signalBaseOverlaysDownloadDone()), this, SLOT(slotDrawBaseImage()));
    connect(m_animation_timer, SIGNAL(timeout()), this, SLOT(slotDrawRadar()));
    connect(ui->checkBoxAnimate, SIGNAL(clicked(bool)), this, SLOT(slotAnimateRadar()));
    connect(ui->sliderOpacity, SIGNAL(valueChanged(int)), this, SLOT(slotSetRadarOpacity(int)));
    connect(ui->pushButtonRefresh, SIGNAL(clicked(bool)), m_radar_station, SLOT(slotRefreshRadarProduct()));
    connect(ui->actionEdit_Settings , SIGNAL(triggered(bool)), settingsDialog, SLOT(exec()));
    connect(ui->radarDisplay, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotRadarMenu(QPoint)));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));

    return;
}
void MainWindow::slotTabChanged(int i){

    if(i == 0 && m_is_animated){
        m_animation_timer->stop();
    }else if(i == 1 && m_is_animated){
        m_animation_timer->start(m_animation_interval);
    }

    return;
}


void MainWindow::slotRadarMenu(QPoint pos){

    qDebug() << "MainWindow::slotRadarMenu(QPoint pos)";

    //////////////////////////////////////////////
    ///
    ///  radarTypeMenu
    ///
    //////////////////////////////////////////////

    QMenu radarTypeMenu(tr("Radar Type"), this);

    QAction action1("Base", this);
    QAction action2("Long Range", this);
    QAction action3("Storm Motion", this);
    QAction action4("Velocity", this);
    QAction action5("One-Hour Precipitation", this);
    QAction action6("Composite", this);
    QAction action7("Storm Precipitation", this);

    radarTypeMenu.addAction(&action1);
    radarTypeMenu.addAction(&action2);
    radarTypeMenu.addAction(&action3);
    radarTypeMenu.addAction(&action4);
    radarTypeMenu.addAction(&action5);
    radarTypeMenu.addAction(&action6);
    radarTypeMenu.addAction(&action7);

    //radarTypeMenu.setStyleSheet("QMenu::item:selected{border:1px solid red;}");

    connect(&action1, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::N0R); });
    connect(&action2, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::N0Z); });
    connect(&action3, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::N0S); });
    connect(&action4, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::N0V); });
    connect(&action5, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::N1P); });
    connect(&action6, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::NCR); });
    connect(&action7, &QAction::triggered, this, [this]{ slotRadarChanged(RadarType::NTP); });

    QMenu radarImageContextMenu(tr("Context Menu"), this);

    radarImageContextMenu.addMenu(&radarTypeMenu);

    //////////////////////////////////////////////
    ///
    ///  radarStationMenu
    ///
    //////////////////////////////////////////////

    QMenu radarStationMenu(tr("Radar Station"), this);

    RadarTreeWidget radarTreeWidget(this);

    connect(&radarTreeWidget, SIGNAL(signalDoubleClick(QString)), this, SLOT(slotWeatherStationSelected(QString)));

    QWidgetAction *radarTreeAction = new QWidgetAction(this);

    radarTreeAction->setDefaultWidget(&radarTreeWidget);

    radarStationMenu.addAction(radarTreeAction);

    radarImageContextMenu.addMenu(&radarStationMenu);

    //////////////////////////////////////////////
    ///
    ///  radarOverlayMenu
    ///
    //////////////////////////////////////////////

    bool rangeRing = m_radar_station->getCurrentRadarProductType() != RadarType::N0Z;

    QMenu radarOverlayMenu(tr("Overlays"), this);

    QCheckBox *checkTopography = new QCheckBox("Topography", this);
    QCheckBox *checkCities = new QCheckBox("Cities", this);
    QCheckBox *checkCounties = new QCheckBox("Counties", this);
    QCheckBox *checkHighways = new QCheckBox("Highways", this);
    QCheckBox *checkRivers = new QCheckBox("Rivers", this);
    QCheckBox *checkRangeRing = new QCheckBox("Range Ring", this);
    QCheckBox *checkWarnings = new QCheckBox("Warnings", this);
    QCheckBox *checkLegend = new QCheckBox("Legend", this);

    checkTopography->setStyleSheet("padding: 6px;");
    checkCities->setStyleSheet("padding: 6px;");
    checkCounties->setStyleSheet("padding: 6px;");
    checkHighways->setStyleSheet("padding: 6px;");
    checkRivers->setStyleSheet("padding: 6px;");
    if (rangeRing) checkRangeRing->setStyleSheet("padding: 6px;");
    checkWarnings->setStyleSheet("padding: 6px;");
    checkLegend->setStyleSheet("padding: 6px;");

    checkTopography->setChecked(this->m_radar_settings.topoEnabled);
    checkCities->setChecked(this->m_radar_settings.cityEnabled);
    checkCounties->setChecked(this->m_radar_settings.countiesEnabled);
    checkHighways->setChecked(this->m_radar_settings.highwaysEnabled);
    checkRivers->setChecked(this->m_radar_settings.riversEnabled);
    if (rangeRing) checkRangeRing->setChecked(this->m_radar_settings.rangeEnabled);
    checkWarnings->setChecked(this->m_radar_settings.warningsEnabled);
    checkLegend->setChecked(this->m_radar_settings.legendEnabled);

    QWidgetAction *radarTopographyAction = new QWidgetAction(this);
    QWidgetAction *radarCitiesAction = new QWidgetAction(this);
    QWidgetAction *radarCountiesAction = new QWidgetAction(this);
    QWidgetAction *radarHighwaysAction = new QWidgetAction(this);
    QWidgetAction *radarRiversAction = new QWidgetAction(this);
    QWidgetAction *radarRangeRingAction = new QWidgetAction(this);
    QWidgetAction *radarWarningsAction = new QWidgetAction(this);
    QWidgetAction *radarLegendAction = new QWidgetAction(this);

    radarTopographyAction->setDefaultWidget(checkTopography);
    radarCitiesAction->setDefaultWidget(checkCities);
    radarCountiesAction->setDefaultWidget(checkCounties);
    radarHighwaysAction->setDefaultWidget(checkHighways);
    radarRiversAction->setDefaultWidget(checkRivers);
    if (rangeRing) radarRangeRingAction->setDefaultWidget(checkRangeRing);
    radarWarningsAction->setDefaultWidget(checkWarnings);
    radarLegendAction->setDefaultWidget(checkLegend);

    radarOverlayMenu.addAction(radarTopographyAction);
    radarOverlayMenu.addAction(radarCitiesAction);
    radarOverlayMenu.addAction(radarCountiesAction);
    radarOverlayMenu.addAction(radarHighwaysAction);
    radarOverlayMenu.addAction(radarRiversAction);
    if (rangeRing) radarOverlayMenu.addAction(radarRangeRingAction);
    radarOverlayMenu.addAction(radarWarningsAction);
    radarOverlayMenu.addAction(radarLegendAction);

    connect(checkTopography, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(1); } );
    connect(checkCities, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(2); } );
    connect(checkCounties, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(3); } );
    connect(checkHighways, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(4); } );
    connect(checkRivers, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(5); } );
    if (rangeRing) connect(checkRangeRing, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(6); } );
    connect(checkWarnings, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(7); } );
    connect(checkLegend, &QCheckBox::clicked, this, [this]{ slotOverlayChanged(8); } );

    radarImageContextMenu.addMenu(&radarOverlayMenu);

    ////////////////////////////////////////////////

    radarImageContextMenu.exec(ui->radarDisplay->mapToGlobal(pos));

    return;
}

void MainWindow::slotOverlayChanged(int overlay){

    qDebug() << "MainWindow::slotOverlayChanged()";

    qDebug() << "Action value" << overlay;

    QSettings s;

    switch(overlay){

        case 1:{
            qDebug() << "case 1";
            m_radar_settings.topoEnabled = !m_radar_settings.topoEnabled;
            if(m_radar_settings.topoEnabled){
                s.setValue("topoEnabled", "true");
            }else{
                s.setValue("topoEnabled", "false");
            }
            qDebug() << m_radar_settings.topoEnabled;
            break;
        }
        case 2:{
            qDebug() << "case 2";
            m_radar_settings.cityEnabled = !m_radar_settings.cityEnabled;
            if(m_radar_settings.cityEnabled){
                s.setValue("cityEnabled", "true");
            }else{
                s.setValue("cityEnabled", "false");
            }
            break;
        }
        case 3:{
            qDebug() << "case 3";
            m_radar_settings.countiesEnabled = !m_radar_settings.countiesEnabled;
            if(m_radar_settings.countiesEnabled){
                s.setValue("countiesEnabled", "true");
            }else{
                s.setValue("countiesEnabled", "false");
            }
            break;
        }
        case 4:{
            qDebug() << "case 4";
            m_radar_settings.highwaysEnabled  = !m_radar_settings.highwaysEnabled;
            if(m_radar_settings.highwaysEnabled){
                s.setValue("highwaysEnabled", "true");
            }else{
                s.setValue("highwaysEnabled", "false");
            }
            break;
        }
        case 5:{
            qDebug() << "case 5";
            m_radar_settings.riversEnabled = !m_radar_settings.riversEnabled;
            if(m_radar_settings.riversEnabled){
                s.setValue("riversEnabled", "true");
            }else{
                s.setValue("riversEnabled", "false");
            }
            break;
        }
        case 6:{
            qDebug() << "case 6";
            m_radar_settings.rangeEnabled = !m_radar_settings.rangeEnabled;
            if(m_radar_settings.rangeEnabled){
                s.setValue("rangeEnabled", "true");
            }else{
                s.setValue("rangeEnabled", "false");
            }
            break;
        }
        case 7:{
            qDebug() << "case 7";
            m_radar_settings.warningsEnabled = !m_radar_settings.warningsEnabled;
            if(m_radar_settings.warningsEnabled){
                s.setValue("warningsEnabled", "true");
            }else{
                s.setValue("warningsEnabled", "false");
            }
            break;
        }
        case 8:{
            qDebug() << "case 8";
            m_radar_settings.legendEnabled = !m_radar_settings.legendEnabled;
            if(m_radar_settings.legendEnabled){
                s.setValue("legendEnabled", "true");
            }else{
                s.setValue("legendEnabled", "false");
            }
            break;
        }
    }
    m_radar_station->setRadarSettings(m_radar_settings);

    slotDrawRadar();

    return;
}

void MainWindow::slotSelectRadarType(){

    qDebug() << "MainWindow::slotSelectRadarType()";

    return;
}

void MainWindow::slotAnimateRadar(){

    if(ui->checkBoxAnimate->isChecked()){
        m_animation_timer->start(m_animation_interval);
        m_is_animated = true;
    }else{
        m_animation_timer->stop();
        m_is_animated = false;
    }

    return;
}

void MainWindow::slotWeatherStationSelected(QString stationId){

    qDebug() << "MainWindow::slotWeatherStationSelected(QString stationId)";

    if(m_radar_station->getStationId() == stationId){
        return;
    }

    disconnect(m_radar_station, SIGNAL(signalRadarProductDownloadDone(RadarType)), this, SLOT(slotDrawRadar()));
    disconnect(ui->pushButtonRefresh, SIGNAL(clicked(bool)), m_radar_station, SLOT(slotRefreshRadarProduct()));

    // Save the currently selected radar type before destruction.
    RadarType rt = m_radar_station->getCurrentRadarProductType();

    m_radar_station->deleteLater();

    m_radar_station = new WeatherStation(stationId, m_radar_settings, this);
    connect(m_radar_station, SIGNAL(signalRadarProductDownloadDone(RadarType)), this, SLOT(slotDrawRadar()));
    connect(ui->pushButtonRefresh, SIGNAL(clicked(bool)), m_radar_station, SLOT(slotRefreshRadarProduct()));

    slotRadarChanged(rt);

    return;
}

void MainWindow::slotRadarChanged(RadarType rt){

    qDebug() << "MainWindow::slotRadarChanged(RadarType rt)" << rt;

    //RadarType rt = static_cast<RadarType>(buttonId);

    // The range ring for the long range product (N0Z) is either misleading, erroneous,
    // or misunderstood.  The site: 'http://www.srh.noaa.gov/jetstream/doppler/ridge_download.htm',
    // which details the radar data, states that the N0Z product provides data out to 248nmi, yet,
    // the range ring does not expand (as of 12/8/15) beyond the short
    // range radar range ring.
    //
    // Below two "if( rt. . ." condititionals disable and enable the radio button to accomidate the issue.

    // TODO: Make sure this functionality (^) is duplicated in context menu

    /*
    if(rt == RadarType::N0Z && ui->checkRangeRing->isEnabled()){
        ui->checkRangeRing->setDisabled(true);
    }
    if(rt != RadarType::N0Z && !ui->checkRangeRing->isEnabled()){
        ui->checkRangeRing->setDisabled(false);
    }
    */

    if(!m_radar_station->setRadarProduct(rt)){
        qDebug() << "Failure setting radar product."
                 << "RadarType:" << rt
                 << __FILE__ << __LINE__;
        exit(-1);
    }

    return;
}

void MainWindow::slotDrawRadar(){

    qDebug() << "MainWindow::slotDrawRadar";

    if( ui->checkBoxAnimate->isChecked() ){

        QPixmap* qp = m_radar_station->getNextImage();
        if( qp != nullptr ){
            //ui->radarDisplay->adjustSize();
            ui->radarDisplay->setPixmap(*qp);
            delete qp;
        }
    }else{
        QPixmap* qp = m_radar_station->getCurrentImage();
        if( qp != nullptr ){
            //ui->radarDisplay->adjustSize();
            ui->radarDisplay->setPixmap(*qp);
            delete qp;
        }
    }

    return;
}

void MainWindow::slotDrawBaseImage(){

    qDebug() << "MainWindow::slotDrawBaseImage()";

    QPixmap* qp = m_radar_station->getBaseImage();
    if( qp != nullptr ){
        ui->radarDisplay->setPixmap(*qp);
        delete qp;
    }

    return;
}

void MainWindow::slotSetRadarOpacity(int opacity){

    // TODO: add range check for opacity.
    this->m_radar_settings.radarOpacity = static_cast<qreal>(opacity) / 100;
    m_radar_station->setRadarSettings(this->m_radar_settings);
    qDebug() << "Slider" << opacity << "radar_opacity" << this->m_radar_settings.radarOpacity;
    if( !ui->checkBoxAnimate->isChecked() ){
        slotDrawRadar();
    }
}

void MainWindow::initSettings(){
    /// Initialize all settings to default.
    QSettings settings;

    settings.setValue("temperatureUnits","Farenheit");      // [Farenheit|Celcius]
    settings.setValue("distanceUnits","Miles");             // [Miles|Kilometers]
    settings.setValue("speedUnits","mph");                  // [mph|kph|knots|m/s|ft/s]
    settings.setValue("pressureUnits","inHg");              // [inHg|atm|mb]
    // TODO: make sure 1 hour and X minutes all work/
    settings.setValue("fcastUpdateInterval", "1 Hour");     // [15 Minutes|30 Minutes|45 Minutes|1 Hour]

    settings.setValue("topoEnabled", "true");
    settings.setValue("cityEnabled", "true");
    settings.setValue("countiesEnabled", "true");
    settings.setValue("highwaysEnabled", "true");
    settings.setValue("legendEnabled", "true");
    settings.setValue("rangeEnabled", "true");
    settings.setValue("riversEnabled", "true");
    settings.setValue("warningsEnabled", "true");
    settings.setValue("radarOpacity", "0.75");

    settings.setValue("radarFrequency", "10 Hz");
    settings.setValue("radarProduct", "Base Refelectivity (N0R)");
    settings.setValue("radarBackgroundColor", "White");

    settings.setValue("radarRefreshInterval", "1 Minutes");     // [15 Minutes|30 Minutes|45 Minutes|60 Minutes]
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}
