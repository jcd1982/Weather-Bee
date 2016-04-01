#include <QDomDocument>
#include <QNetworkReply>
#include <QSettings>
#include <QFile>

#include "radartreewidget.h"

RadarTreeWidget::RadarTreeWidget(QWidget *parent) :
  QTreeWidget(parent),
  m_isValid(false)
{
    init();
}

void RadarTreeWidget::init(){

    this->m_nam = new SCNetworkAccessManager(this);

    this->setToolTip(tr("Double-Click to change station now"));

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEmitDoubleClick()));

    reqRadarIDs();

    return;
}

void RadarTreeWidget::slotEmitDoubleClick(){

    qDebug() << "RadarTreeWidget::slotEmitDoubleClick()";

    if(this->currentItem()->parent() == nullptr){
        return;
    }

    emit(signalDoubleClick(this->currentItem()->text(this->currentColumn()).split(" - ").at(0).toUpper()));
    return;
}

void RadarTreeWidget::reqRadarIDs(){

    QNetworkRequest request;
    request.setUrl(QUrl("http://www.srh.noaa.gov/jetstream/doppler/ridge_download.html#radar"));
    request.setAttribute(QNetworkRequest::User, QVariant("RadarIDs"));
    this->m_nam->get(request);

    connect(this->m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotHandleNetworkReplies(QNetworkReply*)));

    return;
}

