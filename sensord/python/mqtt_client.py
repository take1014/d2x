#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import socket
import paho.mqtt.client as mqtt

class MqttClient:
    def __init__(self, mqtt_pub_host=socket.gethostname(), mqtt_pub_port=1883, mqtt_pub_key='default'):
        # mqtt
        self.hostname = mqtt_pub_host
        self.mqtt_pub_key = mqtt_pub_key
        self.mqtt_client = mqtt.Client()
        self.mqtt_client.on_connect = self.__callback_on_connect
        self.mqtt_client.on_publish = self.__callback_on_publish
        self.mqtt_client.connect(host=self.hostname, port=mqtt_pub_port)
        self.mqtt_client.loop_start()

    def __callback_on_connect(self, client, userdata, flag, rc)->None:
        print("Connected with result code " + str(rc))

    def __callback_on_publish(self, client, userdata, mid)->None:
        print("publish: {0}".format(mid))

    def publish(self, payload=None)->None:
        assert payload is not None
        assert type(payload) == str
        self.mqtt_client.publish(topic=self.mqtt_pub_key + '/' + self.hostname, payload=payload)