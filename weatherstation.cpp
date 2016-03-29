#include <QSettings>

#include "weatherstation.h"

class RadarProduct;

WeatherStation::WeatherStation(QString sid, RadarSettings settings, QObject *parent)
    : QObject{parent},
      m_station_id{sid},
      m_base_url{"http://radar.weather.gov/ridge"},
      m_current_image_key{""}
{
    m_overlay_settings.topoEnabled = settings.topoEnabled;
    m_overlay_settings.cityEnabled = settings.cityEnabled;
    m_overlay_settings.countiesEnabled = settings.countiesEnabled;
    m_overlay_settings.highwaysEnabled = settings.highwaysEnabled;
    m_overlay_settings.legendEnabled = settings.legendEnabled;
    m_overlay_settings.rangeEnabled = settings.rangeEnabled;
    m_overlay_settings.riversEnabled = settings.riversEnabled;
    m_overlay_settings.warningsEnabled = settings.warningsEnabled;
    m_overlay_settings.radarOpacity = settings.radarOpacity;

    m_base_image_download_done = false;

    m_nam = new QNetworkAccessManager(this);

    /* Urls for static overlays. */

    m_topo_short.m_url = m_base_url + "/Overlays/Topo/Short/" + m_station_id + "_Topo_Short.jpg";
    m_topo_long.m_url  = m_base_url + "/Overlays/Topo/Long/" + m_station_id + "_Topo_Long.jpg";

    m_county_short.m_url = m_base_url + "/Overlays/County/Short/" + m_station_id + "_County_Short.gif";
    m_county_long.m_url  = m_base_url + "/Overlays/County/Long/" + m_station_id + "_County_Long.gif";

    m_rivers_short.m_url = m_base_url + "/Overlays/Rivers/Short/" + m_station_id + "_Rivers_Short.gif";
    m_rivers_long.m_url = m_base_url + "/Overlays/Rivers/Long/" + m_station_id + "_Rivers_Long.gif";

    m_highways_short.m_url = m_base_url + "/Overlays/Highways/Short/" + m_station_id + "_Highways_Short.gif";
    m_highways_long.m_url = m_base_url + "/Overlays/Highways/Long/" + m_station_id + "_Highways_Long.gif";

    m_city_short.m_url = m_base_url + "/Overlays/Cities/Short/" + m_station_id + "_City_Short.gif";
    m_city_long.m_url = m_base_url + "/Overlays/Cities/Long/" + m_station_id + "_City_Long.gif";

    m_range_ring_short.m_url = m_base_url + "/Overlays/RangeRings/Short/" + m_station_id + "_RangeRing_Short.gif";
    m_range_ring_long.m_url = m_base_url + "/Overlays/RangeRings/Long/" + m_station_id + "_RangeRing_Long.gif";


    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_topo_short.m_url, ReqType::topo_s});
    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_topo_long.m_url, ReqType::topo_l});

    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_county_short.m_url, ReqType::county_s});
    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_county_long.m_url, ReqType::county_l});

    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_rivers_short.m_url, ReqType::rivers_s});
    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_rivers_long.m_url, ReqType::rivers_l});

    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_highways_short.m_url, ReqType::highways_s});
    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_highways_long.m_url, ReqType::highways_l});

    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_city_short.m_url, ReqType::city_s});
    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_city_long.m_url, ReqType::city_l});

    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_range_ring_short.m_url, ReqType::range_ring_s});
    m_base_overlays_download_list.push_back(std::pair<QString, ReqType>{m_range_ring_long.m_url, ReqType::range_ring_l});

    makeRequest(m_base_overlays_download_list);

    // Instantiate each radar product.
    m_radar_product_N0R = new RadarProduct(m_base_url, m_station_id, RadarType::N0R, "N0R", this);
    m_radar_product_N0Z = new RadarProduct(m_base_url, m_station_id, RadarType::N0Z, "N0Z", this);
    m_radar_product_N0S = new RadarProduct(m_base_url, m_station_id, RadarType::N0S, "N0S", this);
    m_radar_product_N0V = new RadarProduct(m_base_url, m_station_id, RadarType::N0V, "N0V", this);
    m_radar_product_N1P = new RadarProduct(m_base_url, m_station_id, RadarType::N1P, "N1P", this);
    m_radar_product_NCR = new RadarProduct(m_base_url, m_station_id, RadarType::NCR, "NCR", this);
    m_radar_product_NTP = new RadarProduct(m_base_url, m_station_id, RadarType::NTP, "NTP", this);

    // Create a RadarType Map of pointers to each radar product instance.
    m_radar_products.insert(RadarType::N0R, m_radar_product_N0R);
    m_radar_products.insert(RadarType::N0Z, m_radar_product_N0Z);
    m_radar_products.insert(RadarType::N0S, m_radar_product_N0S);
    m_radar_products.insert(RadarType::N0V, m_radar_product_N0V);
    m_radar_products.insert(RadarType::N1P, m_radar_product_N1P);
    m_radar_products.insert(RadarType::NCR, m_radar_product_NCR);
    m_radar_products.insert(RadarType::NTP, m_radar_product_NTP);

    // Initialize each RadarType Map of lists of pending radar image replies.
    m_radar_images_pending_replies.insert(RadarType::N0R, QList<QNetworkReply*>());
    m_radar_images_pending_replies.insert(RadarType::N0Z, QList<QNetworkReply*>());
    m_radar_images_pending_replies.insert(RadarType::N0S, QList<QNetworkReply*>());
    m_radar_images_pending_replies.insert(RadarType::N0V, QList<QNetworkReply*>());
    m_radar_images_pending_replies.insert(RadarType::N1P, QList<QNetworkReply*>());
    m_radar_images_pending_replies.insert(RadarType::NCR, QList<QNetworkReply*>());
    m_radar_images_pending_replies.insert(RadarType::NTP, QList<QNetworkReply*>());

    // Initialize each RadarType Map of lists of pending legend image replies.
    m_legend_images_pending_replies.insert(RadarType::N0R, QList<QNetworkReply*>());
    m_legend_images_pending_replies.insert(RadarType::N0Z, QList<QNetworkReply*>());
    m_legend_images_pending_replies.insert(RadarType::N0S, QList<QNetworkReply*>());
    m_legend_images_pending_replies.insert(RadarType::N0V, QList<QNetworkReply*>());
    m_legend_images_pending_replies.insert(RadarType::N1P, QList<QNetworkReply*>());
    m_legend_images_pending_replies.insert(RadarType::NCR, QList<QNetworkReply*>());
    m_legend_images_pending_replies.insert(RadarType::NTP, QList<QNetworkReply*>());

    // Initialize each RadarType Map of lists of pending warning image replies.
    m_warning_images_pending_replies.insert(RadarType::N0R, QList<QNetworkReply*>());
    m_warning_images_pending_replies.insert(RadarType::N0Z, QList<QNetworkReply*>());
    m_warning_images_pending_replies.insert(RadarType::N0S, QList<QNetworkReply*>());
    m_warning_images_pending_replies.insert(RadarType::N0V, QList<QNetworkReply*>());
    m_warning_images_pending_replies.insert(RadarType::N1P, QList<QNetworkReply*>());
    m_warning_images_pending_replies.insert(RadarType::NCR, QList<QNetworkReply*>());
    m_warning_images_pending_replies.insert(RadarType::NTP, QList<QNetworkReply*>());

    // TODO: follow these lists of QNetworkReply pointers to destruction, make sure they are
    //  deleted properly.

    m_refresh_timer.setInterval(settings.radarRefreshInterval * 60000);
    m_refresh_timer.start();

    connect(this->m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotHandleNetworkReplies(QNetworkReply*)));
    connect(&m_refresh_timer, SIGNAL(timeout()), this, SLOT(slotRefreshRadarProduct()));
    // TODO: make sure m_refresh_timer is disconnected when appropriate.

    //connect(this, SIGNAL(signalRadarProductDownloadDone(RadarType)), this, SLOT(slotStartRefreshTimer(RadarType)));

    return;
}

