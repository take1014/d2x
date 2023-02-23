#ifndef NMEA_STRUCT_H
#define NMEA_STRUCT_H
#include <iostream>

inline float convToFloat(const std::string &data){ return (data.empty()) ? 0.f : std::stof(data); }
inline int   convToIntFromDecimal(const std::string &data){ return (data.empty()) ? 0   : std::stoi(data, nullptr, 10); }
inline int   convToIntFromHex(const std::string &data){ return (data.empty()) ? 0   : std::stoi(data, nullptr, 16); }

struct Time {
    Time() : hours(0.f), minutes(0.f), seconds(0.f), utc(""){}
    void init()
    {
        hours   = 0.f;
        minutes = 0.f;
        seconds = 0.f;
        utc     = "";
    }

    void set(const std::string &value)
    {
        const std::string str_hour = value.substr(0, 2);
        const std::string str_min  = value.substr(2, 2);
        const std::string str_sec  = value.substr(4);
        hours   = convToFloat(str_hour);
        minutes = convToFloat(str_min);
        seconds = convToFloat(str_sec);
        utc = str_hour + ":" + str_min + ":" + str_sec;
    }

    float hours;
    float minutes;
    float seconds;
    std::string utc;
};
struct LatLong_t{
    LatLong_t() : degrees(0.f), minutes(0.f), decimal_degrees(0.f){}

    void init()
    {
        degrees = 0.f;
        minutes = 0.f;
        decimal_degrees = 0.f;
    }

    void set(const std::string &value)
    {
        /* set degrees */
        std::string str_deg = value.substr(0, value.find(".")-2);
        degrees = convToFloat(str_deg);
        /* set minuntes */
        std::string str_min = value.substr(value.find(".")-2);
        minutes = convToFloat(str_min);
        /* set decimal_degrees */
        decimal_degrees = (degrees >= 0.f) ? degrees + minutes / 60.f:
                                             degrees - minutes / 60.f;
    }

    float degrees;
    float minutes;
    float decimal_degrees;
};

struct GPGGA_t{

    GPGGA_t() : data_num(16), type("GPGGA") { init(); }
    GPGGA_t(int data_num, std::string type) : data_num(data_num), type(type){init();}

    void init()
    {
        time.init();
        latitude.init();
        longitude.init();
        lat_direction  = "";
        long_direction = "";
        quality = 0;
        satellites = 0;
        horizontal_dilution_of_precision = 0.f;
        altitude = 0.f;
        undulation = 0.f;
        altitude_units = "";
        undulation_units = "";
        age = 0.f;
        differential_station_ID = "";
        checksum = 0;
    }

    const int data_num;
    const std::string type;

    int quality;
    int satellites;
    int checksum;
    float horizontal_dilution_of_precision;
    float altitude;
    float undulation;
    float age;
    Time time;
    LatLong_t latitude;
    LatLong_t longitude;
    std::string lat_direction;
    std::string long_direction;
    std::string altitude_units;
    std::string undulation_units;
    std::string differential_station_ID;
};

struct GPRMC_t{

    GPRMC_t() : data_num(14), type("GPRMC") { init(); }
    GPRMC_t(int data_num, std::string type) : data_num(data_num), type(type){ init(); }

    void init()
    {
        time.init();
        latitude.init();
        longitude.init();
        lat_direction  = "";
        long_direction = "";
        knots = 0.f;
        degrees = 0.f;
        date = "";
        magnetic_variation = 0.f;
        magnetic_variation_direction = "";
        mode = "";
        checksum = 0;
    }

    const int data_num;
    const std::string type;

    Time time;
    std::string status;
    LatLong_t latitude;
    LatLong_t longitude;
    std::string lat_direction;
    std::string long_direction;
    float knots;
    float degrees;
    std::string date;
    float magnetic_variation;
    std::string magnetic_variation_direction;
    std::string mode;
    int checksum;
};

#endif /* NMEA_STRUCT_H */