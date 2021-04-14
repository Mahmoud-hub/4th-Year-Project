import pandas as pd
import numpy as np
from numpy import where
from sklearn import svm
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split

# Set last index to be read
testCase = 199999

#Seperate the data
sample = pd.read_csv("sample2.csv", sep=':')
time = sample.iloc[:testCase, 0]
ID = sample.iloc[:testCase, 1]
y = sample.iloc[:testCase, 2].str.split("\\", n = 1, expand = True)[0]

# Reshape the data to be 2D in order to train and test the data
time_sh = np.asanyarray(time)
y_sh = np.asanyarray(y)
time_sh1 = time_sh.reshape(-1, 1)
y_sh1 = y_sh.reshape(-1, 1)

# Split x and y data into half training and half testing data
x_train, x_test, y_train, y_test = train_test_split(time_sh1, y_sh1, test_size = 0.50)

# Apply one class classification algorithm with linear fit and 5% error margin
oneclass = svm.OneClassSVM(kernel='linear', nu=0.05)
oneclass.fit(x_train, y_train)
# Predict outliers
pred_X = oneclass.predict(x_test)
pred_Y = oneclass.predict(y_test)

# Count number of outliers
unique, counts = np.unique(pred_Y, return_counts= True)
print(np.asarray((unique, counts)).T)

# Plot the data in scatter form
plt.scatter(time, y)
plt.title('Normal Data Sample V1')
plt.xlabel('Time')
plt.ylabel('Throttle and Breaking Percentage')
plt.ylim(60, 0)
plt.show()