QString WeatherStation::getStationId(){
    return this->m_station_id;
}

RadarType WeatherStation::getCurrentRadarProductType(){
    return this->m_current_radar_product->m_radar_type;
}

bool WeatherStation::setRadarProduct(RadarType rt){

    // Returns true on success and false otherwise.
    if( this->m_radar_products.count(rt) == 0 ){
        return false;
    }

    this->m_current_radar_product = this->m_radar_products[rt];

    this->curr_img_idx = 0;

    if(this->m_current_radar_product->m_radar.size() <= 0)
    /// This product doesn't have data, can request full download.
    {
        qDebug() << "No images loaded, Requesting radar images. . . ";

        this->m_dynamic_images_location_list_download_list.push_back(
                    std::pair<QString, ReqType>{this->m_current_radar_product->m_radar_image_file_list_url,
                                                ReqType::radar_image_file_list});

        this->m_dynamic_images_location_list_download_list.push_back(
                    std::pair<QString, ReqType>{this->m_current_radar_product->m_legend_image_file_list_url,
                                                ReqType::legend_image_file_list});
        QString warning_url;

        if(rt == RadarType::N0Z){
            warning_url = this->m_current_radar_product->m_warning_image_long_file_list_url;
        }else{
            warning_url = this->m_current_radar_product->m_warning_image_short_file_list_url;
        }
        this->m_dynamic_images_location_list_download_list.push_back(
                    std::pair<QString, ReqType>{warning_url, ReqType::warning_image_file_list});

        this->makeRequest(this->m_dynamic_images_location_list_download_list, rt);

        return true;

    }else{
        emit(this->signalRadarProductDownloadDone(rt));
        return true;
    }
}

