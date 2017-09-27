# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt

from numpy import genfromtxt

data = genfromtxt('F.csv', delimiter=',')

plt.plot(data)

plt.show()