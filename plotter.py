import numpy as np
import matplotlib.pyplot as plt



### filename to change
filename = "toDelete.csv"

file = np.genfromtxt(filename, delimiter = ', ', dtype = None)



plt.plot(file[:,6], file[:, 5])

plt.grid()

plt.savefig("toDelete.svg")
plt.show()