void WeatherStation::setRadarSettings(RadarSettings s){

    qDebug("WeatherStation::setOverlaySettings");

    this->m_overlay_settings.topoEnabled = s.topoEnabled;
    this->m_overlay_settings.cityEnabled = s.cityEnabled;
    this->m_overlay_settings.countiesEnabled = s.countiesEnabled;
    this->m_overlay_settings.highwaysEnabled = s.highwaysEnabled;
    this->m_overlay_settings.legendEnabled = s.legendEnabled;
    this->m_overlay_settings.rangeEnabled = s.rangeEnabled;
    this->m_overlay_settings.riversEnabled = s.riversEnabled;
    this->m_overlay_settings.warningsEnabled = s.warningsEnabled;
    this->m_overlay_settings.radarOpacity = s.radarOpacity;

    return;
}

QList<QString> WeatherStation::getCurrentKeys(){

    qDebug() << "WeatherStation::getCurrentKeys";

    QList<QString> keys;

    // Determine the key set that is used to access the images.
    // The different dynamic images have different indexes (keys) in
    // common, and this code chooses the key set which is required
    // based on the users selection of overlays.
    if( !m_overlay_settings.legendEnabled && !m_overlay_settings.warningsEnabled ){
    //if(!ui->checkLegend->isChecked() && !ui->checkWarnings->isChecked()){
        // Radar
        keys = m_current_radar_product->m_radar.keys();
    }else if( m_overlay_settings.legendEnabled && !m_overlay_settings.warningsEnabled ){
        //if(ui->checkLegend->isChecked() && !ui->checkWarnings->isChecked()){
        // Radar and Legend
        keys = m_current_radar_product->m_common_legend;
    }else if( !m_overlay_settings.legendEnabled && m_overlay_settings.warningsEnabled ){
        // if(!ui->checkLegend->isChecked() && ui->checkWarnings->isChecked()){
        // Radar and Warnings
        keys = m_current_radar_product->m_common_warning;
    }else if( m_overlay_settings.legendEnabled && m_overlay_settings.warningsEnabled ){
        // if(ui->checkLegend->isChecked() && ui->checkWarnings->isChecked()){
        // Radar, Legend, and Warnings
        keys = m_current_radar_product->m_common_legend_warning;
    }

    return keys;
}

QPixmap* WeatherStation::getBaseImage(){

    qDebug() << "WeatherStation::getBaseImage";


    if( !this->m_base_image_download_done ){
        return nullptr;
    }

    QPixmap* rendered = new QPixmap(600, 550);
    // TODO: make background configurable in the UI settings (Black or white)

    QSettings settings;
    if( settings.value("radarBackgroundColor").toString() == "White"){
        rendered->fill(Qt::white);
    }else if(settings.value("radarBackgroundColor").toString() == "Black"){
        rendered->fill(Qt::black);
    }

    QPainter painter(rendered);

    if( m_current_radar_product->m_radar_type == RadarType::N0Z ){

        if( m_overlay_settings.topoEnabled ){
            painter.drawPixmap(0,0, m_topo_long.m_image);
        }
        if( m_overlay_settings.riversEnabled ){
            painter.drawPixmap(0,0, m_rivers_long.m_image);
        }
        if( m_overlay_settings.highwaysEnabled ){
            painter.drawPixmap(0,0, m_highways_long.m_image);
        }
        if( m_overlay_settings.countiesEnabled ){
            painter.drawPixmap(0,0, m_county_long.m_image);
        }
        if( m_overlay_settings.rangeEnabled ){
            // The range ring for the long range product (N0Z) is either misleading, erroneous,
            // or misunderstood.  The site: 'http://www.srh.noaa.gov/jetstream/doppler/ridge_download.htm',
            // which details the radar data, states that the N0Z product provides data out to 248nmi.
            // Yet, the range ring does not expand (as of 12/8/15) beyond the short
            // range radars range ring (i.e. the long version of the ring encompasses the same geography.
            // of the short version.)
            // Also, instances were observed where the long drawn ring was smaller than the long
            // displayed radar data.
            //
            // It will be kept out here in order to minimize error reporting.
            //

            //painter.drawPixmap(0,0, m_range_ring_long.m_image);

        }
    }else{
        qDebug() << "In paint topo enabled" << m_overlay_settings.topoEnabled;
        if( m_overlay_settings.topoEnabled ){
            painter.drawPixmap(0,0, m_topo_short.m_image);
        }
        if( m_overlay_settings.riversEnabled ){
            painter.drawPixmap(0,0, m_rivers_short.m_image);
        }
        if( m_overlay_settings.highwaysEnabled ){
            painter.drawPixmap(0,0, m_highways_short.m_image);
        }
        if( m_overlay_settings.countiesEnabled ){
            painter.drawPixmap(0,0, m_county_short.m_image);
        }
        if( m_overlay_settings.rangeEnabled ){
            painter.drawPixmap(0,0, m_range_ring_short.m_image);
        }
    }

    return rendered;
}

QPixmap* WeatherStation::getLatestImage(){

    qDebug() << "WeatherStation::getLatestImage";

    QList<QString> keys;

    keys = this->getCurrentKeys();

    if(keys.isEmpty()){
        qDebug("Image Empty");
        return nullptr;
    }

    this->m_current_image_key = keys.back();

    this->curr_img_idx = keys.indexOf(this->m_current_image_key);

    QPixmap *rendered = getBaseImage();

    if( rendered->isNull() ){
        return nullptr;
    }

    renderRadar(this->m_current_image_key, rendered);

    return rendered;

}

