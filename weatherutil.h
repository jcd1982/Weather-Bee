#ifndef WEATHERUTIL
#define WEATHERUTIL

// unit conversion numbers

const double kts_to_mph = 0.8689762;    // knots to miles per hour.
const double mph_to_kts = 1.1507794;    // miles per hour to knots.

const double mps_to_mph = 0.44704;      // meters per second to miles per hour.
const double mph_to_mps = 2.23694;      // miles per hour to meters per second.

const double kph_to_mph = 1.609344;    // kilometers per hour to miles per hour.
const double mph_to_kph = 0.621371;    // miles per hour to kilometers per hour.

const double ftps_to_mph = 1.46667;     // feet per second to miles per hour.
const double mph_to_ftps = 0.681818;    // miles per hour to feet per second.

const double km_to_mi = 1.609344;       // kilometers to miles.
const double mi_to_km = 0.621371;       // miles to kilometers.

const double inhg_to_mb = 33.8639;      // inHg to millibars.
const double mb_to_inhg = 0.02953;      // millibars to inHg.

const double inhg_to_atm = 0.033421;    // inHg to standard atmosphere.
const double atm_to_inhg = 29.9213;     // standard atmosphere to inHg.

inline double celciusToFarenheit(const double celcius){
    return (celcius * (9.0f/5.0f)) + 32.0f;
}

inline double farenheitToCelcius(const double farenheit){
    return (farenheit - 32.0f) * (5.0f/9.0f);
}

#endif
