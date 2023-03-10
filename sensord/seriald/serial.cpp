#include <unistd.h>
#include <fcntl.h>

#include "serial.hpp"

// Constructor
Serial::Serial() : m_port(-1),
                   m_device("/dev/ttyACM0"),
                   m_baudrate(eB9600){}
Serial::Serial(const std::string device, const BaudRate baudrate): m_port(-1),
                                                                   m_device(device),
                                                                   m_baudrate(baudrate){}
Serial::Serial(const std::string device, const int baudrate) : m_port(-1),
                                                               m_device(device)
{
    /* convert baudrate from integer */
    switch(baudrate)
    {
    case 4800:
        m_baudrate = eB4800;
        break;
    case 9600:
        m_baudrate = eB9600;
        break;
    case 19200:
        m_baudrate = eB19200;
        break;
    case 38400:
        m_baudrate = eB38400;
        break;
    default:
        std::cout << "Serial: Integer baudrate must be one of 4800, 9600, 19200, 38400. Set default baudrate(4800)." << std::endl;
        m_baudrate = eB4800;
        break;
    }
}

// Destructor
Serial::~Serial()
{
    kill();
}

// Kill serial connection.
// return true or false.
bool
Serial::kill()
{
   if (tcsetattr(m_port, TCSANOW, &m_oldtio) < 0)
    {
        std::cout << "Serial: tcsetattr error." << std::endl;
        return false;
    }

    if (close(m_port) < 0)
    {
        std::cout << "Serial: serial close error." << std::endl;
        return false;
    }
    return true;
}

// Initialize serial connection.
// return true or false.
bool
Serial::init()
{
   /* Initalize */
    m_port = open(m_device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (m_port < 0)
    {
        std::cout << "Serial: open error. device :\"" << m_device << "\""<<std::endl;
        return false;
    }

    /* Save now setting */
    if (tcgetattr(m_port, &m_oldtio) < 0)
    {
        std::cout << "Serial: tcgetattr error." << std::endl;
        close(m_port);
        return false;
    }

    m_newtio = m_oldtio;

    /* set baudrate */
    if ((cfsetispeed(&m_newtio, m_baudrate) < 0) || (cfsetospeed(&m_newtio, m_baudrate) < 0))
    {
        std::cout << "Serial: baudrate error. baudrate :\"" << m_baudrate << "\"" << std::endl;
        close(m_port);
        return false;
    }

    m_newtio.c_iflag = IGNPAR;
    m_newtio.c_oflag = 0;
    m_newtio.c_lflag = 0;
    m_newtio.c_cflag= (CS8 | CLOCAL | CREAD);

    /* clear serial io */
    /* set serial */
    if (tcsetattr(m_port, TCSANOW, &m_newtio) < 0)
    {
        std::cout << "Serial: tcsetattr error." << std::endl;
        close(m_port);
        return false;
    }
    return true;
}

// Send message.
// return true or false.
bool
Serial::sendMsg(const std::string &send_msg)
{
    if (send_msg.size() <= 0)
    {
        std::cout << "Serial: serial send message size error." << std::endl;
        return false;
    }

    std::size_t send_sz = send_msg.size() + 1;
    char *send_chars = new char[send_sz];
    send_msg.copy(send_chars, send_msg.size());
    send_chars[send_msg.size()] = '\0';   /* terminating character */
    if(write(m_port, send_chars, send_sz) != send_sz)
    {
        std::cout << "Serial: serial send message error." << std::endl;
        return false;
    }
    return true;
}

// Receive message.
// return message from serial.
std::string
Serial::recvMsg(const std::size_t recv_sz)
{
    if (recv_sz <= 0)
    {
        std::cout << "Serial: serial recv_sz error." << std::endl;
        return "";
    }

    std::string recv_msg;
    char *recv_char = new char[recv_sz];
    if (read(m_port, recv_char, recv_sz) != recv_sz)
    {
        std::cout << "Serial: serial receive message error." << std::endl;
        return "";
    }
    return recv_msg;
}

// Receive message. (One character at a time.)
// return message from serial.
std::string
Serial::recvMsg(const bool wait, const char terminate)
{
    std::string recv_msg;
    bool is_recv = false;
    char recv_char;
    while(true)
    {
        ssize_t errono = read(m_port, &recv_char, 1);
        if (errono > 0)
        {
            is_recv = true;
            recv_msg.append(1, recv_char);
            if (recv_char == terminate) { break;}
        }
        else
        {
            if (errono == EINTR)  { continue; }
            if (!wait || is_recv) {    break; }
        }
    }
    return recv_msg;
}