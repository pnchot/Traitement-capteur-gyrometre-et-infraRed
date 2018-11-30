#!/usr/bin/python

from time import sleep

from lsm6ds33 import LSM6DS33

imu=LSM6DS33()
imu.enableLSM()

while True:
    print "Gyro:", imu.getGyroscopeRaw()
    print "Accelerometer:", imu.getAccelerometerRaw()
    sleep(0.2)
