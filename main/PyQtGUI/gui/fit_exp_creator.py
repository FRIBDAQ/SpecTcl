#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import pandas as pd
import numpy as np
from scipy.optimize import curve_fit

import fit_factory
        
class ExpFit:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c
        
    # function defined by the user
    def exp(self, x, a, b, c):
        return a+b*np.exp(x*c)
        
    # implementation of the fitting algorithm
    def start(self, x, y, xmin, xmax, axis, fit_results):
        fitln = None
        p_init = [self.a, self.b, self.c]
        popt, pcov = curve_fit(self.exp, x, y, p0=p_init, maxfev=5000)

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

class ExpFitBuilder:
    def __init__(self):
        self._instance = None

    def __call__(self, a=1, b=5, c=-1):
        if not self._instance:
            self._instance = ExpFit(a, b, c)
        return self._instance
