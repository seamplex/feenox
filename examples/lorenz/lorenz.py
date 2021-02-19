import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

data = np.genfromtxt('lorenz.dat', delimiter='\t')

x = []
y = []
z = []

for i in range(len(data)):
  x.append(data[i][1])
  y.append(data[i][2])
  z.append(data[i][3])

fig = plt.figure()
ax = fig.gca(projection='3d')

ax.plot(x, y, z, lw=0.5)
ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_zlabel("z")

plt.show()
