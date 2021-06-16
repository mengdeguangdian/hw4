import time
import serial

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

#s.write("/park/run 30 30 south\n".encode())
s.write("/park/run 20 20 north\n".encode())
#s.write("/park/run 20 20 east\n".encode())
#s.write("/park/run 30 30 west\n".encode())

#line = input()
#print(line)
#d1 = int(float(line))
#print(d1)

#line = input()
#print(line)
#d2 = int(float(line))
#print(d2)

#line = input()
#if line ==  'south':
#    print("south")            
#    print ("right")
#    s.write("/turn/run 100 -0.15 \n".encode())
#    time.sleep(1.27)
#    s.write("/stop/run \n".encode())
#    time.sleep(1)

#    print("up")
#    s.write("/goStraight/run 100 \n".encode())
#    time1 = (d2-7)*0.06
#    time.sleep(time1)
#    s.write("/stop/run \n".encode())
#    time.sleep(1)
    
#    print("left")
#    s.write("/turn/run 100 0.15 \n".encode())
#    time.sleep(1.24)
#    s.write("/stop/run \n".encode())
#    time.sleep(1)

#    print("up")
#    s.write("/goStraight/run 100 \n".encode())
#    time2 = (d1-7)*0.06
#    time.sleep(time2)

#    s.write("/stop/run \n".encode())