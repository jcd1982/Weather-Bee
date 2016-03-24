#ifndef RADARPRODUCT_H
#define RADARPRODUCT_H

#include <QObject>
#include <map>
#include <QMap>
#include <QSet>

#include "radarimage.h"
#include "reqtype.h"

enum RadarType{
    /// The order of these is important, as the ui radio button group
    /// is built to return corresponding int's for the RadarType selected
    N0R = 0,
    N0Z = 1,
    N0S = 2,   //radar_N0Z is the long range radar product.
    N0V = 3,
    N1P = 4,
    NCR = 5,
    NTP = 6
};

class RadarProduct : public QObject
{
    Q_OBJECT

private:

    ~RadarProduct();

public:

    explicit RadarProduct(QString base_url, QString sid, RadarType radar_type, QString radar_name, QObject *parent = 0);

    void Destroy();

    RadarType m_radar_type; /// The radar type.

    QString m_radar_name;   /// The radar name.

    QString m_radar_image_file_list_url;        /// The url for the http page that contains
                                                /// the list of the radar image urls for the
                                                /// radar products instance.
    QString m_legend_image_file_list_url;       /// The url for the http page that contains
                                                /// the list of the legend image urls for the
                                                /// radar products instance.

    QString m_warning_image_short_file_list_url;    /// The url for the http page that contains
                                                    /// the list of the short warning image urls for the
                                                    /// radar products instance.
    QString m_warning_image_long_file_list_url;     /// The url for the http page that contains
                                                    /// the list of the legend image urls for the
                                                    /// radar products instance.

    QList<QString> m_common_legend;             /// List of QString keys to images which
                                                /// are common to the radar and legend image maps.
    QList<QString> m_common_warning;            /// List of QString keys to images which
                                                /// are common to the radar and warning image maps.
    QList<QString> m_common_legend_warning;     /// List of QString keys to images which
                                                /// are common to the radar, legend and warning image maps.

    QMap<QString, RadarImage> m_radar;      /// The Key Ordered Map of radar images.
    QMap<QString, RadarImage> m_legend;     /// The Key Ordered Map of legend images.
    QMap<QString, RadarImage> m_warning;    /// The Key Ordered Map of warning images.

    std::list<std::pair<QString, ReqType>> m_radar_image_download_list;
    std::list<std::pair<QString, ReqType>> m_legend_image_download_list;
    std::list<std::pair<QString, ReqType>> m_warning_image_download_list;

    bool m_radar_images_download_done;
    bool m_legend_images_download_done;
    bool m_warning_images_download_done;

    QTimer* m_refresh_timer;

    void addRadarImage(RadarImage r);
    void addLegendImage(RadarImage r);
    void addWarningImage(RadarImage r);

};

#endif // RADARPRODUCT_H
