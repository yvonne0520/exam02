import matplotlib.pyplot as plt
import numpy as np
import serial
import time
t = np.arange(0,100,1)

# x = np.arange(-1,1,0.02)
y = np.arange(0,5,0.05)
# z = np.arange(-1,1,0.02)
log = np.arange(0,1,0.01)
serdev = '/dev/ttyACM1'
s = serial.Serial(serdev,115200)
for i in range(1,100):
    # line=s.readline() # Read an echo string from K66F terminated with '\n'
    # x[i] = float(line)
    line_1=s.readline()
    y[i] = float(line_1)
    # line_2=s.readline()
    # z[i] = float(line_2)    
    line_3=s.readline()
    log[i] = float(line_3)

fig, ax = plt.subplots(2, 1)
# ax[0].plot(t,x,label='ACC_x')
ax[0].plot(t,y,label='distance')
# ax[0].plot(t,z,label='ACC_z')
ax[0].set_xlabel('Time')
ax[0].set_ylabel('DISTANCE')
ax[1].stem(t,log,use_line_collection = True) 
ax[1].set_xlabel('Time')
ax[1].set_ylabel('over5cm')
plt.show()
s.close()