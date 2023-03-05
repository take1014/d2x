#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <filesystem>
#include <json/json.h>

#include "gps.hpp"

constexpr int DATA_SZ_GPZDA =  8;
constexpr int DATA_SZ_GPGLL =  9;
constexpr int DATA_SZ_GPVTG = 11;
constexpr int DATA_SZ_GPRMC = 14;
constexpr int DATA_SZ_GPGGA = 16;
constexpr int DATA_SZ_GPGSA = 19;
constexpr int DATA_SZ_GPGSV = 21;

static std::vector<std::string>
split(const std::string &data)
{
    std::vector<std::string> splitted_data;
    std::stringstream ss(data);
    std::string buffer;
    while(std::getline(ss, buffer, ','))
    {
        splitted_data.push_back(buffer);
    }

    // check sum
    ss.str("");
    ss.clear(std::stringstream::goodbit);
    ss << splitted_data.back();
    /* pop last content */
    splitted_data.pop_back();
    while (std::getline(ss, buffer, '*'))
    {
        splitted_data.push_back(buffer);
    }
    return splitted_data;
}

static bool
parseGPGGA(const std::vector<std::string> &splitted_data, Json::Value &gpgga)
{
    if (splitted_data.size() != DATA_SZ_GPGGA) { return false; }
    gpgga["time_utc"]                         = calcUTC(splitted_data.at(1));
    gpgga["latitude"]                         = calcDecimalDegrees(splitted_data.at(2));
    gpgga["lat_direction"]                    = splitted_data.at(3);
    gpgga["longitude"]                        = calcDecimalDegrees(splitted_data.at(4));
    gpgga["long_direction"]                   = splitted_data.at(5);
    gpgga["quality"]                          = convToIntFromDecimal(splitted_data.at(6));
    gpgga["number_of_satellites"]             = convToIntFromDecimal(splitted_data.at(7));
    gpgga["horizontal_dilution_of_precision"] = convToFloat(splitted_data.at(8));
    gpgga["altitude"]                         = convToFloat(splitted_data.at(9));
    gpgga["altitude_units"]                   = splitted_data.at(10);
    gpgga["undulation"]                       = convToFloat(splitted_data.at(11));
    gpgga["undulation_units"]                 = splitted_data.at(12);
    gpgga["age"]                              = convToFloat(splitted_data.at(13));
    gpgga["differential_station_ID"]          = splitted_data.at(14);
    gpgga["checksum"]                         = convToIntFromHex(splitted_data.at(15));
    return true;
}

static bool
parseGPRMC(const std::vector<std::string> &splitted_data, Json::Value &gprmc)
{
    if(splitted_data.size() != DATA_SZ_GPRMC){ return false; }
    gprmc["time_utc"]                     = calcUTC(splitted_data.at(1));
    gprmc["status"]                       = splitted_data.at(2);
    gprmc["latitude"]                     = calcDecimalDegrees(splitted_data.at(3));
    gprmc["lat_direction"]                = splitted_data.at(4);
    gprmc["longitude"]                    = calcDecimalDegrees(splitted_data.at(5));
    gprmc["long_direction"]               = splitted_data.at(6);
    gprmc["knots"]                        = convToFloat(splitted_data.at(7));
    gprmc["degrees"]                      = convToFloat(splitted_data.at(8));
    gprmc["date"]                         = splitted_data.at(9);
    gprmc["magnetic_variation"]           = convToFloat(splitted_data.at(10));
    gprmc["magnetic_variation_direction"] = splitted_data.at(11);
    gprmc["mode"]                         = splitted_data.at(12);
    gprmc["checksum"]                     = convToIntFromHex(splitted_data.at(13));
    return true;
}

static bool
parseGPGLL(const std::vector<std::string> &splitted_data, Json::Value &gpgll)
{
    if(splitted_data.size() != DATA_SZ_GPGLL){ return false; }
    gpgll["latitude"]       = calcDecimalDegrees(splitted_data.at(1));
    gpgll["lat_direction"]  = splitted_data.at(2);
    gpgll["longitude"]      = calcDecimalDegrees(splitted_data.at(3));
    gpgll["long_direction"] = splitted_data.at(4);
    gpgll["time_utc"]       = calcUTC(splitted_data.at(5));
    gpgll["status"]         = splitted_data.at(6);
    gpgll["mode"]           = splitted_data.at(7);
    gpgll["checksum"]       = convToIntFromHex(splitted_data.at(8));
    return true;
}

