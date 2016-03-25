#include <QVariant>
#include <QtWidgets>
#include <QLabel>
#include <QSettings>

#include "currentconditions.h"
#include "weatherutil.h"

CurrentConditions::CurrentConditions(QString zip, QWidget *parent):
    QWidget(parent),
    m_zip(zip)
{
    init();

    reqCurrCond(this->m_zip);
}

CurrentConditions::CurrentConditions(QGeoCoordinate position, QWidget *parent):
    QWidget(parent),
    m_position(position)
{
    init();

    reqCurrCond(this->m_position);
}

void CurrentConditions::init(){

    ////

    this->m_currentDateDisplay = new QLabel;

    this->m_currentDewPointDisplay = new QLabel;
    this->m_currentHumidityDisplay = new QLabel;
    this->m_currentWindSpeedDisplay = new QLabel;
    this->m_currentWindDirectionDisplay = new QLabel;
    this->m_currentVisibilityDisplay = new QLabel;
    this->m_currentWindChillDisplay = new QLabel;

    ////

    this->m_nam = new SCNetworkAccessManager(this);

    connect(this->m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotHandleNetworkReplies(QNetworkReply*)));
    connect(this, SIGNAL(signalWeatherIconDownloadDone()), this, SLOT(slotDrawWeatherIcon()));

    //QSpacerItem *leftSpacer = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->m_currentConditionsIconDisplay = new QLabel;
    this->m_currentTemperatureDisplay = new QLabel;
    this->m_currentTemperatureScaleDisplay = new QLabel;
    QSpacerItem *rightSpacer = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *weatherIconTempScaleLt = new QHBoxLayout;
    //weatherIconTempScaleLt->addSpacerItem(leftSpacer);
    weatherIconTempScaleLt->addWidget(this->m_currentConditionsIconDisplay);
    weatherIconTempScaleLt->addWidget(this->m_currentTemperatureDisplay);
    weatherIconTempScaleLt->addWidget(this->m_currentTemperatureScaleDisplay);
    weatherIconTempScaleLt->addSpacerItem(rightSpacer);

    this->m_currentLocationDisplay = new QLabel;
    this->m_currentShortWeatherDiscriptionDisplay = new QLabel;

    QVBoxLayout *primaryCurrentWeatherLt = new QVBoxLayout;
    primaryCurrentWeatherLt->addWidget(this->m_currentLocationDisplay);
    primaryCurrentWeatherLt->addLayout(weatherIconTempScaleLt);
    primaryCurrentWeatherLt->addWidget(this->m_currentShortWeatherDiscriptionDisplay);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    //mainLayout->addWidget(settingsTabWidget);
    //mainLayout->addWidget(locationGroup);

    mainLayout->addLayout(primaryCurrentWeatherLt);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

}

void CurrentConditions::slotDrawWeatherIcon(){
    qDebug() << "slotDrawWeatherIcon";
    this->m_currentConditionsIconDisplay->setPixmap(*m_weatherIcon);
    return;
}

void CurrentConditions::reqCurrCond(const QString& zip){
    /// Requests the lat/lon for a given zip, and forwards reply data to the overload.
    ///
    /// The reply to the get call is handled in CurrentConditions::slotHandleNetworkReplies
    ///  which in turn calls reqCurrCond(const QGeoCoordinate& position) using the
    ///  latitude and longitude data in the reply

    // Request latitude and longitude data for a given zip code
    QNetworkRequest request(QUrl("http://graphical.weather.gov/xml/sample_products/browser_interface/ndfdXMLclient.php?listZipCodeList=" + zip));
    request.setAttribute(QNetworkRequest::User, QVariant(ReqType::NOAA_LAT_LON_FROM_ZIP));

    this->m_nam->get(request);

    return;
}

void CurrentConditions::reqCurrCond(const QGeoCoordinate& position){
    /// Requests the current conditions data.

    if( position.isValid() ){

        QNetworkRequest request(QUrl(QString::fromStdString(std::string("http://forecast.weather.gov/MapClick.php?lat="
                                             + std::to_string(position.latitude())
                                             + "&lon="
                                             + std::to_string(position.longitude())
                                             + "&FcstType=json"))));
        request.setAttribute(QNetworkRequest::User, QVariant(ReqType::NOAA_JSON_FCAST_CURRCOND));

        m_nam->get(request);
    }
    // TODO if m_position is not valid.
    return;
}

