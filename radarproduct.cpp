#include "radarproduct.h"

#include <QString>
#include <QDebug>

RadarProduct::RadarProduct(QString base_url, QString sid, RadarType radar_type, QString radar_name, QObject *parent)
    : QObject(parent),
      m_radar_type(radar_type),
      m_radar_name(radar_name),
      m_radar_image_file_list_url(base_url + "/RadarImg/" + radar_name + "/" + sid + "/"),
      m_legend_image_file_list_url(base_url + "/Legend/" + radar_name + "/" + sid + "/"),
      m_warning_image_short_file_list_url(base_url + "/Warnings/Short/" + sid + "/"),
      m_warning_image_long_file_list_url(base_url + "/Warnings/Long/" + sid + "/"),
      m_radar_images_download_done(false),
      m_legend_images_download_done(false),
      m_warning_images_download_done(false),
      m_refresh_timer(nullptr)
{

}

void RadarProduct::addRadarImage(RadarImage r){
    // This method adds the radar image to the m_radar container.
    // If the m_legend or m_warning map contain the same key it also inserts the string based key
    // (to the m_radar value) into the common lists.
    // If the radar images time stamp is already extant in m_radar, it does nothing.

    QString key = r.m_time.toString("yyyyMMdd_HHmm");

    if(this->m_radar.count(key) == 0){
        this->m_radar[key] = r;

        for(auto legend: this->m_legend.keys()){
            if(key == legend && !this->m_common_legend.contains(key)){
                this->m_common_legend.append(key);
                std::sort(this->m_common_legend.begin(), this->m_common_legend.end());
            }
        }
        for(auto warning: this->m_warning.keys()){
            if(key == warning && !this->m_common_warning.contains(key)){
                this->m_common_warning.append(key);
                std::sort(this->m_common_warning.begin(), this->m_common_warning.end());
            }

        }
        if(this->m_common_legend.contains(key) && this->m_common_warning.contains(key)
                && !this->m_common_legend_warning.contains(key)){
            this->m_common_legend_warning.append(key);
            std::sort(this->m_common_legend_warning.begin(), this->m_common_legend_warning.end());
        }
    }
}

void RadarProduct::addLegendImage(RadarImage r){
    // This method adds the legend image to the m_legend and container.
    // If the m_radar and m_warning map contain the same key it also inserts the string based key
    // (to the m_legend value) into the common lists.
    // If the legend images time stamp is already extant in m_legend, it does nothing.

    QString key = r.m_time.toString("yyyyMMdd_HHmm");

    if(this->m_legend.count(key) == 0){
        this->m_legend[key] = r;

        for(auto radar: this->m_radar.keys()){
            if(key == radar && !this->m_common_legend.contains(key)){
                this->m_common_legend.append(key);
                std::sort(this->m_common_legend.begin(), this->m_common_legend.end());

                for(auto warning: this->m_warning.keys()){
                    if(key == warning && !this->m_common_legend_warning.contains(key)){
                        this->m_common_legend_warning.append(key);
                        std::sort(this->m_common_legend_warning.begin(), this->m_common_legend_warning.end());
                    }
                }
            }
        }
    }
}

void RadarProduct::addWarningImage(RadarImage r){
    // This method adds the warning image to the m_warning container.
    // If the warning images time stamp is already extant in m_warning, it does nothing.

    QString key = r.m_time.toString("yyyyMMdd_HHmm");

    if(this->m_warning.count(key) == 0){
        this->m_warning[key] = r;

        for(auto radar: this->m_radar.keys()){
            if(key == radar && !this->m_common_warning.contains(key)){
                this->m_common_warning.append(key);
                std::sort(this->m_common_warning.begin(), this->m_common_warning.end());

                for(auto legend: this->m_legend.keys()){
                    if(key == legend && !this->m_common_legend_warning.contains(key)){
                        this->m_common_legend_warning.append(key);
                        std::sort(this->m_common_legend_warning.begin(), this->m_common_legend_warning.end());
                    }
                }
            }
        }
    }
}

void RadarProduct::Destroy(){
    this->~RadarProduct();
}

RadarProduct::~RadarProduct(){
    // TODO: Add auto refresh (use the already set aside timer m_refresh_timer)
    //if(m_refresh_timer != nullptr) delete m_refresh_timer;
}
