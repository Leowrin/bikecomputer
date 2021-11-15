import numpy as np
import matplotlib.pyplot as plt



### filename to change
filename = "toDelete.csv"

file = np.genfromtxt(filename, delimiter = ', ', dtype = None)

fig, (p1, p2) = plt.subplots(2)



p1.plot(file[:,5], file[:, 8])

p2.plot(file[:,5], file[:, 9])


p1.grid()
p2.grid()

plt.savefig("toDelete.svg")
plt.show()
