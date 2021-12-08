import numpy as np
import matplotlib.pyplot as plt
import sys


### filename, to change
filename = "computed_data.csv"
folder = str(sys.argv[1])


file = np.genfromtxt(folder+filename, delimiter = ', ', dtype = None)

csvLen = file.shape[0]


### power - energy, gps vs bmp280 vs bmp280 corrige
fig, (p1, p2) = plt.subplots(2)

p1.plot(np.arange(0,csvLen), file[:, 8], label="gps")
p1.plot(np.arange(0,csvLen), file[:, 11], label="bmp280")
p1.plot(np.arange(0,csvLen), file[:, 13], label="corrected_bmp280")

p2.plot(np.arange(0,csvLen), file[:, 9])
p2.plot(np.arange(0,csvLen), file[:, 12])
p2.plot(np.arange(0,csvLen), file[:, 14], label="total energy (height+resistance)")

plt.xlabel("time [s]")

p1.set_ylabel("power [w]")
p2.set_ylabel("energy [wh]")

p1.legend()

p1.grid()
p2.grid()

plt.savefig(folder + "energy_comparison.svg", format="svg", dpi=1200)
### plt.show()
plt.clf()


### deviation from reference point
plt.scatter(file[:, 0] - 2538365.5, file[:, 2] - 1151904.3, c=np.linspace(0, 1, csvLen), cmap="plasma")

plt.xlabel("Deviation relative to reference point, axis E [m]")
plt.ylabel("Deviation relative to reference point, axis N [m]")

plt.gca().set_aspect('equal', adjustable='box')
plt.grid()

plt.savefig(folder + "gps_deviation.svg", format="svg", dpi=1200)
### plt.show()
plt.clf()


### deltaH from gps and bmp280
y1 = file[:,7]
y2 = file[:,10]

plt.plot(np.arange(0,csvLen), y1, label="GPS derived deltaH")
plt.plot(np.arange(0,csvLen), y2, label="bmp280 derived deltaH")


limit = np.amax(np.absolute(np.concatenate((y1, y2))))
plt.ylim(-limit, limit)

plt.grid()



plt.xlabel("Time [s]")
plt.ylabel("Altitude variation from reference point [m]")
plt.legend()
plt.savefig(folder + "deltaH.svg", format="svg", dpi=1200)
### plt.show()
plt.clf()