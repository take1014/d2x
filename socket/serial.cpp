#include "serial.hpp"

// Constructor
Serial::Serial() : m_device("/dev/ttyACM0"),
                   m_baudrate(eB9600){}
Serial::Serial(const std::string device, const BaudRate baudrate): m_device(device),
                                                                   m_baudrate(baudrate){}
// Destructor
Serial::~Serial()
{
    kill();
}

// Kill serial connection.
// return true or false.
bool Serial::kill(void)
{
   if (tcsetattr(m_port, TCSANOW, &m_oldtio) < 0)
    {
        std::cout << "tcsetattr error." << std::endl;
        return false;
    }
    close(m_port);
    return true;
}

// Initialize serial connection.
// return true or false.
bool Serial::init(void)
{
   /* Initalize */
    m_port = open(m_device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    if (m_port == -1)
    {
        std::cout << "open error. device :\"" << m_device << "\""<<std::endl;
        return false;
    }

    /* Save now setting */
    if (tcgetattr(m_port, &m_oldtio) < 0)
    {
        std::cout << "tcgetattr error." << std::endl;
        return false;
    }

    m_newtio = m_oldtio;

    /* set baudrate */
    if (cfsetspeed(&m_newtio, m_baudrate) < 0)
    {
        std::cout << "baudrate error. baudrate :\"" << m_baudrate << "\"" << std::endl;
        return false;
    }

    /* clear serial io */
    if (tcflush(m_port, TCIFLUSH) < 0)
    {
        std::cout << "tcflush error." << std::endl;
        return false;
    }
    /* set serial */
    if (tcsetattr(m_port, TCSANOW, &m_newtio) < 0)
    {
        std::cout << "tcsetattr error." << std::endl;
        return false;
    }

    return true;
}

// Receive message from serial.
// return message from serial.
std::string Serial::receive(const char terminate)
{
    std::string received_msg;
    bool is_received = false;
    char received_char;
    while(true)
    {
        int read_size = read(m_port, &received_char, 1);

        if(read_size <= 0 && is_received) break;

        is_received = true;
        received_msg.append(1, received_char);

        if(received_char == terminate) break;
    }
    return received_msg;
}

// Send message to Serial
// return true or false.
bool Serial::send(const std::string &send_msg)
{
    size_t send_sz = send_msg.size() + 1;
    char *send_chars = new char[send_sz];
    send_msg.copy(send_chars, send_msg.size());
    send_chars[send_msg.size()] = '\0';   /* terminating character */

    if(write(m_port, send_chars, send_sz) != send_sz)
    {
        std::cout << "send message error." << std::endl;
        return false;
    }
    return true;
}