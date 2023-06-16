#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import sys
import os
import socket

import cv2
import json
import spdlog
import numpy as np

from mqtt_client import MqttClient

class Camera:
    def __init__(self, device_id=0):
        assert (device_id >= 0)
        self.device_id  = device_id
        # camera open
        self.cap = cv2.VideoCapture(self.device_id)
        assert self.cap.isOpened()
        self.img = None
        # mqtt
        self.mqtt_client = MqttClient(mqtt_pub_key='d2x/cam')
        self.frm_cnt = 0

    def read(self):
        ret, img = self.cap.read()
        self.img = img if ret else None
        self.frm_cnt += 1
        return self.img

    def write(self, img, filename=None)->None:
        assert img is not None
        assert filename is not None
        cv2.imwrite(filename, img)

    def getOriginImg(self):
        return self.img

    def resize(self, img, resize_h, resize_w):
        assert img is not None
        return cv2.resize(img, (resize_w, resize_h))

    def show(self, img, window_name='camera')->None:
        assert img is not None
        cv2.imshow(window_name, img)

    def wait(self, wait_time=10):
        return cv2.waitKey(wait_time)

    def publish(self, img)->None:
        assert img is not None
        assert type(img) == np.ndarray
        payload = json.dumps({"img":img.flatten().tolist(), "frm_cnt":self.frm_cnt, "img_shape": img.shape})
        self.mqtt_client.publish(payload=payload)

    def __del__(self)->None:
        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    import threading

    def runCam()->None:
        cam = Camera()
        while True:
            img = cam.read()
            img = cam.resize(img, resize_h=480, resize_w=640)
            # TODO: add inference
            cam.show(img)
            cam.publish(img)
            if cam.wait(1) == 27:
                break

    cam_thread = threading.Thread(target=runCam)
    cam_thread.start()