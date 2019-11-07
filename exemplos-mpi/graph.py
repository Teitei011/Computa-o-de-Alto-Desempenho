# -*- coding: utf-8 -*-
"""
Created on Wed Oct 23 20:45:12 2019

@author: stefa
"""

import matplotlib.pyplot as plt
import numpy as np

image = np.loadtxt("teste.txt")


plt.plot(image.any())
plt.savefig("heat_image.png")