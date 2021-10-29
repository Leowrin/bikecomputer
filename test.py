import RPi.GPIO as GPIO #uncomment on rPi
import serial
import time
import numpy as np

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1) ###https://pyserial.readthedocs.io/en/latest/shortintro.html#opening-serial-ports

coordinates = np.empty((0, 3), dtype=float)

filename = str(time.strftime("%Y_%m_%d_%H_%M", time.localtime())) + ".csv"


#def parse_gps(data):
    
count=0

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
                tmp = float(data[7])
                tmp1 = float(data[2])
            
            except :
                tmp = 0
            
            if tmp>0 :
                lat = float(data[2])
                lon = float(data[4])
                alt = float(data[9])

                
                coordinates = np.concatenate((coordinates, [[lat, lon, alt]]), axis=0)
                print(coordinates)
                count+=1
            
        #else :
        #    print("no GPS signal or no GPGGA serial input")

    except KeyboardInterrupt:
        
        #terminer python et lancer C
        break
    
np.savetxt(filename, coordinates, delimiter=',')
exit()