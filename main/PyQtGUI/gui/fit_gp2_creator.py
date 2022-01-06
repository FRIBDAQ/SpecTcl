#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import pandas as pd
import numpy as np
from scipy.optimize import curve_fit

import fit_factory
        
class GPol2Fit:
    def __init__(self, amplitude, mean, standard_deviation, p0, p1, p2, f):
        self.amplitude = amplitude
        self.mean = mean
        self.standard_deviation = standard_deviation
        self.p0 = p0
        self.p1 = p1
        self.p2 = p2        
        self.f = f        
        
    # function defined by the user
    def gpol2(self, x, amplitude, mean, standard_deviation, p0, p1, p2, f):
        g = self.gauss(x, amplitude, mean, standard_deviation)
        pol2 = self.pol1(x,p0,p1,p2)
        return f*g+(1-f)*pol2
        
    # implementation of the fitting algorithm
    def start(self, x, y, xmin, xmax, axis, fit_results):
        fitln = None
        if (fitpar[0] != 0.0):
            self.amplitude = fitpar[0]
        else:
            self.amplitude = 1000
        if (fitpar[1] != 0.0):
            self.mean = fitpar[1]
        else:
            self.mean = xmin+(xmax-xmin)/2
        if (fitpar[2] != 0.0):
            self.standard_deviation = fitpar[2]
        else:
            self.standard_deviation = self.mean/10
        if (fitpar[3] != 0.0):
            self.p0 = fitpar[3]
        else:
            self.p0 = 100
        if (fitpar[4] != 0.0):
            self.p1 = fitpar[4]
        else:
            self.p1 = 10
        if (fitpar[5] != 0.0):
            self.p2 = fitpar[5]
        else:
            self.p2 = 10
        if (fitpar[6] != 0.0):
            self.f = fitpar[6]
        else:
            self.f = 0.9
        p_init = [amplitude, mean, standard_deviation, self.p0, self.p1, self.p2, self.f]
        popt, pcov = curve_fit(self.gpol2, x, y, p0=p_init, maxfev=5000)

        # plotting fit curve and printing results 
        try:
            x_fit = np.linspace(x[0],x[-1], 10000)
            y_fit = self.gauss(x_fit, *popt)

            fitln, = axis.plot(x_fit,y_fit, 'r-')
            for i in range(len(popt)):
                s = 'Par['+str(i)+']: '+str(round(popt[i],3))+'+/-'+str(round(pcov[i][i],3))
                fit_results.append(s)
        except:
            pass
        return fitln
        
class GPol2FitBuilder:
    def __init__(self):
        self._instance = None

    def __call__(self, amplitude=1000, mean=100, standard_deviation=10, p0=100, p1=10, p2=10, f=0.9):
        if not self._instance:
            self._instance = GPol2Fit(amplitude, mean, standard_deviation, p0, p1, p2, f)
        return self._instance
