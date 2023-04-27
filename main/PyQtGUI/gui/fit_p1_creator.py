#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import pandas as pd
import numpy as np
from scipy.optimize import curve_fit

import fit_factory

class Pol1Fit:
    def __init__(self, p0, p1):
        self.p0 = p0
        self.p1 = p1

    # function defined by the user
    def pol1(self, x, p0, p1):
        return p0+p1*x

    # implementation of the fitting algorithm
    def start(self, x, y, xmin, xmax, fitpar, axis, fit_results):
        fitln = None
        if (fitpar[0] != 0.0):
            self.p0 = fitpar[0]
        else:
            self.p0 = 100
        if (fitpar[1] != 0.0):
            self.p1 = fitpar[1]
        else:
            self.p1 = 10
        p_init = [self.p0, self.p1]
        popt, pcov = curve_fit(self.pol1, x, y, p0=p_init, maxfev=5000)

        # plotting fit curve and printing results
        try:
            x_fit = np.linspace(x[0],x[-1], 10000)
            y_fit = self.pol1(x_fit, *popt)

            fitln, = axis.plot(x_fit,y_fit, 'r-')
            for i in range(len(popt)):
                s = 'Par['+str(i)+']: '+str(round(popt[i],3))+'+/-'+str(round(pcov[i][i],3))
                fit_results.append(s)
        except:
            pass
        return fitln

class Pol1FitBuilder:
    def __init__(self):
        self._instance = None

    def __call__(self, p0=100, p1=10):
        if not self._instance:
            self._instance = Pol1Fit(p0, p1)
        return self._instance