static bool
parseGPGSA(const std::vector<std::string> &splitted_data, Json::Value &gpgsa)
{
    if(splitted_data.size() != DATA_SZ_GPGSA){ return false; }
    gpgsa["mode"]            = splitted_data.at(1);
    gpgsa["status"]          = convToIntFromDecimal(splitted_data.at(2));
    gpgsa["satellite_id_1"]  = convToIntFromDecimal(splitted_data.at(3));
    gpgsa["satellite_id_2"]  = convToIntFromDecimal(splitted_data.at(4));
    gpgsa["satellite_id_3"]  = convToIntFromDecimal(splitted_data.at(5));
    gpgsa["satellite_id_4"]  = convToIntFromDecimal(splitted_data.at(6));
    gpgsa["satellite_id_5"]  = convToIntFromDecimal(splitted_data.at(7));
    gpgsa["satellite_id_6"]  = convToIntFromDecimal(splitted_data.at(8));
    gpgsa["satellite_id_7"]  = convToIntFromDecimal(splitted_data.at(9));
    gpgsa["satellite_id_8"]  = convToIntFromDecimal(splitted_data.at(10));
    gpgsa["satellite_id_9"]  = convToIntFromDecimal(splitted_data.at(11));
    gpgsa["satellite_id_10"] = convToIntFromDecimal(splitted_data.at(12));
    gpgsa["satellite_id_11"] = convToIntFromDecimal(splitted_data.at(13));
    gpgsa["satellite_id_12"] = convToIntFromDecimal(splitted_data.at(14));
    gpgsa["pdop"]            = convToFloat(splitted_data.at(15));
    gpgsa["hdop"]            = convToFloat(splitted_data.at(16));
    gpgsa["vdop"]            = convToFloat(splitted_data.at(17));
    gpgsa["checksum"]        = convToIntFromHex(splitted_data.at(18));
    return true;
}

static bool
parseGPGSV(const std::vector<std::string> &splitted_data, Json::Value &gpgsv)
{
    if(splitted_data.size() != DATA_SZ_GPGSV){ return false; }
    gpgsv["total_message_num"]      = convToIntFromDecimal(splitted_data.at(1));
    gpgsv["message_number"]         = convToIntFromDecimal(splitted_data.at(2));
    gpgsv["total_satellite_number"] = convToFloat(splitted_data.at(3));
    gpgsv["satellite_number_1"]     = convToIntFromDecimal(splitted_data.at(4));
    gpgsv["elevation_1"]            = convToFloat(splitted_data.at(5));
    gpgsv["azimuth_1"]              = convToFloat(splitted_data.at(6));
    gpgsv["sn_1"]                   = convToFloat(splitted_data.at(7));
    gpgsv["satellite_number_2"]     = convToIntFromDecimal(splitted_data.at(8));
    gpgsv["elevation_2"]            = convToFloat(splitted_data.at(9));
    gpgsv["azimuth_2"]              = convToFloat(splitted_data.at(10));
    gpgsv["sn_2"]                   = convToFloat(splitted_data.at(11));
    gpgsv["satellite_number_3"]     = convToIntFromDecimal(splitted_data.at(12));
    gpgsv["elevation_3"]            = convToFloat(splitted_data.at(13));
    gpgsv["azimuth_3"]              = convToFloat(splitted_data.at(14));
    gpgsv["sn_3"]                   = convToFloat(splitted_data.at(15));
    gpgsv["satellite_number_4"]     = convToIntFromDecimal(splitted_data.at(16));
    gpgsv["elevation_4"]            = convToFloat(splitted_data.at(17));
    gpgsv["azimuth_4"]              = convToFloat(splitted_data.at(18));
    gpgsv["sn_4"]                   = convToFloat(splitted_data.at(19));
    gpgsv["checksum"]               = convToIntFromHex(splitted_data.at(20));
    return true;
}

static bool
parseGPVTG(const std::vector<std::string> &splitted_data, Json::Value &gpvtg)
{
    if(splitted_data.size() != DATA_SZ_GPVTG){ return false; }
    gpvtg["track"]               = convToFloat(splitted_data.at(1));
    gpvtg["track_mode"]          = splitted_data.at(2);
    gpvtg["magnetic_track"]      = convToFloat(splitted_data.at(3));
    gpvtg["magnetic_track_mode"] = splitted_data.at(4);
    gpvtg["ground_speed_knots"]  = convToFloat(splitted_data.at(5));
    gpvtg["knots"]               = splitted_data.at(6);
    gpvtg["ground_speed_kmh"]    = convToFloat(splitted_data.at(7));
    gpvtg["kmh"]                 = splitted_data.at(8);
    gpvtg["mode"]                = splitted_data.at(9);
    gpvtg["checksum"]            = convToIntFromHex(splitted_data.at(10));
    return true;
}

static bool
parseGPZDA(const std::vector<std::string> &splitted_data, Json::Value &gpzda)
{
    if(splitted_data.size() != DATA_SZ_GPZDA){ return false; }
    gpzda["time_utc"]                       = calcUTC(splitted_data.at(1));
    gpzda["day"]                            = convToIntFromDecimal(splitted_data.at(2));
    gpzda["month"]                          = convToIntFromDecimal(splitted_data.at(3));
    gpzda["year"]                           = convToIntFromDecimal(splitted_data.at(4));
    gpzda["local_zone_hours_description"]   = convToIntFromDecimal(splitted_data.at(5));
    gpzda["local_zone_minutes_description"] = convToIntFromDecimal(splitted_data.at(6));
    gpzda["checksum"]                       = convToIntFromHex(splitted_data.at(7));
    return true;
}


