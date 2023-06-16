#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import os
import socket
import serial
from serial.tools import list_ports

import spdlog as spd
import paho.mqtt.client as mqtt

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
        self.mqtt_client.on_connect = self.__callback_on_connect
        self.mqtt_client.on_publish = self.__callback_on_publish
        self.mqtt_client.connect(host=self.hostname, port=mqtt_pub_port)
        self.mqtt_client.loop_start()

    def __callback_on_connect(self, client, userdata, flag, rc):
        print("Connected with result code " + str(rc))

    def __callback_on_publish(self, client, userdata, mid):
        print("publish: {0}".format(mid))

    def writeLog(self, log:str):
        assert type(log)==str
        self.logger.info(log)

    def publish(self, msg:str):
        assert type(msg)==str
        self.mqtt_client.publish(payload=msg)

    def recvMsg(self):
        # read until \n
        return self.ser.readline().decode("utf-8")

    def sendMsg(self, msg):
        assert type(msg)==str
        self.ser.write(msg)
