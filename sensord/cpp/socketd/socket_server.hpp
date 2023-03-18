#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <iostream>

class SocketServer{
public:
    SocketServer();
    SocketServer(std::string ipv4_addr, int port);
    ~SocketServer();
    bool init();
    bool kill();
    bool sendMsg(const std::string &msg);
    std::string recvMsg(const std::size_t recv_sz);
    std::string recvMsg(const bool wait=true, const char terminate='\0');   // One character at a time.

private:
    int m_sockfd;
    int m_port;
    int m_connect;
    std::string m_ipv4_addr;
};
#endif /* SOCKET_SERVER_HPP */
