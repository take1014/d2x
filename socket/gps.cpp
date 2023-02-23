#include <fstream>
#include <sstream>
#include <vector>
//#include "json/json.h"
#include <json/json.h>

#include "gps.hpp"
#include "nmea_struct.hpp"


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
parseGPGGA(const std::vector<std::string> &splitted_data, GPGGA_t &gpgga)
{
    if (splitted_data.size() != gpgga.data_num)
    {
        gpgga.init();
        return false;
    }

    gpgga.time.set(splitted_data.at(1));
    gpgga.latitude.set(splitted_data.at(2));
    gpgga.lat_direction                    = splitted_data.at(3);
    gpgga.longitude.set(splitted_data.at(4));
    gpgga.long_direction                   = splitted_data.at(5);
    gpgga.quality                          = convToIntFromDecimal(splitted_data.at(6));
    gpgga.satellites                       = convToIntFromDecimal(splitted_data.at(7));
    gpgga.horizontal_dilution_of_precision = convToFloat(splitted_data.at(8));
    gpgga.altitude                         = convToFloat(splitted_data.at(9));
    gpgga.altitude_units                   = splitted_data.at(10);
    gpgga.undulation                       = convToFloat(splitted_data.at(11));
    gpgga.undulation_units                 = splitted_data.at(12);
    gpgga.age                              = convToFloat(splitted_data.at(13));
    gpgga.differential_station_ID          = splitted_data.at(14);
    gpgga.checksum                         = convToIntFromHex(splitted_data.at(15));
    return true;
}

static bool
parseGPRMC(const std::vector<std::string> &splitted_data, GPRMC_t &gprmc)
{
    if(splitted_data.size() != gprmc.data_num)
    {
        gprmc.init();
        return false;
    }
    gprmc.time.set(splitted_data.at(1));
    gprmc.status                       = splitted_data.at(2);
    gprmc.latitude.set(splitted_data.at(3));
    gprmc.lat_direction                = splitted_data.at(4);
    gprmc.longitude.set(splitted_data.at(5));
    gprmc.long_direction               = splitted_data.at(6);
    gprmc.knots                        = convToFloat(splitted_data.at(7));
    gprmc.degrees                      = convToFloat(splitted_data.at(8));
    gprmc.date                         = splitted_data.at(9);
    gprmc.magnetic_variation           = convToFloat(splitted_data.at(10));
    gprmc.magnetic_variation_direction = splitted_data.at(11);
    gprmc.mode                         = splitted_data.at(12);
    gprmc.checksum                     = convToIntFromHex(splitted_data.at(13));
    return true;
}

GPS::GPS() : m_device(""),
             m_baudrate(Serial::eB9600),
             m_gps_serial(nullptr),
             m_gps_thread(nullptr){}

GPS::~GPS()
{
    stop();
}

bool
GPS::init(const std::string device, const Serial::BaudRate baudrate)
{
    m_device     = device;
    m_baudrate   = baudrate;
    m_gps_serial = new Serial(device, baudrate);

    if (!m_gps_serial->init())
    {
        std::cout << "GPS serial error." << std::endl;
        return false;
    }
    sleep(1);
    return true;
}

