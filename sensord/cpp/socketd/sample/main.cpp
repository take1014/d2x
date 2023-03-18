#include <thread>
#include "socket.hpp"

int main()
{
    Socket *socket = new Socket();
    if (!socket->init())
    {
        return 0;
    }

    while(true)
    {
        std::string recv_from_client = "send msg from client";
        // std::string recv_msg = socket->recvMsg(recv_from_client.size()+1);
        std::string recv_msg = socket->recvMsg();
        std::cout << recv_msg << std::endl;
        socket->sendMsg("send msg from server");
    }
    return 1;
}