void CurrentConditions::parseForecastJson(const QByteArray& jsonData){

    qDebug() << "CurrentConditions::parseForecastJson(const QByteArray& jsonData)";

    QJsonParseError jerror;
    QJsonDocument jdoc= QJsonDocument::fromJson(jsonData,&jerror);

    if(jerror.error != QJsonParseError::NoError){
        return;
        // TODO: log this error etc.
    }

    QJsonObject obj = jdoc.object();

    qDebug() << "JSON ARRAYS";

    for (const QJsonValue &t : jdoc.object()["time"].toObject().value("startPeriodName").toArray()){
        m_fcPeriodName.append(t.toString());
        qDebug() << t.toString();
    }

    for(const QJsonValue &t : jdoc.object()["time"].toObject().value("tempLabel").toArray()){
        m_fcTempLabel.append(t.toString());
        qDebug() << t.toString();
    }

    for(const QJsonValue &t : jdoc.object()["data"].toObject().value("temperature").toArray()){
        m_fcTemperature.append(t.toString());
        qDebug() << t.toString();
    }

    for(const QJsonValue &t : jdoc.object()["data"].toObject().value("weather").toArray()){
        m_fcWeather.append(t.toString());
        qDebug() << t.toString();
    }

    for(const QJsonValue &t : jdoc.object()["data"].toObject().value("iconLink").toArray()){
        m_fcIconLink.append(t.toString());
        qDebug() << t.toString();
    }

    for(const QJsonValue &t : jdoc.object()["data"].toObject().value("text").toArray()){
        m_fcWeatherDescription.append(t.toString());
        qDebug() << t.toString();
    }


    return;
}

void CurrentConditions::parseCurrCondJson(const QByteArray& jsonData){

    qDebug() << "CurrentConditions::parseCurrCondJson(const QByteArray& jsonData)";

    QJsonParseError jerror;
    QJsonDocument jdoc= QJsonDocument::fromJson(jsonData,&jerror);

    if(jerror.error != QJsonParseError::NoError){
        return;
        // TODO: log this error etc.
    }

    QJsonObject obj = jdoc.object();
    QJsonObject data = obj["data"].toObject();
    QJsonObject currentObservation = obj["currentobservation"].toObject();

    this->m_location = currentObservation["name"].toString();
    this->m_date = currentObservation["Date"].toString();
    this->m_temp = currentObservation["Temp"].toString();    //999.9 missing
    this->m_dewp = currentObservation["Dewp"].toString();    //999.9 missing
    this->m_relh = currentObservation["Relh"].toString();    //999 missing
    this->m_winds = currentObservation["Winds"].toString();  //99.9 missing
    this->m_windd = currentObservation["Windd"].toString();  //999 missing
    this->m_weatherDescription = currentObservation["Weather"].toString();
    this->m_weatherImage = currentObservation["Weatherimage"].toString();
    this->m_visibility = currentObservation["Visibility"].toString(); //777.7 unlimited 999.9 missing
    this->m_windChill = currentObservation["WindChill"].toString();

    this->m_tempScale = "°F";

    qDebug() << "Location:" << this->m_location;
    qDebug() << "Date:" << this->m_date;
    qDebug() << "Temperature F:" << this->m_temp;
    qDebug() << "Temperature Scale:" << this->m_tempScale;
    qDebug() << "Dewpoint:" << this->m_dewp;
    qDebug() << "Humidity:" << this->m_relh;
    qDebug() << "Windspeed:" << this->m_winds;
    qDebug() << "Wind Direction:" << this->m_windd;
    qDebug() << "Description:" << this->m_weatherDescription;
    qDebug() << "Current Weather Icon Name:" << this->m_weatherImage;
    qDebug() << "Visibility:" << this->m_visibility;
    qDebug() << "Windchill:" << this->m_windChill;

    // TODO: move local var ui assignment to a display func.

//    if(m_settings.value("temperatureUnits") == "Celcius"){
//        if(this->m_temp == "999.9") {m_temp = "No Data";} else {m_temp = QString::number(44.43, "f", 0);}
//    }

    if(m_settings.value("temperatureUnits") == "Celcius"){
        this->m_tempScale = "°C";
    }

    if(this->m_temp == "999.9"){
        this->m_temp = "No Data";
        this->m_windChill = "No Data";
    }else if(m_settings.value("temperatureUnits") == "Celcius"){
        this->m_temp = QString::number(farenheitToCelcius(this->m_temp.toDouble()), 'f', 0);
        this->m_windChill = QString::number(farenheitToCelcius(this->m_windChill.toDouble()), 'f', 0);
    }

    if(this->m_dewp == "999.9"){
        this->m_dewp = "No Data";
    }else if(m_settings.value("temperatureUnits") == "Celcius"){
        this->m_dewp = QString::number(farenheitToCelcius(this->m_dewp.toDouble()), 'f', 0);
    }

    if(this->m_relh == "999"){
        this->m_dewp = "No Data";
    }

    if(this->m_winds == "99.9"){
        this->m_winds = "No Data";
    }else if(m_settings.value("speedUnits") == "kph"){ //[mph|kph|knots|m/s|ft/s]
        this->m_winds = QString::number( m_winds.toDouble() * mph_to_kph, 'f', 0);
    }else if(m_settings.value("speedUnits") == "knots"){
        this->m_winds =  QString::number( m_winds.toDouble() * mph_to_kts, 'f', 0);
    }else if(m_settings.value("speedUnits") == "m/s"){
        this->m_winds =  QString::number( m_winds.toDouble() * mph_to_mps, 'f', 0);
    }else if(m_settings.value("speedUnits") == "ft/s"){
        this->m_winds =  QString::number( m_winds.toDouble() * mph_to_ftps, 'f', 0);
    }
    
    if(this->m_windd == "999"){
        this->m_windd = "No Data";
    }
    
    if(this->m_visibility == "777.7"){
        this->m_visibility = "Unlimited";
    } else if(this->m_visibility == "999.9"){
        this->m_visibility = "No Data";
    } else if(m_settings.value("distanceUnits") == "Kilometers"){
        this->m_visibility = QString::number( m_visibility.toDouble() * mi_to_km, 'f', 0);
    }

    this->m_currentTemperatureDisplay->setText(this->m_temp);
    this->m_currentTemperatureScaleDisplay->setText(this->m_tempScale);
    this->m_currentLocationDisplay->setText(this->m_location);
    this->m_currentShortWeatherDiscriptionDisplay->setText(this->m_weatherDescription);
    this->m_currentDateDisplay->setText(this->m_date);
    this->m_currentDewPointDisplay->setText(this->m_dewp);
    this->m_currentHumidityDisplay->setText(this->m_relh);
    this->m_currentWindSpeedDisplay->setText(this->m_winds);
    this->m_currentWindDirectionDisplay->setText(this->m_windd);
    this->m_currentVisibilityDisplay->setText(this->m_visibility);
    this->m_currentWindChillDisplay->setText(this->m_windChill);

    reqCurrCondIcon(this->m_weatherImage);

    return;
}

