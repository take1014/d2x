#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>

class Socket{
public:
    Socket();
    Socket(std::string ipv4_addr, int port);
    virtual ~Socket();
    bool init();
    bool kill();
    bool sendMsg(const std::string &msg);
    std::string recvMsg(std::size_t recv_sz);
    std::string recvMsg(const bool wait=true, const char terminate='\0');   // One character at a time.

private:
    int m_sockfd;
    int m_port;
    int m_connect;
    std::string m_ipv4_addr;
};
#endif /* SOCKET_HPP */