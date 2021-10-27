import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

gpgga = []
coordinates = np.empty((0, 3))


while 1:
    gpgga = []
    try:
        try:
            text = ser.readline()
        except:
            print("failed to read GPS")
            
        text = str(text)
        data = text.split(",")
        #print(data)
        if data[0] == "$GPGGA" or data[0] == "b'$GPGGA":
            print("YES")
            gpgga.append(float(data[2]))
            gpgga.append(float(data[4]))
            gpgga.append(float(data[9]))
            
            #print(gpgga)
            print(np.shape(coordinates))
            print(np.shape(gpgga))
            coordinates = np.append(coordinates, gpgga, axis=0) #append cree une copie, le = est necessaire. a mettre directement et reshape !
            print(gpgga)
            
            print(coordinates)
        
        
    except KeyboardInterrupt:
        
        break