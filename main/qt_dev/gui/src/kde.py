#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import json
import httplib2
import pandas as pd

import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import seaborn as sns
from scipy import stats
import statsmodels.api as sm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *
import CPyConverter as cpy

'''
df = pd.DataFrame({'x':[1.,2.],'weight':[2,4]})
weighted = sm.nonparametric.KDEUnivariate(df.x)
noweight = sm.nonparametric.KDEUnivariate(df.x)
weighted.fit(fft=False, weights=df.weight)
noweight.fit()

f, (ax1, ax2) = plt.subplots(1, 2, sharey=True)
ax1.plot(noweight.support, noweight.density)
ax2.plot(weighted.support, weighted.density)

ax1.set_title('No Weight')
ax2.set_title('Weighted')

y = np.array(weighted.density, np.float)
y /= np.sum(y)

print(weighted.support, y, weighted.density.sum())
ax2.plot(weighted.support, y)
print(y.sum())
'''

xmin = 0
xmax = 4000
bins = 10

def create_range(bins, vmin, vmax):
    x = []
    step = (vmax-vmin)/bins
    for i in np.arange(vmin, vmax, step):
        x.append(i)
    return x

x0 = create_range(bins, xmin, xmax)
y0 = [0, 21311, 310, 276, 368, 544, 501, 429, 430, 394, 405, 139581]
y0 = y0[1:-1]

x = np.array(x0, dtype=np.float32)
y = np.array(y0, dtype=np.float32)

#Renormalise the sample weights
y = np.array(y, np.float)
y /= np.sum(y)

#Construct a KDE and plot it
weighted = sm.nonparametric.KDEUnivariate(x)
weighted2 = sm.nonparametric.KDEUnivariate(x)
weighted3 = sm.nonparametric.KDEUnivariate(x)
weighted4 = sm.nonparametric.KDEUnivariate(x)
weighted5 = sm.nonparametric.KDEUnivariate(x)
weighted6 = sm.nonparametric.KDEUnivariate(x)
weighted7 = sm.nonparametric.KDEUnivariate(x)
weighted8 = sm.nonparametric.KDEUnivariate(x)

# silverman bandwidth
weighted.fit(kernel='gau',
             fft=False,
             bw="silverman",
             gridsize=100,
             clip=(0,4000),
             weights=y)

weighted2.fit(kernel='tri',
              fft=False,
              bw="silverman",
              gridsize=100,
              clip=(0,4000),
              weights=y)

weighted3.fit(kernel='triw',
              fft=False,
              bw="silverman",
              gridsize=100,
              clip=(0,4000),
              weights=y)

weighted4.fit(kernel='epa',
              fft=False,
              bw="silverman",
              gridsize=100,
              clip=(0,4000),
              weights=y)

# scott bandwidth
weighted5.fit(kernel='gau',
              fft=False,
              bw="scott",
              gridsize=100,
              clip=(0,4000),
              weights=y)

weighted6.fit(kernel='tri',
             fft=False,
             bw="scott",
             gridsize=100,
             clip=(0,4000),
             weights=y)

weighted7.fit(kernel='triw',
             fft=False,
             bw="scott",
             gridsize=100,
             clip=(0,4000),
             weights=y)

weighted8.fit(kernel='epa',
             fft=False,
             bw="scott",
             gridsize=100,
             clip=(0,4000),
             weights=y)

f, (ax1, ax2) = plt.subplots(1, 2, sharey=True)
ax1.hist(x, bins, (xmin-2000, xmax+2000) , histtype='stepfilled', alpha=.2, color='k', label='histogram', weights=y)
y1 = np.array(weighted.density, np.float)
y1 /= np.sum(y1)
ax1.plot(weighted.support, y1, lw=3, color='r', label='Gaussian Kernel - Silverman bw')
y2 = np.array(weighted2.density, np.float)
y2 /= np.sum(y2)
ax1.plot(weighted2.support, y2, lw=3, color='b', label='Triangular Kernel - Silverman bw')
y3 = np.array(weighted3.density, np.float)
y3 /= np.sum(y3)
ax1.plot(weighted3.support, y3, lw=3, color='g', label='Triweight Kernel - Silverman bw')
y4 = np.array(weighted.density, np.float)
y4 /= np.sum(y4)
ax1.plot(weighted4.support, y4, lw=3, color='violet', label='Epanechnikov Kernel - Silverman bw')
ax1.legend()
'''
ax3.plot(weighted5.support, weighted5.density, color='r', label='Gaussian Kernel - Scott bw')
ax3.plot(weighted6.support, weighted6.density, color='b', label='Triangular Kernel - Scott bw')
ax3.plot(weighted7.support, weighted7.density, color='g', label='Triweight Kernel - Scott bw')
ax3.plot(weighted8.support, weighted8.density, color='violet', label='Epanechnikov Kernel - Scott bw')
ax3.legend()
'''

'''
v = []
for i in range(total):
    v.append(np.random.choice(weighted.support, p=weighted.density))
ax1.hist(v, len(v), (xmin-2000, xmax+2000), histtype='stepfilled', alpha=.2, color='r')
'''


plt.show()
    
