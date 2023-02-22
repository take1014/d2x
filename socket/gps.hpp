#include <iostream>
#include <unistd.h>
#include <thread>

#include "serial.hpp"

#ifndef GPS_HPP
class GPS{
    public:
        GPS();
        ~GPS();
        bool init(const std::string device, const Serial::BaudRate baudrate);
        void start(void);
        void stop(void);
    private:
        void event_loop(void);

        std::string m_device;
        Serial::BaudRate m_baudrate;
        Serial m_gps_serial;
        std::thread *m_gps_thread;
};

#define GPS_HPP
#endif /* GPS_HPP */