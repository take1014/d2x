#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <iostream>
#include <termios.h>

// Connect Serial
// example:
//     Serial serial("/dev/ttyACM0", eB9600);
//     bool connected = serial.init();
//     if (!connected) return false;
//     std::istringstream ss(serial.receive('\n'));
//     std::string buffer;
//     while (std::getline(ss, buffer, '\n') ) {
//     error = parser.parse(buffer);
//    }
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
    //explicit Serial(const std::string device, const BaudRate baudrate);
    Serial(const std::string device, const BaudRate baudrate);
    Serial(const std::string device, const int baudrate);
    ~Serial();
    bool init();
    bool kill();
    bool sendMsg(const std::string &send_msg);
    std::string recvMsg(const std::size_t recv_sz);
    std::string recvMsg(const bool wait=true, const char terminate='\0');   // One character at a time.

private:
    int m_port;
    std::string m_device;
    BaudRate m_baudrate;
    termios m_oldtio;
    termios m_newtio;
};
#endif  /* SERIAL_HPP */