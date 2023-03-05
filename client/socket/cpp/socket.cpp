#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include "socket.hpp"

// Constructor
Socket::Socket() : m_sockfd(-1), m_port(1234), m_ipv4_addr("127.0.0.1"){}
Socket::Socket(std::string ipv4_addr, int port) : m_sockfd(-1), m_port(port), m_ipv4_addr(ipv4_addr) {}

// Destructor
Socket::~Socket()
{
    kill();
}

// Kill socket connection.
// return true or false.
bool
Socket::kill()
{
    if (close(m_sockfd) < 0)
    {
        std::cout << "Socket: close error." << std::endl;
        return false;
    }
    return true;
}

// Initialize socket connection.
// return true or false.
bool
Socket::init()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        std::cout << "Socket: socket error." << std::endl;
        return false;
    }

    struct sockaddr_in addr;
    /* clear sockaddr */
    memset(&addr, 0, sizeof(struct sockaddr_in));
    /* create address */
    addr.sin_family = AF_INET;  /* ipv4 */
    addr.sin_port = htons(m_port);
    std::size_t addr_sz = m_ipv4_addr.size();
    char *ipv4_char = new char[addr_sz];
    m_ipv4_addr.copy(ipv4_char, addr_sz);
    addr.sin_addr.s_addr = inet_addr(ipv4_char);

    /* connect socket */
    if (connect(m_sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        std::cout << "Socket: socket connect error" << std::endl;
        return false;
    }

    return true;
}

// Send message.
// return true or false.
bool
Socket::sendMsg(const std::string &send_msg)
{
    if (send_msg.size() <= 0)
    {
        std::cout << "Socket: client socket send message size error." << std::endl;
        return false;
    }

    std::size_t send_sz = send_msg.size() + 1;
    char *send_chars = new char[send_sz];
    send_msg.copy(send_chars, send_msg.size());
    send_chars[send_msg.size()] = '\0';   /* terminating character */

    if (send(m_sockfd, send_chars, send_sz, 0) != send_sz)
    {
        std::cout << "Socket: client socket send message error." << std::endl;
        return false;
    }
    return true;
}

// Receive message.
// return message from socket.
std::string
Socket::recvMsg(std::size_t recv_sz)
{
    if (recv_sz <= 0)
    {
        std::cout << "Socket: client socket recv_sz error." << std::endl;
        return "";
    }

    std::string recv_msg;
    char *recv_char = new char[recv_sz];
    if (recv(m_sockfd, recv_char, recv_sz, 0) != recv_sz)
    {
        std::cout << "Socket: client socket receive message error." << std::endl;
        return "";
    }

    recv_msg.copy(recv_char, recv_sz);
    return recv_msg;
}

// Receive message. (One character at a time.)
// return message from socket.
std::string
Socket::recvMsg(const bool wait, const char terminate)
{
    std::string recv_msg;
    bool is_recv = false;
    char recv_char;
    while(true)
    {
        if (recv(m_sockfd, &recv_char, 1, 0) > 0)
        {
            is_recv = true;
            recv_msg.append(1, recv_char);
            if(recv_char == terminate)
            {
                break;
            }
        }
        else
        {
            if(!wait || is_recv)
            {
                break;
            }
        }
    }
    return recv_msg;
}