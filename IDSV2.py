from numpy.random import seed
from numpy.random import randn
import matplotlib.pyplot as plt
import numpy as np
from numpy import mean
from numpy import std

#Read data from CSV file
file = open('malsamp2.csv', 'r')
lines = file.readlines()
temp = ''
Y = []
X = []
ID = []

#Split each line using the ":" symbol
for line in lines:
	temp = line.split(":")
	X.append(temp[0])

	ID.append(temp[1])

	data = temp[2].split('\\')
	Y.append(data[0])

file.close()

#Parse data
temp_Dat = []
speed = []
breaking = []
time = []
ID2 = []

for i in range(len(Y)):
		s = int(Y[i])
		t = int(X[i])
		d = int(ID[i])
		temp_Dat.append(s)
		time.append(t)
		ID2.append(d)
		
#Seperate speed and braking percentage by checking ID numbers
for j in range(len(ID2)):
	if ID2[j] == 1:
		speed.append(temp_Dat[j])
	else:
		breaking.append(temp_Dat[j])

# calculate summary statistics
data_mean, data_std = mean(temp_Dat), std(temp_Dat)
print(data_mean)
print(data_std)

#score = (temp_Dat - data_mean)/data_std
#z = score.tolist()

# Set up range of allowable data
cut_off = data_std * 1.3
lower, upper = data_mean - cut_off, data_mean + cut_off

# Identify outliers
outliers = [x for x in temp_Dat if x < lower or x > upper]
print('Identified outliers: %d' % len(outliers))

# Identify Normal data
outliers_removed = [x for x in temp_Dat if x >= lower and x <= upper]
print('Non-outlier observations: %d' % len(outliers_removed))

# Plot data in scatter form
plt.scatter(X, temp_Dat)	
plt.title('Malicious Data Log V2 Plot')
plt.xlabel('Time')
plt.ylabel('Throttle and Breaking Percentage')
plt.show()
