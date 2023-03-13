#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import sys
import json
import pynmea2 as parser
import paho.mqtt.client as mqtt
import serial
from serial.tools import list_ports

class GPS:
    def __init__(self, serial_port="/dev/ttyACM0", baudrate=9600, timeout=None,
                       mqtt_pub_url="localhost", mqtt_pub_port=1883):
        devices = [info.device for info in list_ports.comports()]
        assert (serial_port in devices), ("Do not exists port -> {}".format(serial_port))
        assert (baudrate==4800 or baudrate==9600 or baudrate==19200 or baudrate==38400), ("baudrate must be 4800, 9600, 19200, 38400 but {}".format(baudrate))
        self.serial_port     = serial_port
        self.baudrate = baudrate
        self.timeout  = timeout
        self.ser = serial.Serial(port=self.serial_port, baudrate=self.baudrate, timeout=self.timeout)
        self.mqtt_client = mqtt.Client()
        self.mqtt_client.on_connect = self._callback_on_connect
        self.mqtt_client.on_publish = self._callback_on_publish
        self.mqtt_client.connect(host=mqtt_pub_url, port=mqtt_pub_port)
        self.mqtt_client.loop_start()

    def _callback_on_connect(self, client, userdata, flag, rc):
        print("Connected with result code " + str(rc))

    def _callback_on_publish(self, client, userdata, mid):
        print("publish: {0}".format(mid))

    def publish(self, nmea_json):
        self.mqtt_client.publish(topic="gps/ucsk", payload=json.dumps(nmea_json))

    def recvMsg(self):
        # read until \n
        return self.ser.readline().decode("utf-8")

    def sendMsg(self, msg):
        self.ser.write(msg)

    def parseGPGGA(self, nmea, nmea_json):
        nmea_json["RAW_GNSS"]["GPGGA"]["time_utc"]       = nmea.timestamp.isoformat()
        nmea_json["RAW_GNSS"]["GPGGA"]["latitude"]       = nmea.latitude
        nmea_json["RAW_GNSS"]["GPGGA"]["lat_direction"]  = nmea.lat_dir
        nmea_json["RAW_GNSS"]["GPGGA"]["longitude"]      = nmea.longitude
        nmea_json["RAW_GNSS"]["GPGGA"]["long_direction"] = nmea.lon_dir
        nmea_json["RAW_GNSS"]["GPGGA"]["quality"]        = nmea.gps_qual
        nmea_json["RAW_GNSS"]["GPGGA"]["number_of_satellites"] = int(nmea.num_sats)
        nmea_json["RAW_GNSS"]["GPGGA"]["horizontal_dilution_of_precision"] = float(nmea.horizontal_dil)
        nmea_json["RAW_GNSS"]["GPGGA"]["altitude"] = nmea.altitude
        nmea_json["RAW_GNSS"]["GPGGA"]["altitude_units"] = nmea.altitude_units
        nmea_json["RAW_GNSS"]["GPGGA"]["undulation"] = float(nmea.geo_sep) if nmea.geo_sep != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGGA"]["undulation_units"] = nmea.geo_sep_units
        nmea_json["RAW_GNSS"]["GPGGA"]["age"] = float(nmea.age_gps_data) if (nmea.age_gps_data != '') else 0.0
        nmea_json["RAW_GNSS"]["GPGGA"]["differential_station_ID"] = nmea.ref_station_id
        nmea_json["RAW_GNSS"]["GPGGA"]["checksum"] = nmea.checksum(msg)

    def parseGPRMC(self, nmea, nmea_json):
        nmea_json["RAW_GNSS"]["GPRMC"]["time_utc"]                     = nmea.datetime.isoformat()
        nmea_json["RAW_GNSS"]["GPRMC"]["status"]                       = nmea.status
        nmea_json["RAW_GNSS"]["GPRMC"]["latitude"]                     = nmea.latitude
        nmea_json["RAW_GNSS"]["GPRMC"]["lat_direction"]                = nmea.lat_dir
        nmea_json["RAW_GNSS"]["GPRMC"]["longitude"]                    = nmea.longitude
        nmea_json["RAW_GNSS"]["GPRMC"]["long_direction"]               = nmea.lon_dir
        nmea_json["RAW_GNSS"]["GPRMC"]["knots"]                        = nmea.spd_over_grnd
        nmea_json["RAW_GNSS"]["GPRMC"]["degrees"]                      = float(nmea.true_course) if (nmea.true_course != None) else 0.0
        nmea_json["RAW_GNSS"]["GPRMC"]["date"]                         = nmea.datetime.isoformat()
        nmea_json["RAW_GNSS"]["GPRMC"]["magnetic_variation"]           = float(nmea.mag_variation) if(nmea.mag_variation != '') else 0.0
        nmea_json["RAW_GNSS"]["GPRMC"]["magnetic_variation_direction"] = nmea.mag_var_dir
        nmea_json["RAW_GNSS"]["GPRMC"]["mode"]                         = nmea.mode_indicator
        nmea_json["RAW_GNSS"]["GPRMC"]["checksum"]                     = nmea.checksum(msg)

    def parseGPGLL(self, nmea, nmea_json):
        nmea_json["RAW_GNSS"]["GPGLL"]["latitude"]       = nmea.latitude
        nmea_json["RAW_GNSS"]["GPGLL"]["lat_direction"]  = nmea.lat_dir
        nmea_json["RAW_GNSS"]["GPGLL"]["longitude"]      = nmea.longitude
        nmea_json["RAW_GNSS"]["GPGLL"]["long_direction"] = nmea.lon_dir
        nmea_json["RAW_GNSS"]["GPGLL"]["time_utc"]       = nmea.timestamp.isoformat()
        nmea_json["RAW_GNSS"]["GPGLL"]["status"]         = nmea.status
        nmea_json["RAW_GNSS"]["GPGLL"]["mode"]           = nmea.faa_mode
        nmea_json["RAW_GNSS"]["GPGLL"]["checksum"]       = nmea.checksum(msg)

    def parseGPGSA(self, nmea, nmea_json):
        nmea_json["RAW_GNSS"]["GPGSA"]["mode"]            = nmea.mode
        nmea_json["RAW_GNSS"]["GPGSA"]["status"]          = int(nmea.mode_fix_type)
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_1"]  = int(nmea.sv_id01) if nmea.sv_id01 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_2"]  = int(nmea.sv_id02) if nmea.sv_id02 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_3"]  = int(nmea.sv_id03) if nmea.sv_id03 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_4"]  = int(nmea.sv_id04) if nmea.sv_id04 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_5"]  = int(nmea.sv_id05) if nmea.sv_id05 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_6"]  = int(nmea.sv_id06) if nmea.sv_id06 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_7"]  = int(nmea.sv_id07) if nmea.sv_id07 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_8"]  = int(nmea.sv_id08) if nmea.sv_id08 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_9"]  = int(nmea.sv_id09) if nmea.sv_id09 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_10"] = int(nmea.sv_id10) if nmea.sv_id10 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_11"] = int(nmea.sv_id11) if nmea.sv_id11 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["satellite_id_12"] = int(nmea.sv_id12) if nmea.sv_id12 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSA"]["pdop"]            = float(nmea.pdop)
        nmea_json["RAW_GNSS"]["GPGSA"]["hdop"]            = float(nmea.hdop)
        nmea_json["RAW_GNSS"]["GPGSA"]["vdop"]            = float(nmea.vdop)
        nmea_json["RAW_GNSS"]["GPGSA"]["checksum"]        = nmea.checksum(msg)

    def parseGPGSV(self, nmea, nmea_json):
        nmea_json["RAW_GNSS"]["GPGSV"]["total_message_num"]      = int(nmea.num_messages)
        nmea_json["RAW_GNSS"]["GPGSV"]["message_number"]         = int(nmea.msg_num)
        nmea_json["RAW_GNSS"]["GPGSV"]["total_satellite_number"] = int(nmea.num_sv_in_view)
        nmea_json["RAW_GNSS"]["GPGSV"]["satellite_number_1"]     = int(nmea.sv_prn_num_1) if nmea.sv_prn_num_1 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSV"]["elevation_1"]            = float(nmea.elevation_deg_1) if nmea.elevation_deg_1 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["azimuth_1"]              = float(nmea.azimuth_1) if nmea.azimuth_1 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["sn_1"]                   = float(nmea.snr_1) if nmea.snr_1 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["satellite_number_2"]     = int(nmea.sv_prn_num_2) if nmea.sv_prn_num_2 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSV"]["elevation_2"]            = float(nmea.elevation_deg_2) if nmea.elevation_deg_2 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["azimuth_2"]              = float(nmea.azimuth_2) if nmea.azimuth_2 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["sn_2"]                   = float(nmea.snr_2) if nmea.snr_2 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["satellite_number_3"]     = int(nmea.sv_prn_num_3) if nmea.sv_prn_num_3 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSV"]["elevation_3"]            = float(nmea.elevation_deg_3) if nmea.elevation_deg_3 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["azimuth_3"]              = float(nmea.azimuth_3) if nmea.azimuth_3 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["sn_3"]                   = float(nmea.snr_3) if nmea.snr_3 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["satellite_number_4"]     = int(nmea.sv_prn_num_4) if nmea.sv_prn_num_4 != '' else 0
        nmea_json["RAW_GNSS"]["GPGSV"]["elevation_4"]            = float(nmea.elevation_deg_4) if nmea.elevation_deg_4 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["azimuth_4"]              = float(nmea.azimuth_4) if nmea.azimuth_4 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["sn_4"]                   = float(nmea.snr_4) if nmea.snr_4 != '' else 0.0
        nmea_json["RAW_GNSS"]["GPGSV"]["checksum"]               = nmea.checksum(msg)

    def parseGPVTG(self, nmea, nmea_json):
        nmea_json["RAW_GNSS"]["GPVTG"]["track"]               = float(nmea.true_track) if nmea.true_track != None else 0.0
        nmea_json["RAW_GNSS"]["GPVTG"]["track_mode"]          = nmea.true_track_sym
        nmea_json["RAW_GNSS"]["GPVTG"]["magnetic_track"]      = float(nmea.mag_track) if nmea.mag_track != None else 0.0
        nmea_json["RAW_GNSS"]["GPVTG"]["magnetic_track_mode"] = nmea.mag_track_sym
        nmea_json["RAW_GNSS"]["GPVTG"]["ground_speed_knots"]  = float(nmea.spd_over_grnd_kts) if nmea.spd_over_grnd_kts != None else 0.0
        nmea_json["RAW_GNSS"]["GPVTG"]["knots"]               = nmea.spd_over_grnd_kts_sym
        nmea_json["RAW_GNSS"]["GPVTG"]["ground_speed_kmh"]    = float(nmea.spd_over_grnd_kmph) if nmea.spd_over_grnd_kmph != None else 0.0
        nmea_json["RAW_GNSS"]["GPVTG"]["knots"]               = nmea.spd_over_grnd_kmph_sym
        nmea_json["RAW_GNSS"]["GPVTG"]["mode"]                = nmea.faa_mode
        nmea_json["RAW_GNSS"]["GPVTG"]["checksum"]            = nmea.checksum(msg)

