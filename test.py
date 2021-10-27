import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

gpga = ""

while 1:
    try:
        text = ser.readline()
        text = str(text)
        data = text.split(",")
        if data[0] == "$GPGGA":
            gpgga.append(data[4])
            gpgga.append(data[6])

    except KeyboardInterrupt:
        break
