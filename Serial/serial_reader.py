import serial
import time
from datetime import datetime

ser = serial.Serial('/dev/ttyACM0',9600,timeout = 1) #/dev/port

try:
  while 1:
    data=ser.readline()
    print (data)
    with open('DepthReadings.txt','a') as outfile: #fileName
      data=str(data)
      outfile.write(data)
      outfile.write('\n')

except:
  time.sleep(1)
  pass
