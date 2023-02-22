#include <fstream>
#include <sstream>

#include "gps.hpp"

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
    m_device = device;
    m_baudrate = baudrate;
    m_gps_serial = new Serial(device, baudrate);

    if (!m_gps_serial->init())
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
        usleep(1000000.f);
        std::istringstream iss(m_gps_serial->receive('\n'));
        std::string line;
        while (std::getline(iss, line, '\n'))
        {
            /* TODO: parser */
        }
    }
}

void
GPS::start(void)
{
    /* Start GPS thread */
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