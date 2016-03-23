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
    QSettings settings;
    //settings.clear();

    // QSettings default location/filename is platform specific.
    // on Linux it is: '~/.config/<application name>/<organization name>.conf
    if(settings.value("init").toString() != "true"){
        InitSettingsDialog *d = new InitSettingsDialog(this);
        int p = d->exec();
        qDebug() << "Init Exit" << p;
        this->initSettings();
    }

    m_overlay_settings.cityEnabled = settings.value("cityEnabled").toBool();
    m_overlay_settings.countiesEnabled = settings.value("countiesEnabled").toBool();
    m_overlay_settings.highwaysEnabled = settings.value("highwaysEnabled").toBool();
    m_overlay_settings.legendEnabled = settings.value("legendEnabled").toBool();
    m_overlay_settings.rangeEnabled = settings.value("rangeEnabled").toBool();
    m_overlay_settings.riversEnabled = settings.value("riversEnabled").toBool();
    m_overlay_settings.warningsEnabled = settings.value("warningsEnabled").toBool();
    m_overlay_settings.radarOpacity = settings.value("radarOpacity").toReal();

    settingsDialog = new SettingsDialog(this);

    ui->setupUi(this);

    m_radar_station = new WeatherStation(settings.value("radarStationID").toString(), m_overlay_settings, this);

    m_overlays_group = new QButtonGroup(this);
    QList<QCheckBox *> overlays = ui->groupCheckBox->findChildren<QCheckBox *>();
    for(int i = 0; i < overlays.size(); ++i)
    {
        m_overlays_group->addButton(overlays[i],i);
    }
    m_overlays_group->setExclusive(false);

    m_animation_timer = new QTimer(this);
    int freq = settings.value("radarFrequency").toString().split(QChar(' ')).at(0).toInt();
    (freq > 0)?m_animation_interval = 1000 / freq:m_animation_interval = 500;

    connectSignals();

    QString product(settings.value("radarProduct").toString());
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

    ui->checkTopo->click();
    ui->checkCity->click();
    ui->checkCounties->click();
    ui->checkHighways->click();
    ui->checkLegend->click();
    ui->checkRangeRing->click();
    ui->checkRivers->click();
    ui->checkWarnings->click();

    ui->radarDisplay->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->radarDisplay->setBaseSize(600, 550);
    ui->radarDisplay->show();

    qDebug() << "Zip Code:" << settings.value("zipCode").toString();
    c = new CurrentConditions(settings.value("zipCode").toString(), this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(c);
    ui->tab->setLayout(horizontalLayout);


    // REST weather forecast instance: http://graphical.weather.gov/xml/sample_products/browser_interface/ndfdXMLclient.php?zipCodeList=07470&product=time-series&begin=&end=&maxt=maxt&mint=mint
    // REST schema contained in weather forecasts: http://www.nws.noaa.gov/forecasts/xml/DWMLgen/schema/DWML.xsd
    // REST schema redirect (correct): http://graphical.weather.gov/xml/DWMLgen/schema/DWML.xsd
/*
    request.setUrl(QUrl("http://graphical.weather.gov/xml/DWMLgen/schema/DWML.xsd"));
    request.setAttribute(QNetworkRequest::User, QVariant("ForecastWeatherSchema"));
    this->nam->get(request);
*/

}

void MainWindow::slotOverlayChecked(){

    qDebug() << "MainWindow::slotOverlayChecked(int buttonId)";

    m_overlay_settings.topoEnabled = ui->checkTopo->isChecked();
    m_overlay_settings.countiesEnabled = ui->checkCounties->isChecked();
    m_overlay_settings.cityEnabled = ui->checkCity->isChecked();
    m_overlay_settings.highwaysEnabled = ui->checkHighways->isChecked();
    m_overlay_settings.riversEnabled = ui->checkRivers->isChecked();
    m_overlay_settings.rangeEnabled = ui->checkRangeRing->isChecked();
    m_overlay_settings.legendEnabled = ui->checkLegend->isChecked();
    m_overlay_settings.warningsEnabled = ui->checkWarnings->isChecked();

    m_radar_station->setOverlaySettings(m_overlay_settings);

    if( !ui->checkBoxAnimate->isChecked()){
        this->slotDrawRadar();
    }

    return;
}

void MainWindow::connectSignals(){

    connect(m_radar_station, SIGNAL(signalRadarProductDownloadDone(RadarType)), this, SLOT(slotDrawRadar()));
    connect(m_radar_station, SIGNAL(signalBaseOverlaysDownloadDone()), this, SLOT(slotDrawBaseImage()));
    connect(m_animation_timer, SIGNAL(timeout()), this, SLOT(slotDrawRadar()));
    connect(ui->checkBoxAnimate, SIGNAL(clicked(bool)), this, SLOT(slotAnimateRadar()));
    connect(m_overlays_group, SIGNAL(buttonClicked(int)), this, SLOT(slotOverlayChecked()));
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

    QMenu radarStationMenu(tr("Radar Station"), this);

    QAction action8("Select New Radar", this);

    RadarTreeWidget radarTreeWidget(this);

    connect(&radarTreeWidget, SIGNAL(signalDoubleClick(QString)), this, SLOT(slotWeatherStationSelected(QString)));

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(&radarTreeWidget);
    radarStationMenu.addAction(widgetAction);

    radarImageContextMenu.addMenu(&radarStationMenu);

    radarImageContextMenu.exec(ui->radarDisplay->mapToGlobal(pos));

    return;
}

void MainWindow::slotSelectRadarType(){

    qDebug() << "MainWindow::slotSelectRadarType";

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

    m_radar_station = new WeatherStation(stationId, m_overlay_settings, this);
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

    if(rt == RadarType::N0Z && ui->checkRangeRing->isEnabled()){
        ui->checkRangeRing->setDisabled(true);
    }
    if(rt != RadarType::N0Z && !ui->checkRangeRing->isEnabled()){
        ui->checkRangeRing->setDisabled(false);
    }

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
    this->m_overlay_settings.radarOpacity = static_cast<qreal>(opacity) / 100;
    m_radar_station->setOverlaySettings(this->m_overlay_settings);
    qDebug() << "Slider" << opacity << "radar_opacity" << this->m_overlay_settings.radarOpacity;
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
    settings.setValue("fcastUpdateInterval", "1 Hour");     // [15 Minutes|30 Minutes|45 Minutes|1 Hour]

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

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}
