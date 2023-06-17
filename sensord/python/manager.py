#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import sys
import json
import threading

# GPS
from gps import GPS
from nmea_parser import NMEAParser

# Camera
from cam import Camera
from runner import Runner

def runGPS()->None:
    gps = GPS(serial_port="/dev/ttyUSB0")
    #gps = GPS(serial_port="/dev/ttyACM0")
    parser = NMEAParser()

    # TODO: must define extrinsic parameter
    # Change the value to 6 if you use a GPS that supports
    # both GPGSA and GNGSA reception
    sizeof_notlist_nmea = 5 # default: RMC, VTG, GGA, GLL, GSA (excluse GSV)

    # NMEA counter and flags
    # processing nmea counter
    nmea_cnt = 0
    total_nmea_sz = sizeof_notlist_nmea    # total count include GPGSV
    # Initialize flags
    isinit_gpgsv = False
    isinit_glgsv = False
    isinit_gpgsa = False
    isinit_gngsa = False
    issync       = False

    nmea_json = dict()
    while True:
        # get message from serial.
        msg  = gps.recvMsg()
        if len(msg) < 4 or msg[0]!='$': continue

        # parse message from string to NMEA format.
        nmea_type, parsed_nmea = parser.parseNMEA(msg)

        if nmea_type == None or nmea_type[2:] == "TXT": continue
        if nmea_cnt == 0:
            # issync = True if nmea_type[2:] == "GSV" and parsed_nmea["message_number"] == 1 else False
            issync = True if nmea_type[2:] == "GGA" else False
            # and parsed_nmea["message_number"] == 1 else False

        #  Not process if out of sync
        if not issync: continue

        try:
            nmea_json = {"GPS_OUTPUT":{}, "RAW_GNSS":{}} if nmea_cnt == 0 else nmea_json
            if nmea_type == "GPGSV" :
                if not isinit_gpgsv:
                    # Initialize GPGSV
                    nmea_json["RAW_GNSS"][nmea_type] = []
                    total_nmea_sz += parsed_nmea["total_message_num"]
                    isinit_gpgsv = True
                # needs processing per sattelite.
                nmea_json["RAW_GNSS"][nmea_type].append(parsed_nmea)
                print(msg.strip())
                nmea_cnt += 1

            elif nmea_type == "GLGSV":
                # TODO: create parser
                if not isinit_glgsv:
                    # Initialize GPGSV
                    nmea_json["RAW_GNSS"][nmea_type] = []
                    total_nmea_sz += parsed_nmea["total_message_num"]
                    isinit_glgsv = True
                # needs processing per sattelite.
                nmea_json["RAW_GNSS"][nmea_type].append(parsed_nmea)
                print(msg.strip())
                nmea_cnt += 1

            elif nmea_type == "GPGSA":
                # There are more than 12 satellites,
                # but since they are not used, only one type of GSA is saved.
                if not isinit_gpgsa:
                    nmea_json["RAW_GNSS"][nmea_type] = parsed_nmea
                    print(msg.strip())
                    isinit_gpgsa = True
                    nmea_cnt += 1

            elif nmea_type == "GNGSA":
                # There are more than 12 satellites,
                # but since they are not used, only one type of GSA is saved.
                if not isinit_gngsa:
                    nmea_json["RAW_GNSS"][nmea_type] = parsed_nmea
                    print(msg.strip())
                    isinit_gngsa = True
                    nmea_cnt += 1

            # RMC, VTG, GGA, GLL
            else:
                nmea_json["RAW_GNSS"][nmea_type] = parsed_nmea
                print(msg.strip())
                # increment saved nmea count
                nmea_cnt += 1

            if (nmea_cnt == total_nmea_sz) and (total_nmea_sz != sizeof_notlist_nmea):
                nmea_json["GPS_OUTPUT"]["key"] = gps.hostname
                nmea_json["GPS_OUTPUT"]["latitude"]  = nmea_json["RAW_GNSS"]["GNGGA"]["latitude"]
                nmea_json["GPS_OUTPUT"]["longitude"] = nmea_json["RAW_GNSS"]["GNGGA"]["longitude"]
                # write Log
                gps.writeLog(json.dumps(nmea_json))
                # publish to mqtt
                gps.publish(json.dumps(nmea_json))
                # initialize counter and flags
                nmea_cnt = 0
                total_nmea_sz = sizeof_notlist_nmea
                isinit_gpgsv = False
                isinit_glgsv = False
                isinit_gpgsa = False
                isinit_gngsa = False
                issync       = False

        except Exception as e:
            print("Occur EXCEPTION: {}".format(e.args))
            gps.writeLog(e.args)
            # initialize counter and flags
            nmea_cnt = 0
            total_nmea_sz = sizeof_notlist_nmea
            isinit_gpgsv = False
            isinit_glgsv = False
            isinit_gpgsa = False
            isinit_gngsa = False
            issync       = False
            continue

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

def main() -> None:
    # TODO: add proccesing thread

    # Run GPS Thread
    # gps_thread = threading.Thread(target=runGPS)
    # gps_thread.start()

    # Run Camera Thread
    cam_thread = threading.Thread(target=runCam)
    cam_thread.start()

    runner_thread = Runner()
    runner_thread.event_loop()

if __name__ == "__main__":
    main()