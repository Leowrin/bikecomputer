#import RPi.GPIO as GPIO #uncomment on rPi
#import serial
import time

ser = serial.Serial("/dev/Serial0") ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports
ser.baudrate = 9600
while 1:
    try:
        data = ser.read()
    except KeyboardInterrupt:
        break