QPixmap* WeatherStation::getCurrentImage(){

    qDebug() << "WeatherStation::getCurrentImage";

    int size;
    QList<QString> keys;

    keys = this->getCurrentKeys();

    size = keys.size();

    if(keys.isEmpty()){
        qDebug("Image Empty");
        return nullptr;
    }

    if( !keys.contains(this->m_current_image_key) ){
        this->m_current_image_key = keys.last();
    }

    this->curr_img_idx = keys.indexOf(this->m_current_image_key);

    qDebug() << "current image list size:" << size << "Current image number:" << this->curr_img_idx;

    QPixmap *rendered = getBaseImage();

    if( rendered->isNull() ){
        return nullptr;
    }

    renderRadar(this->m_current_image_key, rendered);

    return rendered;

}

QPixmap* WeatherStation::getNextImage(){

    qDebug() << "WeatherStation::getNextImage";

    int size;
    QList<QString> keys;

    keys = this->getCurrentKeys();

    if(keys.isEmpty()){
        qDebug("Image Empty");
        return nullptr;
    }

    size = keys.size();

    if(curr_img_idx >= size) curr_img_idx = 0;

    this->m_current_image_key = keys.at(this->curr_img_idx);

    qDebug() << "current image list size:" << size << "Current image number:" << this->curr_img_idx;

    QPixmap *rendered = getBaseImage();

    if( rendered->isNull() ){
        return nullptr;
    }

    renderRadar(this->m_current_image_key, rendered);

    curr_img_idx += 1;

    return rendered;

}

void WeatherStation::renderRadar(QString key, QPixmap* base){
    ////////////////////////////////////////////
    /// \brief painter
    /// \return
    /// Renders the Radar/Legend/Warning layer onto
    /// the passed QPixmap* base parameter.
    ///
    /// key is the key into the m_radar, m_legend
    /// and m_warning hash maps.
    ///
    /// base is the image that will be rendered onto.
    ////////////////////////////////////////////

    qDebug() << "WeatherStation::renderRadar";

    QPainter painter(base);

    if(m_current_radar_product->m_radar[key].m_image.isNull()){
        qDebug() << "Invalid m_radar_products key."
                 << __FILE__ << __LINE__;
        exit(-1);
    }

    QImage image(m_current_radar_product->m_radar[key].m_image.size(), QImage::Format_ARGB32_Premultiplied);

    qDebug() << "Image Size" << m_current_radar_product->m_radar[key].m_image.size();

    {   // an un-needed logical block for comprehension purposes.

        // A new QPainter is needed to change the opacity of the radar image.
        QPainter painter_t(&image);

        painter_t.setCompositionMode(QPainter::CompositionMode_Source);
        //qDebug() << m_overlay_settings.radarOpacity;
        painter_t.setOpacity(m_overlay_settings.radarOpacity);
        painter_t.drawPixmap(0, 0, m_current_radar_product->m_radar[key].m_image);
    }

    painter.drawPixmap(0, 0, QPixmap::fromImage(image));

    // The below drawPixmap call order is important, because of visibility of overlay elements.
    if( m_overlay_settings.cityEnabled && m_current_radar_product->m_radar_type == RadarType::N0Z ){
        painter.drawPixmap(0,0, m_city_long.m_image);
    }else if( m_overlay_settings.cityEnabled ){
        painter.drawPixmap(0,0, m_city_short.m_image);
    }

    if(m_overlay_settings.warningsEnabled && m_current_radar_product->m_warning.count(key) == 1){
        painter.drawPixmap(0,0, m_current_radar_product->m_warning[key].m_image);
    }

    if(m_overlay_settings.legendEnabled && m_current_radar_product->m_legend.count(key) == 1){
        painter.drawPixmap(0,0, m_current_radar_product->m_legend[key].m_image);
    }

    return;
}

void WeatherStation::slotRefreshRadarProduct(){
    /// This method constructs and submits a new set of requests for the
    /// radar images, legend images and warning images; for the current radar
    /// type.

    qDebug() << "WeatherStation::slotRefreshRadarProduct";

    this->m_dynamic_images_location_list_download_list.push_back(
        std::pair<QString, ReqType>{this->m_current_radar_product->m_radar_image_file_list_url,
                                    ReqType::radar_image_file_list});

    this->m_dynamic_images_location_list_download_list.push_back(
        std::pair<QString, ReqType>{this->m_current_radar_product->m_legend_image_file_list_url,
                                    ReqType::legend_image_file_list});

    QString warning_url;

    if(this->m_current_radar_product->m_radar_type == RadarType::N0Z){
        warning_url = this->m_current_radar_product->m_warning_image_long_file_list_url;
    }else{
        warning_url = this->m_current_radar_product->m_warning_image_short_file_list_url;
    }

    this->m_dynamic_images_location_list_download_list.push_back(
        std::pair<QString, ReqType>{warning_url, ReqType::warning_image_file_list});

    this->makeRequest(this->m_dynamic_images_location_list_download_list, this->m_current_radar_product->m_radar_type);

    return;
}

