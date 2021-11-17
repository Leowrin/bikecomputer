import numpy as np
import matplotlib.pyplot as plt



### filename to change
filename = "toDelete.csv"

file = np.genfromtxt(filename, delimiter = ', ', dtype = None)

"""
fig, (p1, p2) = plt.subplots(2)



p1.plot(np.arange(0,1000), file[:, 8])

p2.plot(np.arange(0,1000), file[:, 9])

plt.xlabel("time [s]")

p1.set_ylabel("power [w]")
p2.set_ylabel("energy [wh]")

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
