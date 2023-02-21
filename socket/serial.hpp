#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <termios.h>

class Serial
{
    public:
        enum BaudRate{
            eB4800  = B4800,
            eB9600  = B9600,
            eB19200 = B19200,
            eB38400 = B38400
        };
        Serial();
        Serial(const std::string device, const BaudRate baudrate);
        virtual ~Serial();
        bool init(void);
        bool kill(void);
        bool send(const std::string &str);
        std::string receive(const char terminate='\n');
    private:
        int m_port;
        std::string m_device;
        BaudRate m_baudrate;
        termios m_oldtio, m_newtio;
};
#endif  /* SERIAL_HPP */