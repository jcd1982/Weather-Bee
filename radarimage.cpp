#include "radarimage.h"
#include <QDebug>

RadarImage::RadarImage(QPixmap i, QDateTime t, QString u)
    : m_image{i}, m_time{t}, m_url{u}
{

}
RadarImage::RadarImage()
{

}

QPixmap* RadarImage::getImage(){
    return &this->m_image;
}

void RadarImage::setImage(QPixmap image){
    m_image = image;
}