void
GPS::event_loop(void)
{
    while(true)
    {
        usleep(1000000.f);
        std::istringstream iss(m_gps_serial->receive('\n'));
        std::string line;

        std::ifstream json_fmt("./nmea_format.json", std::ifstream::binary);
        Json::Value gnss_data;
        Json::Reader reader;
        reader.parse(json_fmt, gnss_data);

        while (std::getline(iss, line, '\n'))
        {
            if (line.size() < 4 || line.front() != '$' || line.back() != '\r')
            {
                std::cout << "Invalid format" << std::endl;
                continue;
            }
            /* TODO: parser */
            /* GPGGA GPGLL GPGSA GPGSV GPRMC GPVTG PGZDA */
            std::vector<std::string> splitted_data = split(line.substr(1, line.size()-2));

            if (splitted_data.front() == "GPGGA")
            {
                GPGGA_t gpgga_data(16, splitted_data.front());
                if (parseGPGGA(splitted_data, gpgga_data))
                {
                    gnss_data["GPGGA"]["time_utc"]                         = gpgga_data.time.utc;
                    gnss_data["GPGGA"]["latitude"]                         = gpgga_data.latitude.decimal_degrees;
                    gnss_data["GPGGA"]["lat_direction"]                    = gpgga_data.lat_direction;
                    gnss_data["GPGGA"]["longitude"]                        = gpgga_data.longitude.decimal_degrees;
                    gnss_data["GPGGA"]["long_direction"]                   = gpgga_data.long_direction;
                    gnss_data["GPGGA"]["quality"]                          = gpgga_data.quality;
                    gnss_data["GPGGA"]["number_of_satellites"]             = gpgga_data.satellites;
                    gnss_data["GPGGA"]["horizontal_dilution_of_precision"] = gpgga_data.horizontal_dilution_of_precision;
                    gnss_data["GPGGA"]["altitude"]                         = gpgga_data.altitude;
                    gnss_data["GPGGA"]["altitude_units"]                   = gpgga_data.altitude_units;
                    gnss_data["GPGGA"]["undulation"]                       = gpgga_data.undulation;
                    gnss_data["GPGGA"]["undulation_units"]                 = gpgga_data.undulation_units;
                    gnss_data["GPGGA"]["age"]                              = gpgga_data.age;
                    gnss_data["GPGGA"]["differential_station_ID"]          = gpgga_data.differential_station_ID;
                    gnss_data["GPGGA"]["checksum"]                         = gpgga_data.checksum;
                    std::string gpgga_json =  "\"GPGGA\":{\"latitude\":" + gnss_data["GPGGA"]["latitude"].asString() + "," + "\"longitude\":" + gnss_data["GPGGA"]["longitude"].asString() + "}";
                    std::cout << gpgga_json << std::endl;
                }
            }

            if (splitted_data.front() == "GPRMC")
            {
                GPRMC_t gprmc_data(14, splitted_data.front());
                if (parseGPRMC(splitted_data, gprmc_data))
                {
                    gnss_data["GPRMC"]["time_utc"]                     = gprmc_data.time.utc;
                    gnss_data["GPRMC"]["status"]                       = gprmc_data.status;
                    gnss_data["GPRMC"]["latitude"]                     = gprmc_data.latitude.decimal_degrees;
                    gnss_data["GPRMC"]["lat_direction"]                = gprmc_data.lat_direction;
                    gnss_data["GPRMC"]["longitude"]                    = gprmc_data.longitude.decimal_degrees;
                    gnss_data["GPRMC"]["long_direction"]               = gprmc_data.long_direction;
                    gnss_data["GPRMC"]["knots"]                        = gprmc_data.knots;
                    gnss_data["GPRMC"]["degrees"]                      = gprmc_data.degrees;
                    gnss_data["GPRMC"]["date"]                         = gprmc_data.date;
                    gnss_data["GPRMC"]["magnetic_variation"]           = gprmc_data.magnetic_variation;
                    gnss_data["GPRMC"]["magnetic_variation_direction"] = gprmc_data.magnetic_variation_direction;
                    gnss_data["GPRMC"]["mode"]                         = gprmc_data.mode;
                    gnss_data["GPRMC"]["checksum"]                     = gprmc_data.checksum;
                    std::string gprmc_json =  "\"GPRMC\":{\"latitude\":" + gnss_data["GPRMC"]["latitude"].asString() + "," + "\"longitude\":" + gnss_data["GPRMC"]["longitude"].asString() + "}";
                    std::cout << gprmc_json << std::endl;
                }
            }
        }

        /* websocket */

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
    m_gps_serial = nullptr;
    m_gps_thread = nullptr;
}