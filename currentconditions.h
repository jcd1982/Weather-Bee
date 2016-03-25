#ifndef CURRENTCONDITIONS_H
#define CURRENTCONDITIONS_H

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QVariant>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlQuery>
#include <QBuffer>
#include <QXmlFormatter>
#include <QGeoCoordinate>
#include <QRegularExpression>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QWidget>
#include <QLabel>
#include <QSettings>

#include "reqtype.h"
#include "scnetworkaccessmanager.h"

// TODO: accounting list for requests
// TODO: why is weather data public?

class CurrentConditions : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentConditions(QString zip, QWidget *parent = 0);
    explicit CurrentConditions(QGeoCoordinate position, QWidget *parent = 0);

    QString m_location;
    QString m_date;
    QString m_temp;
    QString m_tempScale;
    QString m_dewp;
    QString m_relh;
    QString m_winds;
    QString m_windd;
    QString m_weatherDescription;
    QString m_weatherImage;
    QString m_visibility;
    QString m_windChill;
    QPixmap *m_weatherIcon;

    QStringList m_fcPeriodName;
    QStringList m_fcTempLabel;
    QStringList m_fcTemperature;
    QStringList m_fcWeather;
    QStringList m_fcIconLink;
    QStringList m_fcWeatherDescription;

private:

    void init();
    void reqCurrCond(const QString& zip);
    void reqCurrCond(const QGeoCoordinate& position);
    void parseCurrCondJson(const QByteArray& jsonData);
    void parseForecastJson(const QByteArray& jsonData);
    void reqCurrCondIcon(QString iconName);

    SCNetworkAccessManager* m_nam;

    QSettings m_settings;

    double m_latitude;
    double m_longitude;

    QString m_zip;

    QGeoCoordinate m_position;

    QLabel *m_currentConditionsIconDisplay;
    QLabel *m_currentTemperatureDisplay;
    QLabel *m_currentTemperatureScaleDisplay;
    QLabel *m_currentLocationDisplay;
    QLabel *m_currentShortWeatherDiscriptionDisplay;
    QLabel *m_currentDateDisplay;
    QLabel *m_currentDewPointDisplay;
    QLabel *m_currentHumidityDisplay;
    QLabel *m_currentWindSpeedDisplay;
    QLabel *m_currentWindDirectionDisplay;
    QLabel *m_currentVisibilityDisplay;
    QLabel *m_currentWindChillDisplay;



signals:

    void signalLatLonValid();
    void signalWeatherIconDownloadDone();

public slots:

private slots:

    void slotHandleNetworkReplies(QNetworkReply* reply);
    void slotDrawWeatherIcon();
    //void slotTest(QNetworkReply* r);
};

#endif // CURRENTCONDITIONS_H
