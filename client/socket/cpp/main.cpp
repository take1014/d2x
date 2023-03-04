#include <thread>
#include "gps.hpp"
#include "socket.hpp"

int main()
{
    GPS gps = GPS();
    if (gps.init("/dev/ttyACM0", Serial::eB9600))
    {
        /* Execute GPS thread */
        gps.start();
    }
    /* Socket Test */
    // Socket *socket = new Socket();
    // if (!socket->init())
    // {
    //     return 0;
    // }

    // while(true)
    // {
    //     // std::cout << "send msg to server" << std::endl;
    //     std::string send_msg = "send_msg_from_client";
    //     socket->sendMsg(send_msg);

    //     std::string recv_msg = socket->recvMsg();
    //     std::cout << recv_msg << std::endl;
    // }
}