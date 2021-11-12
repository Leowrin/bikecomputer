import numpy as np
import matplotlib.pyplot as plt



### filename to change
filename = "toDelete.csv"

file = np.genfromtxt(filename, delimiter = ', ', dtype = None)

(p1, p2) = plt.subplots(2)



p1.plot(file[:,5], file[:, 7])

p2.plot(np.arange(0,1000), file[:, 7])


plt.grid()

plt.savefig("toDelete.svg")
plt.show()
