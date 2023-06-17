#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import os
import cv2
import json
import numpy as np

from mqtt_publisher import MqttPublisher

class Camera:
    def __init__(self, device_id=0, mqtt_pub_ip='127.0.0.1', mqtt_pub_port=1883):
        assert (device_id >= 0)
        self.device_id  = device_id
        # camera open
        self.cap = cv2.VideoCapture(self.device_id)
        assert self.cap.isOpened()
        self.img = None

        # mqtt
        self.mqtt_pub_ip   = mqtt_pub_ip
        self.mqtt_pub_port = mqtt_pub_port
        self.mqtt_client = MqttPublisher(mqtt_pub_ip=self.mqtt_pub_ip, mqtt_pub_port=self.mqtt_pub_port, mqtt_pub_key='d2x/cam')
        self.frm_cnt = -1

    def read(self):
        ret, img = self.cap.read()
        self.img = img if ret else None
        self.frm_cnt += 1
        return self.img

    def write(self, img, filepath)->None:
        assert img is not None
        assert type(filepath) == str

        if os.path.exists(os.path.dirname(filepath)):
            os.makedirs(os.path.dirname(filepath))
        cv2.imwrite(filepath, img)

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
            img = cam.resize(img, resize_h=480//3, resize_w=640//2)
            # img = cam.resize(img, resize_h=480//3, resize_w=640//2)
            # TODO: add inference
            _img = cam.getOriginImg()
            cam.show(_img)
            cam.publish(img)
            if cam.wait(1) == 27:
                break

    cam_thread = threading.Thread(target=runCam)
    cam_thread.start()