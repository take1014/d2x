#include <fstream>
#include <sstream>
#include <vector>
#include <json/json.h>

#include "gps.hpp"

constexpr int DATA_SZ_GPGGA = 16;
constexpr int DATA_SZ_GPRMC = 14;

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
                if (parseGPGGA(splitted_data, gnss_data["GPGGA"]))
                {
                    std::string gpgga_json =  "\"GPGGA\":{\"latitude\":" + gnss_data["GPGGA"]["latitude"].asString() + "," + "\"longitude\":" + gnss_data["GPGGA"]["longitude"].asString() + "}";
                    std::cout << gpgga_json << std::endl;
                }
            }

            if (splitted_data.front() == "GPRMC")
            {
                if (parseGPRMC(splitted_data, gnss_data["GPRMC"]))
                {
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