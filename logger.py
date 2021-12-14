import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np
import os
import math
from smbus import SMBus ### RPI only
from bmp280 import BMP280 ### RPI only
import config

bmp280 = BMP280(i2c_dev=SMBus(config.i2c_bus))

GPIO.setmode(GPIO.BOARD)

led = 40

### setup les pin pour le record switch et la led temoin
GPIO.setup(config.recordSwitch_gpio, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(led, GPIO.OUT)

### necessaire pour reset le read buffer
ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)
ser.close()

def terminate(folder, filename, coordinates):
    try :
        print(folder+filename)
        np.savetxt(folder+filename, coordinates, fmt='%f', delimiter=',')
        ### print("DONE")

        ### lancer le code C avec la longueur du fichier en argument. coordinates.shape[0]
        cmd = "/home/pi/bikecomputer/gps " + folder+filename + ' ' + str(coordinates.shape[0]) + ' ' + str(coordinates.shape[1]) + " > " + folder + "computed_data.csv"
        os.system(cmd)

        ### debug
        time.sleep(1)

        os.system("python3 /home/pi/bikecomputer/plotter.py " + folder)

        ser.close()

        return 1

    except :
        return 0


count = 0
lat = 0.0
lon = 0.0
alt = 0.0

### debug
bmp280.get_pressure()


stateA = 0
stateB = 0


while 1 :
    while GPIO.input(config.recordSwitch_gpio) == 0 :
        stateB = 1
        text = None

        if stateA == 0 :
            ### debug
            bmp280.get_pressure()
            lat = 0.0
            lon = 0.0
            alt = 0.0

            ### start LED temoin
            GPIO.output(led, GPIO.HIGH)

            coordinates = np.empty((0, 4), dtype=float)

            folder = "/home/pi/bikecomputer/logs/"
            folder += str(time.strftime("%Y_%m_%d_%H_%M", time.localtime())) + '/'
            filename = "logged_data.csv"

            try :
                cmd = "mkdir " + folder
                os.system(cmd)

            except :
                try :
                    cmd = "mkdir -p " + folder
                    os.system(cmd)
                except :
                    pass
            stateA = 1

        try:
            ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)
        except:
            print("erreur init com serial0")

        try:
            text = ser.readline()
            text = str(text)

        except:
            print("failed to read GPS")
            text = None


        try :
            ### check nb satellites et coo
            satellites = int(data[7])
            float(data[2])

        except :
            satellites = 0



        if text.split(",")[0] == "$GPGGA" or text.split(",")[0] == "b'$GPGGA" :
            ### print(text)
            data = text.split(",")
        ### précision vs sample rate, tmp>0 pour toutes valeurs, satellites > 3 pour précision
            if satellites>3 :
                try:
                    lat = float(data[2])/100
                    lon = float(data[4])/100
                    alt = float(data[9])
                except:
                    lat = 0
                    lon = 0
                    alt = 0
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

            pressure = bmp280.get_pressure() ### envoie donnees pression meme sans GPS

            coordinates = np.concatenate((coordinates, [[lat, lon, alt, pressure]]), axis=0)
            print(count)
            count+=1

    if stateB == 1 :
        terminate(folder, filename, coordinates)
        stateB = 0
        count = 0
        GPIO.output(led, GPIO.LOW)

    stateA = 0
