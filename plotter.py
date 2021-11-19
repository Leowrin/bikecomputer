import numpy as np
import matplotlib.pyplot as plt



### filename to change
filename = "test_gps.csv"

file = np.genfromtxt(filename, delimiter = ', ', dtype = None)

csvLen = file.shape[0]

fig, (p1, p2) = plt.subplots(2)



p1.plot(np.arange(0,1000), file[:, 8], label="gps")
p1.plot(np.arange(0,1000), file[:, 11], label="bmp280")

p2.plot(np.arange(0,1000), file[:, 9])
p2.plot(np.arange(0,1000), file[:, 12])

plt.xlabel("time [s]")

p1.set_ylabel("power [w]")
p2.set_ylabel("energy [wh]")

p1.legend()

p1.grid()
p2.grid()

plt.savefig("unreliability_gps_power.svg")
plt.show()



"""
plt.plot(file[:, 0] - 2538365.5, file[:, 2] - 1151904.3)

plt.xlabel("Deviation relative to reference point, axis E [m]")
plt.ylabel("Deviation relative to reference point, axis N [m]")

plt.gca().set_aspect('equal', adjustable='box')
plt.grid()

plt.savefig("historique/gps_relative_deviation.svg")
plt.show()
"""


"""
y1 = file[:,7]
y2 = file[:,10]

plt.plot(np.arange(0,csvLen), y1, label="GPS derived deltaH")
plt.plot(np.arange(0,csvLen), y2, label="bmp280 derived deltaH")


limit = np.amax(np.concatenate((y1, y2)))
plt.ylim(-limit, limit)

plt.grid()



plt.xlabel("Time [s]")
plt.ylabel("Altitude variation from reference point [m]")
plt.legend()
plt.savefig("gps_bmp.svg")
plt.show()
"""