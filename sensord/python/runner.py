#!/usr/bin/env python3
from mqtt_server import MqttServer
import numpy as np
import json

class Runner(MqttServer):
    def execute(self, client, userdata, msg) -> None:
        dict_info = json.loads(msg.payload)
        img = np.array(dict_info['img']).reshape(dict_info['img_shape'])
        print(img.shape, dict_info['frm_cnt'])

if __name__ == '__main__':
    runner = Runner(mqtt_sub_key='d2x/cam')
    runner.event_loop()