void WeatherStation::makeRequest(const std::list<std::pair<QString, ReqType>> &reqs){

    /// args: list of pairs of Url (first) and ReqType (second)
    /// This method creates individual requests for each pair in the arg list.

    qDebug() << "WeatherStation::makeRequest";

    QNetworkRequest request;

    for(std::pair<QString, ReqType> r : reqs){
        request.setUrl(QUrl::fromEncoded(r.first.toLocal8Bit()));
        request.setAttribute(QNetworkRequest::User, r.second);
        m_nam->get(request);
    }

    return;
}
void WeatherStation::makeRequest(const std::list<std::pair<QString, ReqType>> &reqs, RadarType rt){
    /// args: list of pairs of Url (first) and ReqType (second), and RadarTypes.
    /// This method creates individual requests for each pair in the arg list.

    qDebug() << "WeatherStation::makeRequest";

    QNetworkRequest request;

    for(std::pair<QString, ReqType> r : reqs){

        request.setUrl(QUrl::fromEncoded(r.first.toLocal8Bit()));        
        request.setAttribute(QNetworkRequest::User, r.second);
        request.setAttribute(static_cast<QNetworkRequest::Attribute>(1001), rt);

        QNetworkReply* rep;
        rep = m_nam->get(request);

        if(r.second == ReqType::radar_image){
            // Have to build the accounting list.
            m_radar_images_pending_replies[rt].push_back(rep);

        }else if(r.second == ReqType::legend_image){
            m_legend_images_pending_replies[rt].push_back(rep);

        }else if(r.second == ReqType::warning_image){
            m_warning_images_pending_replies[rt].push_back(rep);

        }
    }
    return;
}