void RadarTreeWidget::parseRadarIDs(QString replyHtml, bool fallback){

    if(fallback){
        QFile in(":/datasource/noaa_radar_station_list_source.html");
        in.open(QIODevice::ReadOnly);

        QTextStream sin(&in);
        replyHtml = sin.readAll();
        in.close();
    }

    // Ugly, I know. But I couldn't find a better source of radar stations that was activly maintained.
    QStringList l1 = replyHtml.split(QRegExp("</head>"));
    qDebug() << l1.size();

    if(l1.size() < 2 && !fallback){

        parseRadarIDs(replyHtml, true);
        return;
    }
    QString s1 = l1.at(1);
    QStringList l2 = s1.split(QRegExp("\n</select></li></ul>\n\n</div></div>\n\n<nav>\n<ul class=\"pager hidden-print\">\n"));
    qDebug() << l2.size();
    if(l2.size() < 2 && !fallback){
        parseRadarIDs(replyHtml, true);
        return;
    }
    QString s2 = l2.at(0);
    QStringList l3 = s2.split(QRegExp("<option>yux - Yuma, AZ</option>\n</select>\n</li></ul>\n\n\n<ul class=\"list-unstyled\">\n<li><b>Radars ID's by State</b><br>\n<select size=\"10\">\n"));
    qDebug() << l3.size();
    if(l3.size() < 2 && !fallback){
        parseRadarIDs(replyHtml, true);
        return;
    }
    QString cleanedHtml = "<html><body>" + l3.at(1) + "</body></html>";

    /*QString cleanedHtml = "<html><body>" +
            replyHtml.split(QRegExp("</head>")).at(1).
            split(QRegExp("\n</select></li></ul>\n\n</div></div>\n\n<nav>\n<ul class=\"pager hidden-print\">\n")).at(0).
            split(QRegExp("<option>yux - Yuma, AZ</option>\n</select>\n</li></ul>\n\n\n<ul class=\"list-unstyled\">\n<li><b>Radars ID's by State</b><br>\n<select size=\"10\">\n")).at(1)
            + "</body></html>";
*/
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;

    if( !doc.setContent( cleanedHtml, &errorMsg, &errorLine, &errorColumn ) ) {
        qDebug() << "ERROR IN setContent" << errorMsg << "Line" << errorLine << "Col" << errorColumn;

        if(!fallback){
            parseRadarIDs(replyHtml, true);
        }

        return;
    }

    // <html>
    QDomElement rootElement = doc.namedItem("html").toElement();

    if ( rootElement.isNull() ) {
        qWarning() << "No <html> tag found:";

        if(!fallback){
            parseRadarIDs(replyHtml, true);
        }

        return;
    }

    ///////////////////////////////////////////////////////////
    ///
    /// <optgroup label="Alabama">
    /// <option>bmx - Birmingham</option>
    /// <option>mxx - E. Alabama</option>
    /// <option>eox - Fort Rucker</option>
    /// <option>mob - Mobile</option>
    /// <option>htx - Nrn. Alabama</option></optgroup>
    /// <optgroup label="Alaska">
    ///
    ///////////////////////////////////////////////////////////

    // Search entire Dom under root (i.e. <html>) for <optgroup> nodes, make a list of them,
    // these are the states.
    QDomNodeList statesNodeList = rootElement.elementsByTagName("optgroup");

    if( !statesNodeList.isEmpty() ){

        for(int i = 0; i < statesNodeList.count(); ++i){

            QDomElement stateElement = statesNodeList.at(i).toElement();

            QString state(stateElement.attribute("label"));     // Get the states name.

            //qDebug() << "state:" << state;

            // Search the Dom under <optgroup> nodes for <option> nodes, these are the indivividual
            // weather stations.
            QDomNodeList stationNodeList = stateElement.elementsByTagName("option");

            QList<QString> id_cities;

            if( !stationNodeList.isEmpty() ){

                for(int j = 0; j < stationNodeList.count(); ++j){
                    // Extract relavent information.

                    QDomElement stationElement = stationNodeList.at(j).toElement();
                    QString station_id(stationElement.text().split(QRegExp(" - ")).at(0));
                    QString city(stationElement.text().split(QRegExp(" - ")).at(1));

                    //qDebug() << "station_id:" << station_id;
                    //qDebug() << "city:" << city;

                    std::pair<QString, QString> pair_city_state(city, state);
                    std::pair<QString, std::pair<QString, QString>> pair_id_pair_city_state(station_id, pair_city_state);

                    if(station_id != "" ){
                        id_cities.append(stationElement.text());
                    }
                }
                if( m_map_station_list_by_state.count(state) == 0 && state != ""){
                    m_map_station_list_by_state.insert(std::pair<QString, QList<QString>> (state, id_cities));
                }
            }
        }
    }
    // FILLDATA
    QTreeWidgetItem *saved = nullptr;

    QSettings settings;

    for(std::pair<QString, QList<QString>> s: m_map_station_list_by_state){

        // Fill in the tree data.
        QTreeWidgetItem *twi_root = new QTreeWidgetItem(this);//ui->treeWidgetRadarStations);

        twi_root->setText(0,s.first);

        //qDebug() << "State" << s.first;

        for( QString c: s.second ){
            QTreeWidgetItem *twi = new QTreeWidgetItem();
            twi->setText(0,c);
            twi_root->addChild(twi);
            if(settings.contains("radarStationID") && c == settings.value("radarStationID").toString()){
                saved = twi;
                qDebug() << "Saved Radar Station:" << settings.value("radarStationID").toString();
            }
            //qDebug() << "Station" << c;
        }
    }
    if(saved != nullptr){

        saved->parent()->setExpanded(true);
        saved->setSelected(true);
        this->scrollToItem(saved);
        this->setCurrentItem(saved);
    }

    saved = nullptr;
    // SET DEFAULT SELECTED




    return;
}

void RadarTreeWidget::slotHandleNetworkReplies(QNetworkReply* reply){

    qDebug() << "RadarTreeWidget::slotHandleNetworkReplies(QNetworkReply* reply)";

    this->headerItem()->setText(0, tr("Radar Station"));

    // TODO: add redirect. . . other return code handling and send RadarIDs to fallback.

    if( reply->request().attribute(QNetworkRequest::User).toString() == "RadarIDs")
    {
        parseRadarIDs(reply->readAll());

        ///////

    }

    return;
}
