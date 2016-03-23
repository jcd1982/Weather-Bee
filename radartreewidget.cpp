#include <QDomDocument>
#include <QNetworkReply>
#include <QSettings>

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
    request.setUrl(QUrl("http://www.srh.noaa.gov/jetstream/doppler/ridge_download.htm#radar"));
    request.setAttribute(QNetworkRequest::User, QVariant("RadarIDs"));
    this->m_nam->get(request);

    connect(this->m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotHandleNetworkReplies(QNetworkReply*)));

    return;
}

void RadarTreeWidget::slotHandleNetworkReplies(QNetworkReply* reply){

    qDebug() << "RadarTreeWidget::slotHandleNetworkReplies(QNetworkReply* reply)";

    // Set the title text for the tree.
    // This has to occur after instantiation, even though
    // this is an odd spot for this to occur (i.e. within slotHandleNetworkReplies)
    // it is a good time to take care of this, as this function is
    // guarenteed to be called after instantiation.
    this->headerItem()->setText(0, tr("Radar Station"));

    if( reply->request().attribute(QNetworkRequest::User).toString() == "RadarIDs")
    // Extract the Radar Site Description and Station IDs from HTML.

    // TODO: This function needs to be meta level network
    //  handeling, and the below code needs to be broken up into:
    //   PARSING and FILLDATA

    // PARSING
    // TODO: This whole RadarID DOM extraction, and tree creation needs to be stepwise commented,
    //  and it will be a good idea to provide a source example, i.e. the text being parsed.
    {
        // Need to remove the <script> ... </script> at the end of the xhtml
        // for the site: "http://www.srh.noaa.gov/jetstream/doppler/ridge_download.htm#radar" in
        // order to make it valid xml so that the Dom Engine can parse it.
        QString replyHtml(reply->readAll());

        //qDebug() << "Radar Sites page:" << replyHtml;

        QStringList replyList = replyHtml.split(QRegExp("<script>"));

        QDomDocument doc;
        QString errorMsg;
        int errorLine, errorColumn;

        if( !doc.setContent( replyList.at(0), &errorMsg, &errorLine, &errorColumn ) ) {
            qDebug() << "ERROR IN setContent" << errorMsg << errorLine;
            //TODO: add defaulting
            return;
        }

        // <html>
        QDomElement rootElement = doc.namedItem("html").toElement();

        if ( rootElement.isNull() ) {
            qWarning() << "No <html> tag found at:" << reply->request().url();
            return;
            //TODO: add defaulting.
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

    }

    return;
}
