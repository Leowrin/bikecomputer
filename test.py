import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

coordinates = np.empty((0, 3))

filename = str(time.strftime("%Y_%m_%d_%H_%M", time.localtime())) + ".csv"

while 1:
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
            lat = float(data[2])
            lon = float(data[4])
            alt = float(data[9])

            #print(gpgga)
            print(np.shape(coordinates))
            coordinates = np.append(coordinates, [lat, lon, alt], axis=0) #append cree une copie, le = est necessaire. a mettre directement et reshape !
            print(coordinates)


    except KeyboardInterrupt:
        np.savetxt(filename)
        break
