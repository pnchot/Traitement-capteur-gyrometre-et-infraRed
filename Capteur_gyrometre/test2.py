#!/usr/bin/python

from datetime import datetime
from time import sleep

from altimu import AltIMU
imu = AltIMU()
imu.enable()
#data = []
imu.calibrateGyroAngles()

#for x in range(1000):
#    startTime = datetime.now()
#    angles = imu.trackGyroAngles(deltaT = 0.0002)

#print angles

start = datetime.now()

while(1):
    stop = datetime.now() - start
    #start = datetime.now()
    deltaT = stop.microseconds/1000000.0
    print " "
    print "Loop:", deltaT
    print "Accel:", imu.getAccelerometerAngles()
    print "Gyro:", imu.trackGyroAngles(deltaT = deltaT)
    print "AngleCouple",imu.getComplementaryAngles(deltaT = deltaT)
    #data.append(imu.getComplementaryAngles(deltaT = deltaT))
    print "KalmanAngles",imu.getKalmanAngles(deltaT = deltaT)
    sleep(0.1)

#angleX = (1/len(data))*sum(data[i][0] for i in range(len(data))
#angleY = (1/len(data))*sum(data[i][1] for i in range(len(data))
#angleZ = (1/len(data))*sum(data[i][2] for i in range(len(data))
#print "AngleMoyX =", angleX 
#print "AngleMoyY =", angleY 
#print "AngleMoyZ =", angleZ
