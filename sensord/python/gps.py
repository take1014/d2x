#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import os
import serial
from serial.tools import list_ports
import spdlog as spd

from mqtt_publisher import MqttPublisher

class GPS:
    def __init__(self, serial_port="/dev/ttyACM0", baudrate=9600, timeout=None, mqtt_pub_ip='127.0.0.1', mqtt_pub_port=1883):
        devices = [info.device for info in list_ports.comports()]
        assert (serial_port in devices), ("Do not exists port -> {}".format(serial_port))
        assert (baudrate==4800 or baudrate==9600 or baudrate==19200 or baudrate==38400), ("baudrate must be 4800, 9600, 19200, 38400 but {}".format(baudrate))

        # serial
        self.serial_port = serial_port
        self.ser = serial.Serial(port=self.serial_port, baudrate=baudrate, timeout=timeout)
        # logger
        if not os.path.exists("./logs"):
            os.mkdir("./logs")
        self.logger = spd.RotatingLogger("GPS_LOG", "logs/gps.log", True, 1048576*5, 3)
        self.logger.set_pattern("[%H:%M:%S.%f] %v")

        # mqtt
        self.mqtt_pub_ip   = mqtt_pub_ip
        self.mqtt_pub_port = mqtt_pub_port
        self.mqtt_client   = MqttPublisher(mqtt_pub_ip=self.mqtt_pub_ip, mqtt_pub_port=self.mqtt_pub_port, mqtt_pub_key='d2x/gps')
        self.frm_cnt = -1

    def writeLog(self, log:str):
        assert type(log)==str
        self.logger.info(log)

    def publish(self, msg:str):
        assert type(msg)==str
        self.mqtt_client.publish(payload=msg)
        self.frm_cnt += 1

    def recvMsg(self):
        # read until \n
        return self.ser.readline().decode("utf-8")

    def sendMsg(self, msg):
        assert type(msg)==str
        self.ser.write(msg)