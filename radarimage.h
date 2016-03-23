#ifndef RADARIMAGE_H
#define RADARIMAGE_H

#include <QPixmap>
#include <QDateTime>

class RadarImage
{
    /// This class contains the NOAA image resources that define a radar image.
    /// It is used for all radar components i.e. radar data, overlays, and legends.

private:

public:
    RadarImage();
    RadarImage(QPixmap i, QDateTime t, QString u);

    QPixmap *getImage();
    void setImage(QPixmap);


    QPixmap m_image;
    QDateTime m_time;
    QString m_url;

};

#endif // RADARIMAGE_H
