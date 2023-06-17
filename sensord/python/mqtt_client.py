#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import socket
import paho.mqtt.client as mqtt

class MqttClient:
    def __init__(self, hostname=socket.gethostname(), mqtt_pub_ip="127.0.0.1", mqtt_pub_port=1883, mqtt_pub_key='default'):
        # mqtt
        self.hostname      = hostname
        self.mqtt_pub_ip   = mqtt_pub_ip
        self.mqtt_pub_port = mqtt_pub_port
        self.mqtt_pub_key  = mqtt_pub_key
        self.mqtt_client   = mqtt.Client()

        self.mqtt_client.on_connect    = self.__callback_on_connect
        self.mqtt_client.on_disconnect = self.__callback_on_disconnect
        self.mqtt_client.on_publish    = self.__callback_on_publish

        self.mqtt_client.connect(host=self.mqtt_pub_ip, port=mqtt_pub_port, keepalive=60)
        self.mqtt_client.loop_start()

    def __callback_on_connect(self, client, userdata, flag, rc)->None:
        print("Connected with result code " + str(rc))

    def __callback_on_disconnect(self, client, userdata, rc)->None:
        print("Disconnected with result code " + str(rc))

    def __callback_on_publish(self, client, userdata, mid)->None:
        print("publish: {0}".format(mid))

    def __del__(self):
        self.mqtt_client.disconnect()

    def publish(self, payload=None)->None:
        assert payload is not None
        assert type(payload) == str
        self.mqtt_client.publish(topic=self.mqtt_pub_key + '/' + self.hostname, payload=payload)