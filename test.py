import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

while 1:
    try:
        text = ser.readline()
        text = str(text)
        print(type(text))
        data = text.split(",")
        print(text)
#         if data[0] == "$GPGGA":
#             pass

    except KeyboardInterrupt:
        break
