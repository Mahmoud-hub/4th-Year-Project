from numpy.random import seed
from numpy.random import randn
import matplotlib.pyplot as plt
import numpy as np
from numpy import mean
from numpy import std

#Read data from CSV file
file = open('Phase3Revised.csv', 'r')
lines = file.readlines()
temp = ''
X = []
Y = []
data = []
mal = []
ID = []

#Split each line using the ":" symbol
for line in lines:
	temp = line.split(":")
	X.append(temp[0])

	ID.append(temp[1])

	Y.append(temp[2])
	
	data = temp[3].split('\\')
	mal.append(data[0])

file.close()

#Parse data
temp_Dat = []
speed = []
BP = []
front_Sen = []
time = []
ID_data = []
full_data = []

for i in range(len(Y)):
	full_data.append(int(Y[i]))
	s = int(Y[i])
	t = int(X[i])
	d = int(ID[i])
	temp_Dat.append(s)
	time.append(t)
	ID_data.append(d)
		
#Seperate speed, braking percentage and front sensor status by checking ID numbers
for j in range(len(ID_data)):
	if ID_data[j] == 1:
		speed.append(temp_Dat[j])
	elif ID_data[j] == 2:
		BP.append(temp_Dat[j])
	else:
		front_Sen.append(temp_Dat[j])

# calculate summary statistics
data_mean, data_std = mean(full_data), std(full_data)
print('Mean: %f' % data_mean)
print('Standard Deviation: %f' % data_std)

#score = (temp_Dat - data_mean)/data_std
#z = score.tolist()

# Set up range of allowable data
cut_off = data_std * 2
lower, upper = data_mean - cut_off, data_mean + cut_off

# identify outliers
outliers = [x for x in full_data if x < lower or x > upper]
print('Identified outliers: %d' % len(outliers))

# Identify normal data
non_outliers = []
ind = []
for x in range(len(full_data)): 
	if full_data[x] >= lower and full_data[x] <= upper:
		non_outliers.append(full_data[x])
		ind.append(ID[x])
		
print('Non-outlier observations: %d' % len(non_outliers))

#Counting number of malicious data
cnt = 0
for n in mal:
	if n == '1':
		cnt = cnt + 1

#print('Number of malicious messages injected: %d' %cnt)

#Check all messages if their ID numbers are out of order by
#checking adjacent message IDs. If the current message is out
#of order with adjacent IDs, add one to the count
mal_list = []
for i in range(len(non_outliers)):
	if i != 0 and i != len(non_outliers) - 1:
		if ind[i] == '1':
			if ind[i - 1] != '3' and ind[i + 1] != '2':
				mal_list.append(mal[i])
		elif ind[i] == '2':
			if ind[i - 1] != '1' and ind[i + 1] != '3':
				mal_list.append(mal[i])
		elif ind[i] == '3':
			if ind[i - 1] != '2' and ind[i + 1] != '1':
				mal_list.append(mal[i])
			
#Count number of false positives generated
fp_count = 0
for k in mal_list:
	if k == '0':
		fp_count = fp_count + 1

print('Number of false positives: %d' %fp_count)
print('Number of detected hidden malicious messages: %d' %len(mal_list))

#Calculate error percentage for malicious data detection
mal_err = (float(abs(len(mal_list) - cnt)) / cnt) * 100
print('Percentage of error for malicious data detection: %d' %mal_err)
			
#Plot all the data in scatter form				
plt.scatter(X, full_data)	
plt.title('Malicious Data Log V4 Plot')
plt.xlabel('Time')
plt.ylabel('Throttle and Breaking Percentage')
plt.show()
