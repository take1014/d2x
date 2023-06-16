#!/usr/bin/env python3
from abc import ABCMeta, abstractmethod
import socket
import paho.mqtt.client as mqtt
import threading

class MqttServer(metaclass=ABCMeta):
    def __init__(self, mqtt_sub_host=socket.gethostname(), mqtt_sub_ip="ws://big-ucsk.local:15675", mqtt_sub_key=None):
        assert mqtt_sub_key is not None, "you must be set mqtt_sub_key"
        # mqtt
        self.hostname     = mqtt_sub_host
        self.mqtt_sub_ip  = mqtt_sub_ip
        self.mqtt_sub_key = mqtt_sub_key
        self.mqtt_server  = mqtt.Client()

        self.mqtt_server.on_connect   = self.__callback_on_connect
        self.mqtt_server.on_subscribe = self.__callback_on_subscribe
        self.mqtt_server.on_message   = self.execute

        # self.mqtt_server.connect(host=socket.gethostname(), port=1883)
        self.mqtt_server.connect(host="127.0.0.1", port=1883, keepalive=60)
        print(self.mqtt_sub_key + '/' + self.hostname)
        self.mqtt_server.subscribe(topic=self.mqtt_sub_key + '/' + self.hostname)
        self.mqtt_thread = threading.Thread(target=self.mqtt_server.loop_forever)

    def __callback_on_connect(self, client, userdata, flag, rc)->None:
        print("Connected with result code " + str(rc))

    def __callback_on_subscribe(self, mqttc, obj, mid, granted_qos)->None:
        print("Subscribed: "+str(mid)+" "+str(granted_qos))

    def event_loop(self):
        self.mqtt_thread.start()

    @abstractmethod
    def execute(self, client, userdata, msg)->None:
        raise NotImplementedError()