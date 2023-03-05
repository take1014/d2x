#ifndef GPS_HPP
#define GPS_HPP

#include <iostream>
#include <unistd.h>
#include <thread>
#include <mqtt/client.h>

#include "serial.hpp"

class GPS{
public:
    struct GpsConf_t{
        GpsConf_t () : device("/dev/ttyACM0"),
                       baudrate(9600),
                       mqtt_local_ip("localhost:1883"),
                       mqtt_local_id("publisher"),
                       mqtt_server_ip("192.168.3.32:1883"),
                       mqtt_server_id("publisher"),
                       mqtt_pub_key("gps/ucsk"),
                       json_fmt_path("../nmea_format.json") {}

        std::string device;
        int baudrate;
        std::string mqtt_local_ip;
        std::string mqtt_local_id;
        std::string mqtt_server_ip;
        std::string mqtt_server_id;
        std::string mqtt_pub_key;
        std::string json_fmt_path;
    };

    GPS();
    ~GPS();
    bool init(GpsConf_t &gps_conf);
    void start();
    void stop();

private:
    void event_loop();

    std::string   m_json_fmt_path;
    std::string   m_mqtt_pub_key;
    Serial       *m_gps_serial;
    std::thread  *m_gps_thread;
    mqtt::client *m_mqtt_local;
    mqtt::client *m_mqtt_server;
};

/* inline function */
inline float convToFloat(const std::string &data){ return (data.empty()) ? 0.f : std::stof(data); }
inline int   convToIntFromDecimal(const std::string &data){ return (data.empty()) ? 0 : std::stoi(data, nullptr, 10); }
inline int   convToIntFromHex(const std::string &data){ return (data.empty()) ? 0 : std::stoi(data, nullptr, 16); }

inline std::string calcUTC(const std::string &value)
{
    if (value.empty()) return "xx:xx:xx";
    const std::string str_hour = value.substr(0, 2);
    const std::string str_min  = value.substr(2, 2);
    const std::string str_sec  = value.substr(4);
    return str_hour + ":" + str_min + ":" + str_sec;
}

inline float calcDecimalDegrees(const std::string &value)
{
    constexpr float DISABLE_DECIMAL_DEGREES = 999;

    if (value.empty()) return DISABLE_DECIMAL_DEGREES;
    /* set degrees */
    std::string str_deg = value.substr(0, value.find(".")-2);
    float degrees = convToFloat(str_deg);
    /* set minuntes */
    std::string str_min = value.substr(value.find(".")-2);
    float minutes = convToFloat(str_min);
    /* set decimal_degrees */
    return (degrees >= 0.f) ? degrees + minutes / 60.f : degrees - minutes / 60.f;
}

#endif /* GPS_HPP */