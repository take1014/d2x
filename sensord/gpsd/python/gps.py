#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import sys
import os
import socket
import json
import serial
from serial.tools import list_ports

import spdlog as spd
import pynmea2 as parser
import paho.mqtt.client as mqtt

from nmea_parser import NMEAParser

class GPS:
    def __init__(self, serial_port="/dev/ttyACM0", baudrate=9600, timeout=None,
                       mqtt_pub_host=socket.gethostname(), mqtt_pub_port=1883):
        devices = [info.device for info in list_ports.comports()]
        assert (serial_port in devices), ("Do not exists port -> {}".format(serial_port))
        assert (baudrate==4800 or baudrate==9600 or baudrate==19200 or baudrate==38400), ("baudrate must be 4800, 9600, 19200, 38400 but {}".format(baudrate))

        self.hostname  = mqtt_pub_host
        # serial
        self.serial_port = serial_port
        self.ser = serial.Serial(port=self.serial_port, baudrate=baudrate, timeout=timeout)
        # logger
        if not os.path.exists("./logs"):
            os.mkdir("./logs")
        self.logger = spd.RotatingLogger("GPS_LOG", "logs/gps.log", True, 1048576*5, 3)
        self.logger.set_pattern("[%H:%M:%S.%f] %v")
        # mqtt
        self.mqtt_client = mqtt.Client()
        self.mqtt_client.on_connect = self._callback_on_connect
        self.mqtt_client.on_publish = self._callback_on_publish
        self.mqtt_client.connect(host=self.hostname, port=mqtt_pub_port)
        self.mqtt_client.loop_start()

    def _callback_on_connect(self, client, userdata, flag, rc):
        print("Connected with result code " + str(rc))

    def _callback_on_publish(self, client, userdata, mid):
        print("publish: {0}".format(mid))

    def writeLog(self, nmea_json):
        self.logger.info(json.dumps(nmea_json))

    def publish(self, nmea_json):
        self.mqtt_client.publish(topic="gps/"+self.hostname, payload=json.dumps(nmea_json))

    def recvMsg(self):
        # read until \n
        return self.ser.readline().decode("utf-8")

    def sendMsg(self, msg):
        self.ser.write(msg)

if __name__ == "__main__":
    gps = GPS()
    parser = NMEAParser()

    nmea_cnt = 0
    sizeof_notlist_nmea = 5 # total count excluse GPGSV 
    total_nmea_sz = 0    # total count include GPGSV

    nmea_json = dict()

    while True:
        nmea_json = {"GPS_OUTPUT":{}, "RAW_GNSS":{}} if nmea_cnt == 0 else nmea_json

        msg  = gps.recvMsg()
        print(msg.strip())

        nmea_type, parsed_nmea = parser.parseNMEA(msg)

        if nmea_type == None:
            continue

        if nmea_type != "GPGSV":
            nmea_json["RAW_GNSS"][nmea_type] = parsed_nmea
        else:
            if total_nmea_sz == 0:
                # initilize GPGSV elements
                nmea_json["RAW_GNSS"][nmea_type] = []
                total_nmea_sz = sizeof_notlist_nmea + parsed_nmea["total_message_num"]
            # needs processing per sattelite.
            nmea_json["RAW_GNSS"][nmea_type].append(parsed_nmea)

        # increment saved nmea count
        nmea_cnt += 1
        if nmea_cnt == total_nmea_sz:
            nmea_json["GPS_OUTPUT"]["key"] = gps.hostname
            nmea_json["GPS_OUTPUT"]["latitude"]  = nmea_json["RAW_GNSS"]["GPRMC"]["latitude"]
            nmea_json["GPS_OUTPUT"]["longitude"] = nmea_json["RAW_GNSS"]["GPRMC"]["longitude"]
            # write Log
            gps.writeLog(nmea_json)
            # publish to mqtt
            gps.publish(nmea_json)
            # initialize counter
            nmea_cnt = 0
            total_nmea_sz = 0
