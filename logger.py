import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np
import os
import math
import smbus
from bmp280 import BMP280

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

coordinates = np.empty((0, 3), dtype=float)

filename = str(time.strftime("%Y_%m_%d_%H_%M", time.localtime())) + ".csv"


#def parse_gps(data):

count=0

while count<1000 :

    try:
        try:
            text = ser.readline()
        except:
            print("failed to read GPS")

        text = str(text)
        if text.split(",")[0] == "$GPGGA" or text.split(",")[0] == "b'$GPGGA" :
            data = text.split(",")

            try :
                ### check nb satellites et coo
                satellites = int(data[7])
                float(data[2])

            except :
                satellites = 0

            ### précision vs sample rate, tmp>0 pour toutes valeurs, satellites > 3 pour précision max
            if satellites>3 :
                lat = float(data[2])/100
                lon = float(data[4])/100
                alt = float(data[9])


                tmp = lat - math.floor(lat)
                tmp /= 60
                lat = math.floor(lat)
                lat += tmp*100
                lat = round(lat, 7)

                tmp = lon - math.floor(lon)
                tmp /= 60
                lon = math.floor(lon)
                lon += tmp*100
                lon = round(lon, 7)


                coordinates = np.concatenate((coordinates, [[lat, lon, alt]]), axis=0)
                count+=1

        #else :
        #    print("no GPS signal or no GPGGA serial input")

    except KeyboardInterrupt:

        #terminer python et lancer C
        break
### save en csv

### %f ou %1.10f ?????????????????????????????????????????????????????????????????????????????????????
np.savetxt(filename, coordinates, fmt='%f', delimiter=',')
print("DONE")

### lancer le code C avec la longueur du fichier en argument. coordinates.shape[0]
# cmd = "./compute_info " + filename + " " + coordinates.shape[0]
# os.system("./compute_info")
#
# ### debug
# sleep(3)

exit()