void WeatherStation::slotHandleNetworkReplies(QNetworkReply* reply){
    /// Handles all requests made by WeatherStation objects.

    bool error = false;

    if (reply->error() != QNetworkReply::NoError){
        qDebug() << "Error in" << reply->url() <<
                    ":" << reply->errorString();
        error = true;
        //TODO: add if error to the meta pages and base images
    }    

    if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()){
        // TODO: Follow the redirection.
        qDebug() << "Redirect to:" << reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        error = true;
    }

    // Switch on ReqType (Contained in QNetworkRequest::User attribute)
    switch (reply->request().attribute(QNetworkRequest::User).toInt()){

    //////////////////////////////////////
    ///
    ///     radar_image_file_list
    ///
    ///     Parse the reply data in order to obtain the list of
    ///     currently available radar images; And then make
    ///     requests for them.
    ///
    //////////////////////////////////////

    case ReqType::radar_image_file_list:{

        QByteArray r_data = reply->readAll();

        // Remove from the accounting list.
        m_dynamic_images_location_list_download_list.remove(
                    std::pair<QString, ReqType>(reply->request().url().toString(),
                                                ReqType::radar_image_file_list));

        // Get the RadarType. This is sent as an attribute to the reqest, and returned in the reply.
        QNetworkRequest::Attribute atr = static_cast<QNetworkRequest::Attribute>(1001);
        RadarType rt = static_cast<RadarType>(reply->request().attribute(atr).toInt());

        // Verify RadarType exists
        if(m_radar_products.count(rt) != 1){
            qDebug() << "Invalid m_radar_products key."
                     << "RadarType:" << rt
                     << __FILE__ << __LINE__;
            exit(-1);
        }

        // Construct the regex applied to the reply data.
        QRegularExpression re("</td><td><a href=\"" + m_station_id + "_(\\d{8}_\\d{4})_"
                              + m_radar_products[rt]->m_radar_name
                              + ".gif\">" + m_station_id);

        // Apply the regex.
        QRegularExpressionMatchIterator matches = re.globalMatch(r_data);

        unsigned int num_matches = 0;

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            //qDebug() << "Match" << match.captured(1);

            QDateTime image_time = QDateTime::fromString(match.captured(1),"yyyyMMdd_HHmm");
            QString s_image_time = image_time.toString("yyyyMMdd_HHmm");

            // If this is 'updating', check to see if the image to
            //  request already exists, and skip the request if it does.
            if(m_radar_products[rt]->m_radar.contains(s_image_time)){
                continue;
            }

            QString image_url(m_radar_products[rt]->m_radar_image_file_list_url
                              + m_station_id + "_" + match.captured(1) + "_"
                              + m_radar_products[rt]->m_radar_name + ".gif");

            m_radar_products[rt]->m_radar_image_download_list.push_back(std::pair<QString, ReqType>{image_url, ReqType::radar_image});

            ++num_matches;
        }

        makeRequest(m_radar_products[rt]->m_radar_image_download_list, rt);

        reply->deleteLater();

        return;
    }

    //////////////////////////////////////
    ///
    ///     legend_image_file_list
    ///
    ///     Parse the reply data in order to obtain the list of
    ///     currently available legend images; And then make
    ///     requests for them.
    ///
    //////////////////////////////////////

    case ReqType::legend_image_file_list:{

        QByteArray r_data = reply->readAll();

        // Remove from the accounting list.
        m_dynamic_images_location_list_download_list.remove(
                    std::pair<QString, ReqType>(reply->request().url().toString(),
                                                ReqType::legend_image_file_list));

        // Get the RadarType. This is sent as an attribute to the reqest, and returned in the reply.
        QNetworkRequest::Attribute atr = static_cast<QNetworkRequest::Attribute>(1001);
        RadarType rt = static_cast<RadarType>(reply->request().attribute(atr).toInt());

        // Verify RadarType exists
        if(m_radar_products.count(rt) != 1){
            qDebug() << "Invalid m_radar_products key."
                     << "RadarType:" << rt
                     << __FILE__ << __LINE__;
            exit(-1);
        }

        // Construct the regex applied to the reply data.
        QRegularExpression re("</td><td><a href=\"" + m_station_id
                              + "_(\\d{8}_\\d{4})_" + m_radar_products[rt]->m_radar_name
                              + "_Legend.gif\">" + m_station_id);

        // Apply the regex.
        QRegularExpressionMatchIterator matches = re.globalMatch(r_data);

        unsigned int num_matches = 0;

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();

            QDateTime image_time = QDateTime::fromString(match.captured(1),"yyyyMMdd_HHmm");
            QString s_image_time = image_time.toString("yyyyMMdd_HHmm");

            // If this is 'updating', check to see if the image to
            //  request already exists, and skip the request if it does.
            if(m_radar_products[rt]->m_legend.contains(s_image_time)){
                continue;
            }

            QString image_url(m_radar_products[rt]->m_legend_image_file_list_url
                              + m_station_id + "_" + match.captured(1) + "_"
                              + m_radar_products[rt]->m_radar_name + "_Legend.gif");

            m_radar_products[rt]->m_legend_image_download_list.push_back(std::pair<QString, ReqType>{image_url, ReqType::legend_image});

            ++num_matches;
        }

        makeRequest(m_radar_products[rt]->m_legend_image_download_list, rt);

        reply->deleteLater();

        return;
    }

    //////////////////////////////////////
    ///
    ///     warning_image_file_list
    ///
    ///     Parse the reply data in order to obtain the list of
    ///     currently available warning images; And then make
    ///     requests for them.
    ///
    //////////////////////////////////////

    case ReqType::warning_image_file_list:{

        QByteArray r_data = reply->readAll();

        // Remove from the accounting list.
        m_dynamic_images_location_list_download_list.remove(
                    std::pair<QString, ReqType>(reply->request().url().toString(),
                                                ReqType::warning_image_file_list));

        // Get the RadarType. This is sent as an attribute to the reqest, and returned in the reply.
        QNetworkRequest::Attribute atr = static_cast<QNetworkRequest::Attribute>(1001);
        RadarType rt = static_cast<RadarType>(reply->request().attribute(atr).toInt());

        // Verify RadarType exists
        if(m_radar_products.count(rt) != 1){
            qDebug() << "Invalid m_radar_products key."
                     << "RadarType:" << rt
                     << __FILE__ << __LINE__;
            exit(-1);
        }

        // Construct the regex applied to the reply data.
        // Regex example match: </td><td><a href="ABC_20151215_1743_Short_Warnings.gif">ABC
        QRegularExpression re("</td><td><a href=\"" + m_station_id + "_(\\d{8}_\\d{4})_"
                              + (rt == RadarType::N0Z?"Long":"Short") + "_Warnings.gif\">" + m_station_id);

        // Apply the regex.
        QRegularExpressionMatchIterator matches = re.globalMatch(r_data);

        unsigned int num_matches = 0;

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();

            QDateTime image_time = QDateTime::fromString(match.captured(1),"yyyyMMdd_HHmm");
            QString s_image_time = image_time.toString("yyyyMMdd_HHmm");

            // If this is 'updating', check to see if the image to
            //  request already exists, and skip the request if it does.
            if(m_radar_products[rt]->m_warning.contains(s_image_time)){
                continue;
            }

            // URL example:  http://radar.weather.gov/ridge/Warnings/Short/ABC/ABC_20151215_1743_Short_Warnings.gif
            // The RadarType::N0Z warning overlay is of the long type, and the rest
            // are short.
            QString image_url((rt == RadarType::N0Z?m_radar_products[rt]->m_warning_image_long_file_list_url:m_radar_products[rt]->m_warning_image_short_file_list_url)
                              + m_station_id + "_" + match.captured(1) + "_"
                              + (rt == RadarType::N0Z?"Long":"Short") + "_Warnings.gif");

            m_radar_products[rt]->m_warning_image_download_list.push_back(std::pair<QString, ReqType>{image_url, ReqType::warning_image});

            ++num_matches;
        }

        makeRequest(m_radar_products[rt]->m_warning_image_download_list, rt);

        reply->deleteLater();

        return;
    }

    //////////////////////////////////////
    ///
    ///     radar_image
    ///
    //////////////////////////////////////

    case ReqType::radar_image:{

        // Get the RadarType. This is sent as an attribute to the reqest, and returned in the reply.
        QNetworkRequest::Attribute atr = static_cast<QNetworkRequest::Attribute>(1001);
        RadarType rt = static_cast<RadarType>(reply->request().attribute(atr).toInt());

        // Verify RadarType exists
        if(m_radar_products.count(rt) != 1){
            qDebug() << "Invalid m_radar_products key."
                     << "RadarType:" << rt
                     << __FILE__ << __LINE__;
            exit(-1);
        }

        if(!error){

            QPixmap qp;
            qp.loadFromData(reply->readAll());

            // Extract the time signature from the request url
            QRegularExpression re("_(\\d{8}_\\d{4})_");
            QRegularExpressionMatchIterator matches = re.globalMatch(reply->request().url().toString());
            QString s_image_time = "";
            if (matches.hasNext()) {
                    QRegularExpressionMatch match = matches.next();
                    s_image_time = match.captured(1);
                    if(s_image_time == ""){
                        qDebug() << "s_image_time empty when it *REALLY* should not be."  << __FILE__ << __LINE__;
                        return;
                    }
            }

            if( !qp.isNull() ){

                RadarImage radar_image(qp, QDateTime::fromString(s_image_time,"yyyyMMdd_HHmm"),reply->request().url().toString());

                m_radar_products[rt]->addRadarImage(radar_image);
            }
        }

        // Remove the from the pending lists . .
        m_radar_products[rt]->m_radar_image_download_list.remove(
                    std::pair<QString, ReqType>(reply->request().url().toString(),
                                                ReqType::radar_image));
        m_radar_images_pending_replies[rt].removeOne(reply);

        //TODO set up an expiration of the above data
        //TODO set up a timer that triggers another download in 5 min

        reply->deleteLater();

        /*qDebug() << "Radar Images download status:"
                 << m_radar_products[rt]->m_radar_image_download_list.empty()
                 << m_radar_images_pending_replies[rt].empty();*/

        // TODO: Figure out mechanism for using x_images_download_done
        //  get rid of this flag otherwise.
        //  ... seems like it is a better idea to just display the images when one is available.
        //  ... without regard for the flag,
        //  ... as the display updates faster, but may be a good idea to somehow regulate the calls
        //  ... to draw radar, since this creates a stochotic animation

        if(m_radar_images_pending_replies[rt].empty()){
            m_radar_products[rt]->m_radar_images_download_done = true;
            if(m_radar_products[rt]->m_legend_images_download_done && m_radar_products[rt]->m_warning_images_download_done){
                emit(this->signalRadarProductDownloadDone(rt));
            }
        }



        return;
    }

    //////////////////////////////////////
    ///
    ///     legend_image
    ///
    //////////////////////////////////////

    case ReqType::legend_image:{

        // Get the RadarType. This is sent as an attribute to the reqest, and returned in the reply.
        QNetworkRequest::Attribute radar_type_attribute = static_cast<QNetworkRequest::Attribute>(1001);
        RadarType rt = static_cast<RadarType>(reply->request().attribute(radar_type_attribute).toInt());

        // Verify RadarType exists
        if(m_radar_products.count(rt) != 1){
            qDebug() << "Invalid m_radar_products key."
                     << "RadarType:" << rt
                     << __FILE__ << __LINE__;
            exit(-1);
        }

        if(!error){

            QPixmap qp;
            qp.loadFromData(reply->readAll());

            // Extract the time signature from the request url
            QRegularExpression re("_(\\d{8}_\\d{4})_");
            QRegularExpressionMatchIterator matches = re.globalMatch(reply->request().url().toString());
            QString s_image_time = "";
            if (matches.hasNext()) {
                    QRegularExpressionMatch match = matches.next();
                    //qDebug() << "REPLY Match" << match.captured(1);
                    s_image_time = match.captured(1);
                    if(s_image_time == ""){
                        qDebug() << "s_image_time empty when it *REALLY* should not be."  << __FILE__ << __LINE__;
                        return;
                    }
            }

            if( !qp.isNull() ){

                RadarImage radar_image(qp, QDateTime::fromString(s_image_time,"yyyyMMdd_HHmm"),reply->request().url().toString());

                m_radar_products[rt]->addLegendImage(radar_image);
            }
        }

        // Remove the from the pending lists . .
        m_radar_products[rt]->m_legend_image_download_list.remove(
                    std::pair<QString, ReqType>(reply->request().url().toString(),
                                                ReqType::legend_image));
        m_legend_images_pending_replies[rt].removeOne(reply);

        //TODO set up an expiration of the above data
        //TODO set up a timer that triggers another download in 5 min

        reply->deleteLater();

        /*qDebug() << "Legend Images download status:"
                 << m_radar_products[rt]->m_legend_image_download_list.empty()
                 << m_legend_images_pending_replies[rt].empty();*/

        // TODO: Figure out mechanism for using x_images_download_done
        //  get rid of this flag otherwise.
        //  ... seems like it is a better idea to just display the images when one is available.
        //  ... without regard for the flag,
        //  ... as the display updates faster, but may be a good idea to somehow regulate the calls
        //  ... to draw radar, since this creates a stochotic animation

        if(m_legend_images_pending_replies[rt].empty()){
            m_radar_products[rt]->m_legend_images_download_done = true;
            if(m_radar_products[rt]->m_radar_images_download_done && m_radar_products[rt]->m_warning_images_download_done){
                emit(this->signalRadarProductDownloadDone(rt));
            }
        }

        return;
    }

    //////////////////////////////////////
    ///
    ///     warning_image
    ///
    //////////////////////////////////////

    case ReqType::warning_image:{

        // Get the RadarType. This is sent as an attribute to the reqest, and returned in the reply.
        QNetworkRequest::Attribute radar_type_attribute = static_cast<QNetworkRequest::Attribute>(1001);
        RadarType rt = static_cast<RadarType>(reply->request().attribute(radar_type_attribute).toInt());

        // Verify RadarType exists
        if(m_radar_products.count(rt) != 1){
            qDebug() << "Invalid m_radar_products key."
                     << "RadarType:" << rt
                     << __FILE__ << __LINE__;
            exit(-1);
        }

        if(!error){

            QPixmap qp;
            qp.loadFromData(reply->readAll());

            // Extract the time signature from the request url
            QRegularExpression re("_(\\d{8}_\\d{4})_");
            QRegularExpressionMatchIterator matches = re.globalMatch(reply->request().url().toString());
            QString s_image_time = "";
            if (matches.hasNext()) {
                    QRegularExpressionMatch match = matches.next();
                    //qDebug() << "REPLY Match" << match.captured(1);
                    s_image_time = match.captured(1);
                if(s_image_time == ""){
                    qDebug() << "s_image_time empty when it *REALLY* should not be."  << __FILE__ << __LINE__;
                    return;
                }
            }

            if( !qp.isNull() ){

                RadarImage radar_image(qp, QDateTime::fromString(s_image_time,"yyyyMMdd_HHmm"),reply->request().url().toString());

                m_radar_products[rt]->addWarningImage(radar_image);
            }
        }

        // Remove the from the pending lists . .
        m_radar_products[rt]->m_warning_image_download_list.remove(
                    std::pair<QString, ReqType>(reply->request().url().toString(),
                                                ReqType::warning_image));
        m_warning_images_pending_replies[rt].removeOne(reply);

        //TODO set up an expiration of the above data
        //TODO set up a timer that triggers another download in 5 min

        reply->deleteLater();

        /*qDebug() << "Warning Images download status:"
                 << m_radar_products[rt]->m_warning_image_download_list.empty()
                 << m_warning_images_pending_replies[rt].empty();*/

        // TODO: Figure out mechanism for using x_images_download_done
        //  get rid of this flag otherwise.
        //  ... seems like it is a better idea to just display the images when one is available.
        //  ... without regard for the flag,
        //  ... as the display updates faster, but may be a good idea to somehow regulate the calls
        //  ... to draw radar, since this creates a stochotic animation

        if(m_warning_images_pending_replies[rt].empty()){
            m_radar_products[rt]->m_warning_images_download_done = true;
            if(m_radar_products[rt]->m_radar_images_download_done && m_radar_products[rt]->m_legend_images_download_done){
                emit(this->signalRadarProductDownloadDone(rt));
            }
        }

        return;
    }

    //////////////////////////////////////
    ///
    ///     BASE IMAGES
    ///
    //////////////////////////////////////

    case ReqType::topo_s:{

        m_topo_short.m_image.loadFromData(reply->readAll());

        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::topo_s));

        reply->deleteLater();

        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::topo_l:{
        m_topo_long.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::topo_l));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::county_s:{
        m_county_short.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::county_s));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::county_l:{
        m_county_long.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::county_l));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::rivers_s:{
        m_rivers_short.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::rivers_s));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::rivers_l:{
        m_rivers_long.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::rivers_l));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::highways_s:{
        m_highways_short.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::highways_s));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::highways_l:{
        m_highways_long.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::highways_l));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::city_s:{
        m_city_short.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::city_s));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::city_l:{
        m_city_long.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::city_l));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::range_ring_s:{
        m_range_ring_short.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::range_ring_s));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }
    case ReqType::range_ring_l:{
        m_range_ring_long.m_image.loadFromData(reply->readAll());
        m_base_overlays_download_list.remove(std::pair<QString, ReqType>(reply->request().url().toString(), ReqType::range_ring_l));
        reply->deleteLater();
        if(m_base_overlays_download_list.empty()){
            m_base_image_download_done = true;
            emit signalBaseOverlaysDownloadDone();
        }
        return;
    }

    default:{
        qDebug() << "Default on switch." << __FILE__ << __LINE__;}
    }
    return;
}

WeatherStation::~WeatherStation(){

    delete(m_nam);

    m_radar_product_N0R->Destroy();
    m_radar_product_N0S->Destroy();
    m_radar_product_N0Z->Destroy();   //radar_N0Z is the long range radar product.
    m_radar_product_N0V->Destroy();
    m_radar_product_N1P->Destroy();
    m_radar_product_NCR->Destroy();
    m_radar_product_NTP->Destroy();

    m_current_radar_product = nullptr;
}