if __name__ == "__main__":
    gps = GPS()
    json_open = open("../nmea_format.json", 'r')
    nmea_json =  json.load(json_open)

    while True:
        msg = gps.recvMsg()
        nmea = parser.parse(msg)
        print(nmea)
        if nmea.sentence_type == "GGA":
            gps.parseGPGGA(nmea, nmea_json)
        elif nmea.sentence_type == "RMC":
            gps.parseGPRMC(nmea, nmea_json)
        elif nmea.sentence_type == "GLL":
            gps.parseGPGLL(nmea, nmea_json)
        elif nmea.sentence_type == "GSA":
            gps.parseGPGSA(nmea, nmea_json)
        elif nmea.sentence_type == "GSV":
            gps.parseGPGSV(nmea, nmea_json)
        elif nmea.sentence_type == "VTG":
            gps.parseGPVTG(nmea, nmea_json)
        elif nmea.sentence_type == "ZDA":
            # VFANでは取得できないので後回し
            continue

        nmea_json["GPS_OUTPUT"]["key"] = "takehara"
        nmea_json["GPS_OUTPUT"]["latitude"] = nmea_json["RAW_GNSS"]["GPRMC"]["latitude"]
        nmea_json["GPS_OUTPUT"]["longitude"] = nmea_json["RAW_GNSS"]["GPRMC"]["longitude"]

        gps.publish(nmea_json)