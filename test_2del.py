import numpy as np
import matplotlib.pyplot as plt



### filename to change
filename = "bmp_test.csv"

file = np.genfromtxt(filename, delimiter = ', ', dtype = None)

x = abs(file[:,2])

y = x.copy()

z = np.zeros((1000,1))
x *= 90 * 9.81

for i in range(1000):
    z[i] = z[i-1]
    if(x[i] < 0):
        x[i] = 0
    else:
        z[i] += (x[i]/3600)

fig, (p1, p2, p3) = plt.subplots(3)



p1. plot(np.arange(0, 1000), y)
p2.plot(np.arange(0, 1000), x)
p3.plot(np.arange(0, 1000), z)


p1.grid()
p2.grid()

plt.savefig("bmp_test.svg")
plt.show()
