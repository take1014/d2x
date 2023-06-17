#!/usr/bin/env python3
from mqtt_subscriber import MqttSubscriber
import numpy as np
import json
import cv2

class Runner(MqttSubscriber):
    def __init__(self, mqtt_sub_key):
        super(Runner, self).__init__(mqtt_sub_key=mqtt_sub_key)
        self.model = None

    def execute(self, client, userdata, msg) -> None:
        dict_info = json.loads(msg.payload)
        if self.frm_cnt == dict_info['frm_cnt']:
            return
        self.frm_cnt = dict_info['frm_cnt']
        img = np.array(dict_info['img']).reshape(dict_info['img_shape']).astype(np.uint8)

        cv2.imshow('runner window', img)
        if cv2.waitKey(1) == 27:
            return

        print(f"subscribe: {dict_info['frm_cnt']}")

if __name__ == '__main__':
    runner = Runner(mqtt_sub_key='d2x/cam')
    # runner = Runner()
    runner.event_loop()