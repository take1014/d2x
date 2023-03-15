#!usr/bin/env python3
#-*- coding:utf-8 -*-
import pynmea2 as parser

class NMEAParser:
    def __init__(self, sentence=""):
        self.sentence = ""

    def parseNMEA(self, sentence:str):
        self.sentence = sentence
        nmea = parser.parse(sentence)
        nmea_type = None
        if nmea.sentence_type == "RMC":
            nmea_type = "GPRMC"
            parsed_sentence = self.__parseGPRMC(nmea)
        elif nmea.sentence_type == "VTG":
            nmea_type = "GPVTG"
            parsed_sentence = self.__parseGPVTG(nmea)
        elif nmea.sentence_type == "GGA":
            nmea_type = "GPGGA"
            parsed_sentence = self.__parseGPGGA(nmea)
        elif nmea.sentence_type == "GSA":
            nmea_type = "GPGSA"
            parsed_sentence = self.__parseGPGSA(nmea)
        elif nmea.sentence_type == "GSV":
            nmea_type = "GPGSV"
            parsed_sentence = self.__parseGPGSV(nmea)
        elif nmea.sentence_type == "GLL":
            nmea_type = "GPGLL"
            parsed_sentence = self.__parseGPGLL(nmea)
        elif nmea.sentence_type == "ZDA":
            # not get GPZDA using VFAN
            parsed_sentence = None
        else:
            parsed_sentence = None

        # calc checksum
        if parsed_sentence is not None:
            parsed_sentence["checksum"] = nmea.checksum(sentence)

        return nmea_type, parsed_sentence

    def __parseGPGGA(self, nmea):
        gpgga = dict()
        gpgga["time_utc"]       = nmea.timestamp.isoformat() if nmea.timestamp != None else ""
        gpgga["latitude"]       = nmea.latitude
        gpgga["lat_direction"]  = nmea.lat_dir
        gpgga["longitude"]      = nmea.longitude
        gpgga["long_direction"] = nmea.lon_dir
        gpgga["quality"]        = nmea.gps_qual
        gpgga["number_of_satellites"] = int(nmea.num_sats)
        gpgga["horizontal_dilution_of_precision"] = float(nmea.horizontal_dil)
        gpgga["altitude"] = nmea.altitude
        gpgga["altitude_units"] = nmea.altitude_units
        gpgga["undulation"] = float(nmea.geo_sep) if nmea.geo_sep != '' else 0.0
        gpgga["undulation_units"] = nmea.geo_sep_units
        gpgga["age"] = float(nmea.age_gps_data) if nmea.age_gps_data != '' else 0.0
        gpgga["differential_station_ID"] = nmea.ref_station_id
        # gpgga["checksum"] = nmea.checksum(msg)
        return gpgga

    def __parseGPRMC(self, nmea):
        gprmc = dict()
        gprmc["time_utc"]                     = nmea.timestamp.isoformat() if nmea.timestamp != None else ""
        gprmc["status"]                       = nmea.status
        gprmc["latitude"]                     = nmea.latitude
        gprmc["lat_direction"]                = nmea.lat_dir
        gprmc["longitude"]                    = nmea.longitude
        gprmc["long_direction"]               = nmea.lon_dir
        gprmc["knots"]                        = nmea.spd_over_grnd
        gprmc["degrees"]                      = float(nmea.true_course) if nmea.true_course != None else 0.0
        gprmc["date"]                         = nmea.datestamp.isoformat() if nmea.datestamp != None else ""
        gprmc["magnetic_variation"]           = float(nmea.mag_variation) if nmea.mag_variation != '' else 0.0
        gprmc["magnetic_variation_direction"] = nmea.mag_var_dir
        gprmc["mode"]                         = nmea.mode_indicator
        # gprmc["checksum"]                     = nmea.checksum(msg)
        return gprmc

    def __parseGPGLL(self, nmea):
        gpgll = dict()
        gpgll["latitude"]       = nmea.latitude
        gpgll["lat_direction"]  = nmea.lat_dir
        gpgll["longitude"]      = nmea.longitude
        gpgll["long_direction"] = nmea.lon_dir
        gpgll["time_utc"]       = nmea.timestamp.isoformat() if nmea.timestamp != None else ""
        gpgll["status"]         = nmea.status
        gpgll["mode"]           = nmea.faa_mode
        # gpgll["checksum"]       = nmea.checksum(msg)
        return gpgll

    def __parseGPGSA(self, nmea):
        gpgsa = dict()
        gpgsa["mode"]            = nmea.mode
        gpgsa["status"]          = int(nmea.mode_fix_type)
        gpgsa["satellite_id_1"]  = int(nmea.sv_id01) if nmea.sv_id01 != '' else 0
        gpgsa["satellite_id_2"]  = int(nmea.sv_id02) if nmea.sv_id02 != '' else 0
        gpgsa["satellite_id_3"]  = int(nmea.sv_id03) if nmea.sv_id03 != '' else 0
        gpgsa["satellite_id_4"]  = int(nmea.sv_id04) if nmea.sv_id04 != '' else 0
        gpgsa["satellite_id_5"]  = int(nmea.sv_id05) if nmea.sv_id05 != '' else 0
        gpgsa["satellite_id_6"]  = int(nmea.sv_id06) if nmea.sv_id06 != '' else 0
        gpgsa["satellite_id_7"]  = int(nmea.sv_id07) if nmea.sv_id07 != '' else 0
        gpgsa["satellite_id_8"]  = int(nmea.sv_id08) if nmea.sv_id08 != '' else 0
        gpgsa["satellite_id_9"]  = int(nmea.sv_id09) if nmea.sv_id09 != '' else 0
        gpgsa["satellite_id_10"] = int(nmea.sv_id10) if nmea.sv_id10 != '' else 0
        gpgsa["satellite_id_11"] = int(nmea.sv_id11) if nmea.sv_id11 != '' else 0
        gpgsa["satellite_id_12"] = int(nmea.sv_id12) if nmea.sv_id12 != '' else 0
        gpgsa["pdop"]            = float(nmea.pdop)
        gpgsa["hdop"]            = float(nmea.hdop)
        gpgsa["vdop"]            = float(nmea.vdop)
        # gpgsa["checksum"]        = nmea.checksum(msg)
        return gpgsa

    def __parseGPGSV(self, nmea):
        gpgsv =  dict()
        gpgsv["total_message_num"]      = int(nmea.num_messages)
        gpgsv["message_number"]         = int(nmea.msg_num)
        gpgsv["total_satellite_number"] = int(nmea.num_sv_in_view)
        gpgsv["satellite_number_1"]     = int(nmea.sv_prn_num_1) if nmea.sv_prn_num_1 != '' else 0
        gpgsv["elevation_1"]            = float(nmea.elevation_deg_1) if nmea.elevation_deg_1 != '' else 0.0
        gpgsv["azimuth_1"]              = float(nmea.azimuth_1) if nmea.azimuth_1 != '' else 0.0
        gpgsv["sn_1"]                   = float(nmea.snr_1) if nmea.snr_1 != '' else 0.0
        gpgsv["satellite_number_2"]     = int(nmea.sv_prn_num_2) if nmea.sv_prn_num_2 != '' else 0
        gpgsv["elevation_2"]            = float(nmea.elevation_deg_2) if nmea.elevation_deg_2 != '' else 0.0
        gpgsv["azimuth_2"]              = float(nmea.azimuth_2) if nmea.azimuth_2 != '' else 0.0
        gpgsv["sn_2"]                   = float(nmea.snr_2) if nmea.snr_2 != '' else 0.0
        gpgsv["satellite_number_3"]     = int(nmea.sv_prn_num_3) if nmea.sv_prn_num_3 != '' else 0
        gpgsv["elevation_3"]            = float(nmea.elevation_deg_3) if nmea.elevation_deg_3 != '' else 0.0
        gpgsv["azimuth_3"]              = float(nmea.azimuth_3) if nmea.azimuth_3 != '' else 0.0
        gpgsv["sn_3"]                   = float(nmea.snr_3) if nmea.snr_3 != '' else 0.0
        gpgsv["satellite_number_4"]     = int(nmea.sv_prn_num_4) if nmea.sv_prn_num_4 != '' else 0
        gpgsv["elevation_4"]            = float(nmea.elevation_deg_4) if nmea.elevation_deg_4 != '' else 0.0
        gpgsv["azimuth_4"]              = float(nmea.azimuth_4) if nmea.azimuth_4 != '' else 0.0
        gpgsv["sn_4"]                   = float(nmea.snr_4) if nmea.snr_4 != '' else 0.0
        # gpgsv["checksum"]               = nmea.checksum(msg)
        return gpgsv

    def __parseGPVTG(self, nmea):
        gpvtg = dict()
        gpvtg["track"]               = float(nmea.true_track) if nmea.true_track != None else 0.0
        gpvtg["track_mode"]          = nmea.true_track_sym
        gpvtg["magnetic_track"]      = float(nmea.mag_track) if nmea.mag_track != None else 0.0
        gpvtg["magnetic_track_mode"] = nmea.mag_track_sym
        gpvtg["ground_speed_knots"]  = float(nmea.spd_over_grnd_kts) if nmea.spd_over_grnd_kts != None else 0.0
        gpvtg["knots"]               = nmea.spd_over_grnd_kts_sym
        gpvtg["ground_speed_kmh"]    = float(nmea.spd_over_grnd_kmph) if nmea.spd_over_grnd_kmph != None else 0.0
        gpvtg["knots"]               = nmea.spd_over_grnd_kmph_sym
        gpvtg["mode"]                = nmea.faa_mode
        # gpvtg["checksum"]            = nmea.checksum(msg)
        return gpvtg
