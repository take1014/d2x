#!usr/bin/env python3
#-*- coding:utf-8 -*-
import sys
import pynmea2 as parser

class NMEAParser:
    def __init__(self, sentence=""):
        self.sentence = ""

    def parseNMEA(self, sentence:str):
        self.sentence = sentence
        nmea = parser.parse(sentence)
        if nmea.sentence_type == "RMC":
            parsed_sentence = self.__parseRMC(nmea)
        elif nmea.sentence_type == "VTG":
            parsed_sentence = self.__parseVTG(nmea)
        elif nmea.sentence_type == "GGA":
            parsed_sentence = self.__parseGGA(nmea)
        elif nmea.sentence_type == "GSA":
            parsed_sentence = self.__parseGSA(nmea)
        elif nmea.sentence_type == "GSV":
            parsed_sentence = self.__parseGSV(nmea)
        elif nmea.sentence_type == "GLL":
            parsed_sentence = self.__parseGLL(nmea)
        elif nmea.sentence_type == "ZDA":
            # not get GPZDA using VFAN
            parsed_sentence = None
        else:
            parsed_sentence = None

        # get nmea_type and calc checksum
        if parsed_sentence is not None:
            nmea_type = nmea.identifier().replace(',','')
            parsed_sentence["checksum"] = nmea.checksum(sentence)
        else:
            nmea_type = None

        return nmea_type, parsed_sentence

    def __parseGGA(self, nmea):
        # print(nmea.render)
        gga = dict()
        gga["time_utc"]       = nmea.timestamp.isoformat() if nmea.timestamp != None else ""
        gga["latitude"]       = nmea.latitude
        gga["lat_direction"]  = nmea.lat_dir
        gga["longitude"]      = nmea.longitude
        gga["long_direction"] = nmea.lon_dir
        gga["quality"]        = nmea.gps_qual
        gga["number_of_satellites"] = int(nmea.num_sats)
        gga["horizontal_dilution_of_precision"] = float(nmea.horizontal_dil)
        gga["altitude"] = nmea.altitude
        gga["altitude_units"] = nmea.altitude_units
        gga["undulation"] = float(nmea.geo_sep) if nmea.geo_sep != '' else 0.0
        gga["undulation_units"] = nmea.geo_sep_units
        gga["age"] = float(nmea.age_gps_data) if nmea.age_gps_data != '' else 0.0
        gga["differential_station_ID"] = nmea.ref_station_id
        # gpgga["checksum"] = nmea.checksum(msg)
        return gga

    def __parseRMC(self, nmea):
        rmc = dict()
        rmc["time_utc"]                     = nmea.timestamp.isoformat() if nmea.timestamp != None else ""
        rmc["status"]                       = nmea.status
        rmc["latitude"]                     = nmea.latitude
        rmc["lat_direction"]                = nmea.lat_dir
        rmc["longitude"]                    = nmea.longitude
        rmc["long_direction"]               = nmea.lon_dir
        rmc["knots"]                        = nmea.spd_over_grnd
        rmc["degrees"]                      = float(nmea.true_course) if nmea.true_course != None else 0.0
        rmc["date"]                         = nmea.datestamp.isoformat() if nmea.datestamp != None else ""
        rmc["magnetic_variation"]           = float(nmea.mag_variation) if nmea.mag_variation != '' else 0.0
        rmc["magnetic_variation_direction"] = nmea.mag_var_dir
        rmc["mode"]                         = nmea.mode_indicator
        # gprmc["checksum"]                     = nmea.checksum(msg)
        return rmc

    def __parseGLL(self, nmea):
        gll = dict()
        gll["latitude"]       = nmea.latitude
        gll["lat_direction"]  = nmea.lat_dir
        gll["longitude"]      = nmea.longitude
        gll["long_direction"] = nmea.lon_dir
        gll["time_utc"]       = nmea.timestamp.isoformat() if nmea.timestamp != None else ""
        gll["status"]         = nmea.status
        gll["mode"]           = nmea.faa_mode
        # gpgll["checksum"]       = nmea.checksum(msg)
        return gll

    def __parseGSA(self, nmea):
        gsa = dict()
        gsa["mode"]            = nmea.mode
        gsa["status"]          = int(nmea.mode_fix_type)
        gsa["satellite_id_1"]  = int(nmea.sv_id01) if nmea.sv_id01 != '' else 0
        gsa["satellite_id_2"]  = int(nmea.sv_id02) if nmea.sv_id02 != '' else 0
        gsa["satellite_id_3"]  = int(nmea.sv_id03) if nmea.sv_id03 != '' else 0
        gsa["satellite_id_4"]  = int(nmea.sv_id04) if nmea.sv_id04 != '' else 0
        gsa["satellite_id_5"]  = int(nmea.sv_id05) if nmea.sv_id05 != '' else 0
        gsa["satellite_id_6"]  = int(nmea.sv_id06) if nmea.sv_id06 != '' else 0
        gsa["satellite_id_7"]  = int(nmea.sv_id07) if nmea.sv_id07 != '' else 0
        gsa["satellite_id_8"]  = int(nmea.sv_id08) if nmea.sv_id08 != '' else 0
        gsa["satellite_id_9"]  = int(nmea.sv_id09) if nmea.sv_id09 != '' else 0
        gsa["satellite_id_10"] = int(nmea.sv_id10) if nmea.sv_id10 != '' else 0
        gsa["satellite_id_11"] = int(nmea.sv_id11) if nmea.sv_id11 != '' else 0
        gsa["satellite_id_12"] = int(nmea.sv_id12) if nmea.sv_id12 != '' else 0
        gsa["pdop"]            = float(nmea.pdop)
        gsa["hdop"]            = float(nmea.hdop)
        gsa["vdop"]            = float(nmea.vdop)
        # gpgsa["checksum"]        = nmea.checksum(msg)
        return gsa

    def __parseGSV(self, nmea):
        gsv =  dict()
        gsv["total_message_num"]      = int(nmea.num_messages)
        gsv["message_number"]         = int(nmea.msg_num)
        gsv["total_satellite_number"] = int(nmea.num_sv_in_view)
        gsv["satellite_number_1"]     = int(nmea.sv_prn_num_1) if nmea.sv_prn_num_1 != '' else 0
        gsv["elevation_1"]            = float(nmea.elevation_deg_1) if nmea.elevation_deg_1 != '' else 0.0
        gsv["azimuth_1"]              = float(nmea.azimuth_1) if nmea.azimuth_1 != '' else 0.0
        gsv["sn_1"]                   = float(nmea.snr_1) if nmea.snr_1 != '' else 0.0
        gsv["satellite_number_2"]     = int(nmea.sv_prn_num_2) if nmea.sv_prn_num_2 != '' else 0
        gsv["elevation_2"]            = float(nmea.elevation_deg_2) if nmea.elevation_deg_2 != '' else 0.0
        gsv["azimuth_2"]              = float(nmea.azimuth_2) if nmea.azimuth_2 != '' else 0.0
        gsv["sn_2"]                   = float(nmea.snr_2) if nmea.snr_2 != '' else 0.0
        gsv["satellite_number_3"]     = int(nmea.sv_prn_num_3) if nmea.sv_prn_num_3 != '' else 0
        gsv["elevation_3"]            = float(nmea.elevation_deg_3) if nmea.elevation_deg_3 != '' else 0.0
        gsv["azimuth_3"]              = float(nmea.azimuth_3) if nmea.azimuth_3 != '' else 0.0
        gsv["sn_3"]                   = float(nmea.snr_3) if nmea.snr_3 != '' else 0.0
        gsv["satellite_number_4"]     = int(nmea.sv_prn_num_4) if nmea.sv_prn_num_4 != '' else 0
        gsv["elevation_4"]            = float(nmea.elevation_deg_4) if nmea.elevation_deg_4 != '' else 0.0
        gsv["azimuth_4"]              = float(nmea.azimuth_4) if nmea.azimuth_4 != '' else 0.0
        gsv["sn_4"]                   = float(nmea.snr_4) if nmea.snr_4 != '' else 0.0
        # gpgsv["checksum"]               = nmea.checksum(msg)
        return gsv

    def __parseVTG(self, nmea):
        vtg = dict()
        vtg["track"]               = float(nmea.true_track) if nmea.true_track != None else 0.0
        vtg["track_mode"]          = nmea.true_track_sym
        vtg["magnetic_track"]      = float(nmea.mag_track) if nmea.mag_track != None else 0.0
        vtg["magnetic_track_mode"] = nmea.mag_track_sym
        vtg["ground_speed_knots"]  = float(nmea.spd_over_grnd_kts) if nmea.spd_over_grnd_kts != None else 0.0
        vtg["knots"]               = nmea.spd_over_grnd_kts_sym
        vtg["ground_speed_kmh"]    = float(nmea.spd_over_grnd_kmph) if nmea.spd_over_grnd_kmph != None else 0.0
        vtg["knots"]               = nmea.spd_over_grnd_kmph_sym
        vtg["mode"]                = nmea.faa_mode
        # gpvtg["checksum"]            = nmea.checksum(msg)
        return vtg