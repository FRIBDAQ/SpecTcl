#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import pandas as pd
import numpy as np
from scipy.optimize import curve_fit

import fit_factory
        
class GausFit:
    def __init__(self, amplitude, mean, standard_deviation):
        self.amplitude = amplitude
        self.mean = mean
        self.standard_deviation = standard_deviation
        
    # function defined by the user
    def gauss(self, x, amplitude, mean, standard_deviation):
        return amplitude*np.exp(-(x-mean)**2.0 / (2*standard_deviation**2))
        
    # implementation of the fitting algorithm
    def start(self, x, y, xmin, xmax, axis, fit_results):
        fitln =None
        amplitude = 2000
        mean = xmin+(xmax-xmin)/2
        standard_deviation = mean/10
        p_init = [amplitude, mean, standard_deviation]
        print(p_init)
        
        popt, pcov = curve_fit(self.gauss, x, y, p0=p_init, maxfev=5000)

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
        
class GausFitBuilder:
    def __init__(self):
        self._instance = None

    def __call__(self, amplitude = 1000, mean = 100, standard_deviation = 10):
        if not self._instance:
            self._instance = GausFit(amplitude, mean, standard_deviation)
        return self._instance
