#include <QNetworkReply>
#include <QDebug>
#include <QFile>
#include <QTcpSocket>
#include <QDir>

#include "scnetworkaccessmanager.h"

SCNetworkAccessManager::SCNetworkAccessManager(QObject *parent)
    : QNetworkAccessManager(parent){

    qDebug() << "SCNetAccess::SCNetAccess called.";

}

SCNetworkAccessManager::~SCNetworkAccessManager(){

}

QNetworkReply*	SCNetworkAccessManager::createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData ){

    QNetworkRequest requestCopy = req;

    qDebug() << "SCNetAccess::createRequest called.";

    if( req.url().toString().compare("http://www.nws.noaa.gov/forecasts/xml/DWMLgen/schema/DWML.xsd") == 0){
        //QString path = QDir::currentPath() + QString("/Resources/xml.xsd");
        //QUrl newUrl(path);
        requestCopy.setUrl(QUrl("http://graphical.weather.gov/xml/DWMLgen/schema/DWML.xsdg"));
        qDebug() << "XSD Replaced";
    } else if( req.url().toString().compare("http://www.weather.gov/view/current_observation.xsd") == 0){
        requestCopy.setUrl(QUrl("http://www.nws.noaa.gov/view/current_observation.xsd"));
        qDebug() << "XSD Replaced";
    }else{
        requestCopy.setUrl(req.url());
    }

    qDebug() << "SCNet arg req" << req.url().toString();
    qDebug() << "SCNet copy req" << requestCopy.url().toString();


    return QNetworkAccessManager::createRequest(op, requestCopy, outgoingData);
}
