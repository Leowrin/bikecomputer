import numpy as np
import matplotlib.pyplot as plt
import sys


### args
filename = "computed_data.csv"
folder = str(sys.argv[1])


file = np.genfromtxt(folder+filename, delimiter = ', ', dtype = None)

csvLen = file.shape[0]


### plot mean power over 7 sec and energy use over time
fig, (p1, p2) = plt.subplots(2)

meanPPower = np.array([])
meanTPower =np.array([])

for i in range(1, csvLen-5):
    meanPPower = np.append(meanPPower, [(file[i-3, 11] + file[i-2, 11] + file[i-1, 11] + file[i, 11] + file[i+1, 11] + file[i+2, 11] + file[i+3, 11]) / 7])

for i in range(1, csvLen-5):
    meanTPower = np.append(meanTPower, [(file[i-3, 13] + file[i-2, 13] + file[i-1, 13] + file[i, 13] + file[i+1, 13] + file[i+2, 13] + file[i+3, 13]) / 7])


p1.plot(np.arange(3, csvLen - 3), meanPPower, label="Power output (slope only)")
p1.plot(np.arange(3, csvLen - 3), meanTPower, label="Total power output")

p2.plot(np.arange(0,csvLen), file[:, 12], label="Potential energy")
p2.plot(np.arange(0,csvLen), file[:, 14], label="Total energy")

plt.xlabel("Time [s]")

p1.set_ylabel("Power [w]")
p2.set_ylabel("Energy [wh]")

p1.legend()
p2.legend()

p1.grid()
p2.grid()

plt.savefig(folder + "power_energy.svg", format="svg", dpi=1200)
plt.clf()



### deviation from reference point
plt.scatter(file[:, 0] - file[0, 0], file[:, 2] - file[0, 2], c=np.linspace(0, 1, csvLen), cmap="plasma")

plt.xlabel("Deviation relative to reference point, axis E [m]")
plt.ylabel("Deviation relative to reference point, axis N [m]")
plt.gca().set_aspect('equal', adjustable='box')
plt.grid()

plt.savefig(folder + "gps_deviation.svg", format="svg", dpi=1200)
plt.clf()



### deltaH from gps and bmp280
y1 = file[:,7]
y2 = file[:,10]

plt.plot(np.arange(0,csvLen), y1, label="GPS derived deltaH")
plt.plot(np.arange(0,csvLen), y2, label="bmp280 derived deltaH")
plt.title("Variation in altitude across time")

limit = np.amax(np.absolute(np.concatenate((y1, y2))))
plt.ylim(-limit, limit)

plt.grid()
plt.xlabel("Time [s]")
plt.ylabel("Altitude variation from reference point [m]")
plt.legend()
plt.savefig(folder + "deltaH.svg", format="svg", dpi=1200)
plt.clf()



### altimetric profile
plt.plot(np.arange(0,csvLen), file[:,15])

plt.grid()
plt.title("Altitude variation from start point, across time")
plt.savefig(folder + "altimetric_profile.svg", format="svg")
plt.clf()
