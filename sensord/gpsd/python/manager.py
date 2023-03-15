#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import sys
import json
import threading
from gps import GPS
from nmea_parser import NMEAParser

def run_gps()->None:
    gps = GPS()
    parser = NMEAParser()

    nmea_cnt = 0
    sizeof_notlist_nmea = 5 # total count excluse GPGSV 
    total_nmea_sz = 0    # total count include GPGSV

    nmea_json = dict()

    while True:
        # get message from serial.
        msg  = gps.recvMsg()
        if len(msg) < 4 or msg[0]!='$': continue
        # parse message from string to NMEA format.
        nmea_type, parsed_nmea = parser.parseNMEA(msg)
        if nmea_type == None: continue

        print(msg.strip())

        nmea_json = {"GPS_OUTPUT":{}, "RAW_GNSS":{}} if nmea_cnt == 0 else nmea_json
        if nmea_type != "GPGSV":
            nmea_json["RAW_GNSS"][nmea_type] = parsed_nmea
        else:
            if total_nmea_sz == 0:
                # initilize GPGSV elements
                nmea_json["RAW_GNSS"][nmea_type] = []
                total_nmea_sz = sizeof_notlist_nmea + parsed_nmea["total_message_num"]
            # needs processing per sattelite.
            nmea_json["RAW_GNSS"][nmea_type].append(parsed_nmea)

        # increment saved nmea count
        nmea_cnt += 1
        if nmea_cnt == total_nmea_sz:
            nmea_json["GPS_OUTPUT"]["key"] = gps.hostname
            nmea_json["GPS_OUTPUT"]["latitude"]  = nmea_json["RAW_GNSS"]["GPRMC"]["latitude"]
            nmea_json["GPS_OUTPUT"]["longitude"] = nmea_json["RAW_GNSS"]["GPRMC"]["longitude"]
            # write Log
            gps.writeLog(json.dumps(nmea_json))
            # publish to mqtt
            gps.publish(json.dumps(nmea_json))
            # initialize counter
            nmea_cnt = 0
            total_nmea_sz = 0

def main() -> None:
    # TODO:add proccesing thread
    gps_thread = threading.Thread(target=run_gps)
    gps_thread.start()

if __name__ == "__main__":
    main()