void CurrentConditions::reqCurrCondIcon(QString iconName){

    QNetworkRequest request(QUrl(QString::fromStdString(std::string("http://forecast.weather.gov/newimages/large/"
                                         + iconName.toStdString() ))));
    request.setAttribute(QNetworkRequest::User, QVariant(ReqType::NOAA_ICON));

    m_nam->get(request);
    return;
}

void CurrentConditions::slotHandleNetworkReplies(QNetworkReply* reply){

    // Switch on ReqType.

    qDebug() << "CurrentConditions::slotHandleNetworkReplies";

    switch (reply->request().attribute(QNetworkRequest::User).toInt()){

    case ReqType::NOAA_ICON:{

        QPixmap qp;

        qp.loadFromData(reply->readAll());

        if( !qp.isNull() ){
            this->m_weatherIcon = new QPixmap(qp);
        }

        emit(signalWeatherIconDownloadDone());

        return;
    }
    case ReqType::NOAA_JSON_FCAST_CURRCOND:{

        QByteArray r_data = reply->readAll();

        qDebug() << "The Requested URL:" << reply->url();

        parseCurrCondJson(r_data);

        parseForecastJson(r_data);

        return;
    }

    case ReqType::NOAA_LAT_LON_FROM_ZIP:{

        QByteArray r_data = reply->readAll();

        qDebug() << r_data;
        // Construct the regex applied to the reply data.
        // Regex example match: <latLonList>40.9459,-74.2455</latLonList>
        QRegularExpression re("<latLonList>([\\-\\+]?\\d+(?:\\.\\d+)?),\\s*([\\-\\+]?\\d+(?:\\.\\d+)?)</latLonList>");

        // Apply the regex.
        QRegularExpressionMatchIterator matches = re.globalMatch(r_data);

        if (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();

            this->m_latitude = match.captured(1).toDouble();
            this->m_longitude = match.captured(2).toDouble();
            this->m_position = QGeoCoordinate(m_latitude, m_longitude);

            reqCurrCond(this->m_position);

            //qDebug() << "LAT LON" << match.captured(1) << match.captured(2);
        }

        return;

    }

    default:{
        qDebug() << "default case";
    }}

    return;
}
