#!/usr/bin/env python3
from abc import ABCMeta, abstractmethod
import socket
import paho.mqtt.client as mqtt
import threading

class MqttServer(metaclass=ABCMeta):
    def __init__(self, hostname=socket.gethostname(), mqtt_sub_ip="127.0.0.1", mqtt_sub_port=1883, mqtt_sub_key=None):
        assert mqtt_sub_key is not None, "you must be set mqtt_sub_key"
        # mqtt
        self.hostname      = hostname
        self.mqtt_sub_ip   = mqtt_sub_ip
        self.mqtt_sub_port = mqtt_sub_port
        self.mqtt_sub_key  = mqtt_sub_key
        self.mqtt_server   = mqtt.Client()

        self.mqtt_server.on_connect    = self.__callback_on_connect
        self.mqtt_server.on_disconnect = self.__callback_on_disconnect
        self.mqtt_server.on_subscribe  = self.__callback_on_subscribe
        self.mqtt_server.on_message    = self.execute

        self.mqtt_server.connect(host=self.mqtt_sub_ip, port=self.mqtt_sub_port, keepalive=60)
        self.mqtt_server.subscribe(topic=self.mqtt_sub_key + '/' + self.hostname)
        self.mqtt_thread = threading.Thread(target=self.mqtt_server.loop_forever)

        # frame count
        self.frm_cnt = -1

    def __callback_on_connect(self, client, userdata, flag, rc)->None:
        print("Connected with result code " + str(rc))

    def __callback_on_disconnect(self, client, userdata, rc)->None:
        print("Disconnected with result code " + str(rc))

    def __callback_on_subscribe(self, mqttc, obj, mid, granted_qos)->None:
        print("Subscribed: "+str(mid)+" "+str(granted_qos))

    def __del__(self):
        self.mqtt_server.disconnect()

    def event_loop(self):
        self.mqtt_thread.start()

    @abstractmethod
    def execute(self, client, userdata, msg)->None:
        raise NotImplementedError()