#ifndef REQTYPE
#define REQTYPE

enum ReqType{
    base_image,
    topo_s,
    topo_l,
    county_s,
    county_l,
    rivers_s,
    rivers_l,
    highways_s,
    highways_l,
    city_s,
    city_l,
    range_ring_s,
    range_ring_l,
    radar_image_file_list,
    legend_image_file_list,
    warning_image_file_list,
    radar_image,
    legend_image,
    warning_image,
    NOAA_DWML_SCHEMA = 0,
    NOAA_CURRENT_CONDITIONS = 1,
    NOAA_LAT_LON_FROM_ZIP = 2,
    NOAA_JSON_FCAST_CURRCOND = 3,
    NOAA_ICON = 4
};

#endif // REQTYPE

