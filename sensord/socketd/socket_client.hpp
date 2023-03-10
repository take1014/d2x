#ifndef SOCKET_CLIENT_HPP
#define SOCKET_CLIENT_HPP

#include <iostream>

class SocketClient{
public:
    SocketClient();
    SocketClient(std::string ipv4_addr, int port);
    ~SocketClient();
    bool init();
    bool kill();
    bool sendMsg(const std::string &msg);
    std::string recvMsg(std::size_t recv_sz);
    std::string recvMsg(const bool wait=true, const char terminate='\0');   // One character at a time.

private:
    int m_sockfd;
    int m_port;
    std::string m_ipv4_addr;
};
#endif /* SOCKET_CLIENT_HPP */
