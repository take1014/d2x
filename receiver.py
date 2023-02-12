import sys
import serial
import datetime

ser = serial.Serial("/dev/ttyUSB0")
#ser.flushInput()
#ser.flushOutput()
idx = 0

nmea_data = []

ser.readline()

filename = datetime.datetime.utcnow().strftime("data/gps_data_%Y%m%d-%H%M%s.log")
print(filename)
while True:
    idx += 1
    print(idx)
    nmea_sentence = ser.readline()

    try:
        nmea_sentence = nmea_sentence.decode("utf-8")
        assert(nmea_sentence[0] == '$')

        if "GNRMC" in nmea_sentence:
            print(nmea_sentence)
            f = open(filename, mode="a")
            # f.write(nmea_sentence.decode("utf-8"))
            f.write(nmea_sentence)
            f.close()
    except:
        continue
        #print(str(nmea_sentence))
        #f = open(filename, mode="ab")
        ##f.write("====================================\n")
        #f.write(nmea_sentence)
        #f.close()
ser.close()