GPS::GPS() : m_json_fmt_path("../nmea_format.json"),
             m_mqtt_pub_key("gps/mykey"),
             m_gps_serial(nullptr),
             m_gps_thread(nullptr),
             m_mqtt_local(nullptr),
             m_mqtt_server(nullptr){}

GPS::~GPS()
{
    stop();
}

bool
GPS::init(GpsConf_t &gps_conf)
{
    m_gps_serial = new Serial(gps_conf.device, gps_conf.baudrate);
    if (!m_gps_serial->init())
    {
        std::cout << "GPS: Serial error." << std::endl;
        return false;
    }
    sleep(1);

    /* mqtt */
    m_mqtt_local = new mqtt::client(gps_conf.mqtt_local_ip, gps_conf.mqtt_local_id, mqtt::create_options(MQTTVERSION_3_1));
    m_mqtt_local->connect();
    if (!m_mqtt_local->is_connected())
    {
        std::cout << "GPS: MQTT local connect error." << std::endl;
        return false;
    }

    m_mqtt_server = new mqtt::client(gps_conf.mqtt_server_ip, gps_conf.mqtt_server_id, mqtt::create_options(MQTTVERSION_3_1));
    m_mqtt_server->connect();
    if (!m_mqtt_server->is_connected())
    {
        std::cout << "GPS: MQTT server connect error." << std::endl;
        return false;
    }

    if (!std::filesystem::exists(gps_conf.json_fmt_path))
    {
        std::cout << "GPS: Json format path is not exists." << std::endl;
        return false;
    }
    m_json_fmt_path = gps_conf.json_fmt_path;

    if (gps_conf.mqtt_pub_key.find("gps") == std::string::npos)
    {
        std::cout << "GPS: PUB_KEY must start with \"gps/\"."  << std::endl;
        return false;
    }
    m_mqtt_pub_key  = gps_conf.mqtt_pub_key;

    return true;
}

void
GPS::event_loop(void)
{
    Json::Value gnss_data;
    Json::FastWriter fastWriter;
    Json::Reader reader;
    std::ifstream json_fmt(m_json_fmt_path);
    mqtt::message_ptr mqtt_msgptr = mqtt::make_message(m_mqtt_pub_key, "");

    while(true)
    {
        /* 100m sec/f */
        usleep(1000000.f);
        std::istringstream iss(m_gps_serial->recvMsg(true, '\n'));
        std::string line;

        /* Create Json object from json file */
        reader.parse(json_fmt, gnss_data);

        while (std::getline(iss, line, '\n'))
        {
            if (line.size() < 4 || line.front() != '$' || line.back() != '\r')
            {
                std::cout << "Invalid format" << std::endl;
                continue;
            }

            std::vector<std::string> splitted_data = split(line.substr(1, line.size()-2));

            /* Get NMEA type */
            std::string nmea_type = splitted_data.front();
            /* Parse */
            if (nmea_type == "GPGGA")
            {
                parseGPGGA(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else if (nmea_type == "GPRMC")
            {
                parseGPRMC(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else if (nmea_type == "GPGLL")
            {
                parseGPGLL(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else if (nmea_type == "GPGSA")
            {
                parseGPGSA(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else if (nmea_type == "GPGSV")
            {
                parseGPGSV(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else if (nmea_type == "GPVTG")
            {
                parseGPVTG(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else if(nmea_type== "GPZDA")
            {
                parseGPZDA(splitted_data, gnss_data["RAW_GNSS"][nmea_type]);
            }
            else
            {
                continue;
            }
        }

        /* TODO: Correct GNSS  */
        gnss_data["GPS_OUTPUT"]["key"] = "takehara";
        gnss_data["GPS_OUTPUT"]["latitude"]  = gnss_data["RAW_GNSS"]["GPRMC"]["latitude"];
        gnss_data["GPS_OUTPUT"]["longitude"] = gnss_data["RAW_GNSS"]["GPRMC"]["longitude"];

        /* mqtt websocket */
        /* create send message*/
        std::string send_msg = fastWriter.write(gnss_data);
        std::cout << send_msg << std::endl;
        /* Send message to listeners */
        mqtt_msgptr->set_payload(send_msg);
        m_mqtt_local->publish(mqtt_msgptr);
        m_mqtt_server->publish(mqtt_msgptr);
    }
}

void
GPS::start(void)
{
    /* Start GPS thread */
    std::cout << "GPS thread start." << std::endl;
    m_gps_thread = new std::thread(&GPS::event_loop, this);
}

void
GPS::stop(void)
{
    m_gps_thread->join();
    delete m_gps_serial;
    delete m_gps_thread;
    delete m_mqtt_local;
    delete m_mqtt_server;
    m_gps_serial  = nullptr;
    m_gps_thread  = nullptr;
    m_mqtt_local  = nullptr;
    m_mqtt_server = nullptr;
}