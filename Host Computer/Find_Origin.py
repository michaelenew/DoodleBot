import serial
from pygame.locals import *
import pygame, pygame.event, sys
import fileinput
import numpy as np
import os.path
import time

print ("Opening Serial port...")
arduino = serial.Serial("COM5", 250000,timeout = 1)
time.sleep(1)
arduino.reset_input_buffer()
arduino.reset_output_buffer()

if os.path.isfile("config.txt"):
	default = 'f' # use parameters from file flag
	arduino.write(default.encode())
	configparams = np.loadtxt("config.txt", dtype = 'float', comments = '//', delimiter = None, converters = None, skiprows = 0, usecols = 1, unpack = False, ndmin = 1)
	print("Parameters to be sent:\n")
	print(configparams)
	
	config = ''
	for i in range(0,len(configparams)):
		config += '%s,' % configparams[i]
	
	while (arduino.in_waiting == 0):
		if (arduino.in_waiting > 0):
			break
	arduino.reset_input_buffer()
	arduino.reset_output_buffer()
	
	arduino.write(config.encode())
else:
	default = 'd' # use default parameters flag
	arduino.write(default.encode())

while (arduino.in_waiting == 0):
	if (arduino.in_waiting > 0):
		break

arduino.reset_input_buffer()
arduino.reset_output_buffer()

op = 'o'
arduino.write(op.encode())

num = [0 for x in range(0,3)]

while (arduino.in_waiting == 0):
	if (arduino.in_waiting > 0):
		break

indicator = 'r'
arduino.write(indicator.encode())
print('X mm to origin:')
print(arduino.readline())
arduino.write(indicator.encode())
print('Y mm to origin:')
print(arduino.readline())
arduino.write(indicator.encode())
print('Z mm to origin:')
print(arduino.readline())



#x = fileinput.input(files = "randparams.txt", inplace = 1)
#for line in x:
#	if "xmm2orig" in line:
#		line = 'xmm2orig        %.2f\n' % num[0]
#	elif "ymm2orig" in line:
#		line = 'ymm2orig        %.2f\n' % num[1]
#	elif "zmm2orig" in line:
#		line = 'zmm2orig        %.2f\n' % num[2]
#	 elif "th_orig_potval" in line:
#		line = 'th_orig_potval  %.2f\n' % num[3]
#	print line,
#x.close()