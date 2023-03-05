#include "gps.hpp"
#include "socket.hpp"
#include <fstream>
#include <json/json.h>

int main()
{
    Json::Value app_conf;
    Json::Reader reader;
    std::ifstream conf_json("/home/take/fun/d2x/app_conf.json");
    reader.parse(conf_json, app_conf);

    GPS gps = GPS();

    /* Set GPS configs */
    GPS::GpsConf_t gps_conf = GPS::GpsConf_t();
    gps_conf.device         = app_conf["CLIENT"]["SERIAL"]["DEVICE"].asString();
    gps_conf.baudrate       = app_conf["CLIENT"]["SERIAL"]["BAUDRATE"].asInt();
    gps_conf.mqtt_local_ip  = app_conf["CLIENT"]["GPS"]["LOCAL_PUB_IP"].asString();
    gps_conf.mqtt_local_id  = app_conf["CLIENT"]["GPS"]["LOCAL_PUB_ID"].asString();
    gps_conf.mqtt_server_ip = app_conf["CLIENT"]["GPS"]["SERVER_PUB_IP"].asString();
    gps_conf.mqtt_server_id = app_conf["CLIENT"]["GPS"]["SERVER_PUB_ID"].asString();

    if (gps.init(gps_conf))
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