#include <fstream>
#include <sstream>
#include <vector>

#include "gps.hpp"

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
        hours   = (str_hour.empty()) ? 0.f : std::stof(str_hour);
        minutes = (str_min.empty())  ? 0.f : std::stof(str_min);
        seconds = (str_sec.empty())  ? 0.f : std::stof(str_sec);
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
        std::string str_deg = value.substr(0, value.find("."-2));
        degrees = (str_deg.empty()) ? 0.f : std::stof(str_deg);
        /* set minuntes */
        std::string str_min = value.substr(value.find("."-2));
        minutes = (str_min.empty()) ? 0.f : std::stof(str_min);
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

static bool
split(const std::string &data, std::vector<std::string> &splitted_data)
{
    if (data.size() < 4 || data.front() != '$' || data.back() != 'r')
    {
        return false;
    }

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
    return true;
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
    gpgga.lat_direction = splitted_data.at(3);
    gpgga.longitude.set(splitted_data.at(4));
    gpgga.long_direction = splitted_data.at(5);
    gpgga.quality    = (splitted_data.at(6).empty()) ? 0.f : std::stoi(splitted_data.at(6), nullptr, 10);
    gpgga.satellites = (splitted_data.at(7).empty()) ? 0.f : std::stoi(splitted_data.at(7), nullptr, 10);
    gpgga.horizontal_dilution_of_precision = (splitted_data.at(8).empty()) ? 0.f : std::stof(splitted_data.at(8));
    gpgga.altitude = (splitted_data.at(9).empty()) ? 0.f : std::stof(splitted_data.at(9));
    gpgga.altitude_units = splitted_data.at(10);
    gpgga.undulation = (splitted_data.at(11).empty()) ? 0.f : std::stof(splitted_data.at(11));
    gpgga.undulation_units = splitted_data.at(12);
    gpgga.age = (splitted_data.at(13).empty()) ? 0.f : std::stof(splitted_data.at(13));
    gpgga.differential_station_ID = splitted_data.at(14);
    gpgga.checksum = (splitted_data.at(15).empty()) ? 0.f : std::stoi(splitted_data.at(15), nullptr, 16);
    return true;
}

GPS::GPS() : m_device(""),
             m_baudrate(Serial::eB9600),
             // m_gps_serial(nullptr),
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
    m_gps_serial = Serial(device, baudrate);

    if (!m_gps_serial.init())
    {
        std::cout << "GPS serial error." << std::endl;
        return false;
    }
    return true;
    sleep(1);
}

void
GPS::event_loop(void)
{
    while(true)
    {
        // usleep(1000000.f);
        std::istringstream iss(m_gps_serial.receive('\n'));
        std::string line;
        while (std::getline(iss, line, '\n'))
        {
            /* TODO: parser */
            /* GPGGA GPGLL GPGSA GPGSV GPRMC GPVTG PGZDA */
            std::vector<std::string> splitted_data;
            if (!split(line.substr(1, line.size()-2), splitted_data)) continue;

            if (splitted_data.front() == "GPGGA")
            {
                GPGGA_t gpgga_data(16, splitted_data.front());
                if (parseGPGGA(splitted_data, gpgga_data))
                {
                    std::cout << gpgga_data.latitude.degrees << std::endl;
                    std::cout << gpgga_data.latitude.minutes << std::endl;
                    std::cout << gpgga_data.latitude.decimal_degrees << std::endl;
                }
            }
        }
    }
}

void
GPS::start(void)
{
    /* Start GPS thread */
    std::cout << "GPS thread start." << std::endl;
    event_loop();
    // m_gps_thread = new std::thread(&GPS::event_loop, this);
}

void
GPS::stop(void)
{
    // m_gps_thread->join();
    // delete m_gps_serial;
    delete m_gps_thread;
    // m_gps_serial = nullptr;
    m_gps_thread = nullptr;
}