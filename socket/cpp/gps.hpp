#include <iostream>
#include <unistd.h>
#include <thread>
#include <mqtt/client.h>

#include "serial.hpp"

#ifndef GPS_HPP
#define GPS_HPP

class GPS{
    public:
        GPS();
        ~GPS();
        bool init(const std::string device, const Serial::BaudRate baudrate,
                  const std::string mqtt_ip="mqtt://localhost:1883", const std::string mqtt_id="publisher");
        void start(void);
        void stop(void);
    private:
        void event_loop(void);

        std::string m_device;
        std::string m_mqtt_ip;
        std::string m_mqtt_id;
        Serial::BaudRate m_baudrate;
        Serial *m_gps_serial;
        std::thread  *m_gps_thread;
        mqtt::client *m_mqtt_client;
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
