import serial

serialPort = "/dev/ttyACM1"
serialBaud = 9600
fileName="sample.csv"
samples = 20

#Connecting to the transmission
#try:
s = serial.Serial(serialPort, serialBaud) 
file = open(fileName, "a")
#except:
#	print("Inital setup Fail")
#	quit()

	
#Data collection
line = 0
while line <= samples:
	getData = str(s.readline())
	data = getData[2:][:-1]
	print(data)

	file.write(data)
	file.write('\n')
	line = line + 1 

#file close 
print("Script Complete")
file.close()
