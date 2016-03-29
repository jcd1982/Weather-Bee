#ifndef WEATHERSTATION_H
#define WEATHERSTATION_H

#include <QObject>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QDebug>
#include <QPainter>
#include <QTimer>

#include <list>
#include <set>

#include "radarproduct.h"
#include "radarimage.h"

struct RadarSettings{
    bool    topoEnabled,
            cityEnabled,
            countiesEnabled,
            highwaysEnabled,
            legendEnabled,
            rangeEnabled,
            riversEnabled,
            warningsEnabled;
    qreal   radarOpacity;
    uint16_t radarRefreshInterval;
};

class WeatherStation : public QObject
{
    Q_OBJECT

private slots:
    void slotHandleNetworkReplies(QNetworkReply* reply);
    void slotRefreshRadarProduct();

signals:
    void signalBaseOverlaysDownloadDone();
    void signalRadarProductDownloadDone(RadarType);

private:
    bool m_base_image_download_done;
    QMap<RadarType, bool> m_radar_product_download_done;

    QList<QString> getCurrentKeys();

    void renderRadar(QString key, QPixmap* base);

    RadarSettings m_overlay_settings;

    QNetworkAccessManager *m_nam;

    std::list<std::pair<QString, ReqType>> m_base_overlays_download_list;

    QList<QNetworkReply*> m_base_map_imgs_pend_replies;

    std::list<QVariant> m_base_map_overlays_remaining_requests;

    int curr_img_idx;

    const QString m_station_id;     /// The NOAA defined Radar Station ID.

    const QString m_base_url;

    QString m_current_image_key;

    RadarProduct* m_radar_product_N0R;
    RadarProduct* m_radar_product_N0S;
    RadarProduct* m_radar_product_N0Z;   //radar_N0Z is the long range radar product.
    RadarProduct* m_radar_product_N0V;
    RadarProduct* m_radar_product_N1P;
    RadarProduct* m_radar_product_NCR;
    RadarProduct* m_radar_product_NTP;

    QMap<RadarType, RadarProduct*> m_radar_products;

    /// This list contains the requests for the urls each of which contain the list
    /// of urls that incorporate the dynamic data such as radar products,
    /// radar legends and weather warnings.
    std::list<std::pair<QString, ReqType>> m_dynamic_images_location_list_download_list;

    // TODO: delete the remaining pending QNetworkReply pointers in destructor,
    //  I think they may get away in the case that radar station is changed
    //  b4 the "delete later"?

    QMap<RadarType, QList<QNetworkReply*>> m_radar_images_pending_replies;
    QMap<RadarType, QList<QNetworkReply*>> m_legend_images_pending_replies;
    QMap<RadarType, QList<QNetworkReply*>> m_warning_images_pending_replies;

    /* The remainder of the images are informational overlays
     * which are not dynamic, they vary only by Short or Long range.
     */
    RadarImage m_topo_short;
    RadarImage m_topo_long;
    RadarImage m_county_short;
    RadarImage m_county_long;
    RadarImage m_rivers_short;
    RadarImage m_rivers_long;
    RadarImage m_highways_short;
    RadarImage m_highways_long;
    RadarImage m_city_short;
    RadarImage m_city_long;
    RadarImage m_range_ring_short;
    RadarImage m_range_ring_long;

    RadarProduct* m_current_radar_product;

    QTimer m_refresh_timer;

public:

    explicit WeatherStation(QString sid, RadarSettings o, QObject *parent = 0);
    ~WeatherStation();

    void setRadarSettings(RadarSettings s);
    bool setRadarProduct(RadarType rt);
    RadarType getCurrentRadarProductType();

    QPixmap* getLatestImage();
    QPixmap* getNextImage();
    QPixmap* getCurrentImage();
    QPixmap* getBaseImage();

    QString getStationId();

    void makeRequest(const std::list<std::pair<QString, ReqType>> &reqs);
    void makeRequest(const std::list<std::pair<QString, ReqType>> &reqs, RadarType rt);

};

#endif // WEATHERSTATION_H
