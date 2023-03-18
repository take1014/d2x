#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include "socket_server.hpp"

// Constructor
SocketServer::SocketServer() : m_sockfd(-1), m_port(1234), m_ipv4_addr("127.0.0.1"), m_connect(-1) {}
SocketServer::SocketServer(std::string ipv4_addr, int port) : m_sockfd(-1), m_port(port), m_ipv4_addr(ipv4_addr), m_connect(-1) {}

// Destructor
SocketServer::~SocketServer()
{
    kill();
}

// Kill socket connection.
// return true or false.
bool
SocketServer::kill()
{
    if (close(m_sockfd) < 0)
    {
        std::cout << "SocketServer: socket close error." << std::endl;
        return false;
    }

    if (close(m_connect) < 0)
    {
        std::cout << "SocketServer: socket connect close error." << std::endl;
        return false;
    }

    return true;
}

// Initialize socket connection.
// return true or false.
bool
SocketServer::init()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        std::cout << "SocketServer: socket error." << std::endl;
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

    /* save socket */
    if (bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cout << "SocketServer: socket bind error." << std::endl;
        return false;
    }

    /* wait receive */
    if (listen(m_sockfd, SOMAXCONN) < 0)
    {
        std::cout << "SocketServer: socket listen error." << std::endl;
        close(m_sockfd);
        return false;
    }

    /* wait connection */
    struct sockaddr_in get_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    m_connect = accept(m_sockfd, (struct sockaddr*)&get_addr, &len);
    if (m_connect < 0)
    {
        std::cout << "SocketServer: socket accept error." << std::endl;
        return false;
    }
    return true;
}

// Send message.
// return true or false.
bool
SocketServer::sendMsg(const std::string &send_msg)
{
    if (send_msg.size() <= 0)
    {
        std::cout << "SocketServer: socket send message size error." << std::endl;
        return false;
    }

    std::size_t send_sz = send_msg.size() + 1;
    char *send_chars = new char[send_sz];
    send_msg.copy(send_chars, send_msg.size());
    send_chars[send_msg.size()] = '\0';   /* terminating character */
    if (send(m_connect, send_chars, send_sz, 0) != send_sz)
    {
        std::cout << "SocketServer: socket send message error." << std::endl;
        return false;
    }
    return true;
}

// Receive message.
// return message from socket.
std::string
SocketServer::recvMsg(const std::size_t recv_sz)
{
    if (recv_sz <= 0)
    {
        std::cout << "SocketServer: socket recv_sz error." << std::endl;
        return "";
    }

    std::string recv_msg;
    char *recv_char = new char[recv_sz];
    if (recv(m_connect, recv_char, recv_sz, 0) != recv_sz)
    {
        std::cout << "SocketServer: socket receive message error." << std::endl;
        return "";
    }

    recv_msg.copy(recv_char, recv_sz);
    return recv_msg;
}

// Receive message. (One character at a time.)
// return message from socket.
std::string
SocketServer::recvMsg(const bool wait, const char terminate)
{
    std::string recv_msg;
    bool is_recv = false;
    char recv_char;
    while(true)
    {
        if (recv(m_connect, &recv_char, 1, 0) > 0)
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
