import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

gpgga = ""
#coordinates[][]

while 1:
    try:
        try:
            text = ser.readline()
        except:
            print("failed to read GPS")
        text = str(text)
        print(text)
        data = text.split(",")
        if data[0] == "$GPGGA":
            gpgga.append(data[2])
            gpgga.append(data[4])
            gpgga.append(data[9])
            coordinates.append(gpgga) #à retravailler

    except KeyboardInterrupt:
        break

