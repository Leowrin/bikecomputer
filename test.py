import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

coordinates = np.empty((0, 3))

filename = str(time.strftime("%Y_%m_%d_%H_%M", time.localtime())) + ".csv"


#def parse_gps(data):
    


while 1:
    try:
        try:
            text = ser.readline()
        except:
            print("failed to read GPS")

        text = str(text)
        data = text.split(",")
        print(text)
        try :
            tmp = float(data[5])
            
        except :
            tmp = 0
            
        if (data[0] == "$GPGGA" or data[0] == "b'$GPGGA") and tmp>0:
            print("YES")
            lat = float(data[2])
            lon = float(data[4])
            alt = float(data[9])

            print(np.shape(coordinates))
            coordinates = np.append(coordinates, [lat, lon, alt], axis=0) #append cree une copie, le = est necessaire. a mettre directement et reshape !
            print(coordinates)
        #else :
        #    print("no GPS signal or no GPGGA serial input")

    except KeyboardInterrupt:
        np.savetxt(filename)
        #terminer python et lancer C
        break
