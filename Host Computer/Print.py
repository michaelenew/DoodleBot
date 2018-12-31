# wait for 'r', send parameters or send 'n', wait for 'r', send operation
# o: wait for printer values, store, and write to file
# s: go to sensor testing
# p: run pingpong







import serial
import time
import msvcrt
import struct
from pygame.locals import *
import pygame, pygame.event, sys
import inputbox
import fileinput
import numpy as np
import os.path


#progress bar

pygame.init()

black = [0, 0, 0]
#black = [255, 255, 255]
green = [ 0, 255, 0]

#green = [ 128, 0, 0]

smallfont = pygame.font.SysFont("comicsansms",25)

display_width = 1025
display_height = 500

size = [display_width, display_height]
screen = pygame.display.set_mode(size)

clock = pygame.time.Clock()

progress = 0

def text_objects(text, color, size):
	if size == "small":
		textSurface = smallfont.render(text, True, color)
	return textSurface, textSurface.get_rect()
	
def loading(progress):
	if progress < 100:
		text = smallfont.render("Printing:" + str(int(progress))+"%",True,green)
	else:
		text = smallfont.render("Printing:" + str(100)+"%",True,green)
	
	screen.blit(text, [453, 273])

def message_to_screen(msg, color, y_displace =0,  size = "small"):
	textSurf, textRect = text_objects(msg, color,size)
	textRect.center = (display_width/2), (display_height/2) + y_displace
	screen.blit(textSurf, textRect)

# create the pygame clock
clock = pygame.time.Clock()	

#check resin supplt
paul = 0;	
with open('resinSupply.txt','r') as y:
	for line in y:
		for ian in line.split():
			resin = float(ian)


#Get filename
#answer = "jim"
#while(answer == "jim"):
answer = inputbox.ask(screen, "Filename")
	
	

# Creates numpy matrix from text file

Matrix = np.loadtxt(answer, dtype='float', comments='#', delimiter= None, converters=None, skiprows=1, usecols=None, unpack=False, ndmin=2)
# w = size col
# h = size row

matrixSize = Matrix.shape
w = matrixSize[1]
h = matrixSize[0]


print("Values to be sent:")
print(Matrix)
print("Preparing to send values...")

#Estimate Print Time
totTime = 0;
for g in range(0,h):
	totTime = totTime + Matrix[g][5]
estTime = totTime/60000	
rem = 60*(estTime - int(estTime))

resinLoss = totTime*6.266e-7 #(Total time spent extruding)* (extrusion rate)

stopRequest = ''

if resinLoss > resin:
	print("Error: Print Exceeds Resin Supply! Print Canceled.")
	resinLoss = 0
else:
	estMessage = "Estimated print time " + str(int(estTime)) + " minutes and " + str(int(rem)) + " seconds."
	

	#Setup Serial communication of matrix from python to Arduino
	print ("Opening Serial port...")
	arduino = serial.Serial("COM5", 250000,timeout = 1)
	time.sleep(1)

	print ("Initialise Complete")
	
	
	# wait for arduino to be ready then send config
	while (arduino.in_waiting == 0):
		if (arduino.in_waiting > 0):
			break
			
	# If config.txt exists, send it to the printer
	if os.path.isfile("config.txt"):
		default = 'f' # from file flag
		arduino.write(default.encode())
		configparams = np.loadtxt("config.txt", dtype = 'float', comments = '//', delimiter = None, converters = None, skiprows = 0, usecols = 1, unpack = False, ndmin = 1)
		print(configparams)
		config = ''
		for i in range(0,len(configparams)):
			config += '%s,' % configparams[i]
		arduino.write(config.encode())
	else:
		default = 'd' # use default parameters flag
		arduino.write(default.encode())
		
	arduino.flush()
	arduino.reset_input_buffer()
	arduino.reset_output_buffer()
	
	while (arduino.in_waiting == 0):
		if (arduino.in_waiting > 0):
			break
	
	op = 'p'
	arduino.write(op.encode())
	
	arduino.flush()
	arduino.reset_input_buffer()
	arduino.reset_output_buffer()
	
	M = Matrix

	time.sleep(.1)
	trackTime = 0
	escape = 0
#perform operations for every row in matrix
	for j in range(0,h):
	#Package values in matrix for sending
		command = ''
		for i in range(0,w):
			command += '%s,' % M[j][i]
	#Detect if user wants to stop the print	
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				stopRequest = 'o'
			if event.type == pygame.KEYDOWN:
				if event.key == pygame.K_c:
					stopRequest = 'c'
				if event.key == pygame.K_o:
					stopRequest = 'o'
		
		
		if stopRequest == 'c':
			break
		elif stopRequest == 'o':
			command = '0,0,0,90,0,0,'
			print(command)
			arduino.write(command.encode())

			arduino.flush()
			arduino.reset_input_buffer()
			arduino.reset_output_buffer()
			break
		else:
			print (command)
		
	#Send row of matrix
	
		arduino.write(command.encode())

		arduino.flush()
		arduino.reset_input_buffer()
		arduino.reset_output_buffer()

	
	#Start Progress Bar
	
		if j < h:
			trackTime = trackTime + M[j][5]
	
		progress = 100*trackTime/totTime
		print (progress)
	
		screen.fill(black)
		pygame.draw.rect(screen,green,[423,223,204,49])
		pygame.draw.rect(screen,black,[424,224,202,47])
	
		if progress < 100:
			pygame.draw.rect(screen,green,[425,225,progress*2,45])
		else:
			pygame.draw.rect(screen,green,[425,225,200,45])
	
	
		loading(progress)
		message_to_screen(estMessage, green, y_displace =display_height/4,  size = "small")
		pygame.display.flip()
		time.sleep(0.1)
	#End Progress Bar
	
		print(arduino.in_waiting)
		while (arduino.in_waiting == 0):
			if (arduino.in_waiting > 0):
				break
	
	arduino.close()