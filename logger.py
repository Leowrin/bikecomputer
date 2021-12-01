import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np
import os
import math
from smbus import SMBus ### RPI only
from bmp280 import BMP280 ### RPI only

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports
bmp280 = BMP280(i2c_dev=SMBus(1))

GPIO.setmode(GPIO.BOARD)
GPIO.setup(36, GPIO.IN, pull_up_down=GPIO.PUD_UP)

coordinates = np.empty((0, 4), dtype=float)

folder = "logs/"
folder += str(time.strftime("%Y_%m_%d_%H_%M", time.localtime())) + '/'
filename = "logged_data.csv"

try :
    cmd = "mkdir " + folder
    os.system(cmd)

except :
    cmd = "mkdir -p " + folder
    os.system(cmd)

#def parse_gps(data):

count = 0
lat = 0.0
lon = 0.0
alt = 0.0

### debug
bmp280.get_pressure()

while count<10 :

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

            ### précision vs sample rate, tmp>0 pour toutes valeurs, satellites > 3 pour précision
            if satellites>3 :
                lat = float(data[2])/100
                lon = float(data[4])/100
                alt = float(data[9])

                ### convertion from minutes' to decimal
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

            """else :
                ##### A ENVOYER si pas de signal GPS
                lat = None
                lon = 0"""

            pressure = bmp280.get_pressure() ### envoie donnees pression meme sans GPS

            coordinates = np.concatenate((coordinates, [[lat, lon, alt, pressure]]), axis=0)
            count+=1

        #else :
        #    print("no GPS signal or no GPGGA serial input")

    except KeyboardInterrupt:

        #terminer python et lancer C
        break
    
### save en csv
np.savetxt(folder+filename, coordinates, fmt='%f', delimiter=',')
print("DONE")

### lancer le code C avec la longueur du fichier en argument. coordinates.shape[0]
cmd = "./gps " + folder+filename + ' ' + str(coordinates.shape[0]) + ' ' + str(coordinates.shape[1]) + " > " + folder + "computed_data.csv"
os.system(cmd)

### debug
time.sleep(1)

os.system("python3 plotter.py " + folder)


exit()
