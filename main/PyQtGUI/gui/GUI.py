#!/usr/bin/env python3
import io
import pickle
import traceback, sys, os
sys.path.append(os.getcwd())
sys.path.append("./Lib")
sys.path.append(str(os.environ.get("INSTDIR"))+"/Lib")
import subprocess
import signal
import logging
import ctypes
from ctypes import *

from copy import copy
import json
import httplib2
import threading
import itertools
import time
import multiprocessing
import math
import re

from sklearn import metrics
from sklearn.cluster import KMeans
from sklearn.mixture import GaussianMixture
from sklearn.preprocessing import StandardScaler

import cv2

import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.lines as mlines
import matplotlib.mlab as mlab
import matplotlib.image as mpimg

from matplotlib.patches import Polygon, Circle, Ellipse
from matplotlib.path import Path
from scipy.optimize import curve_fit
from scipy.signal import find_peaks

from PyQt5 import QtCore, QtNetwork
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.gridspec as gridspec
from itertools import chain, compress, zip_longest
from mpl_toolkits.axes_grid1 import make_axes_locatable

import pandas as pd
import numpy as np
import CPyConverter as cpy

import algo_factory
import fit_factory

# import widgets
from MenuGUI import Menu
from Plot import Plot
from ConfigGUI import Configuration
from OutputGUI import OutputPopup
from Clustering import Cluster2D
from PeakFinder import PeakFinder
from logger import log, setup_logging, set_logger
from notebook_process import testnotebook, startnotebook, stopnotebook
from WebWindow import WebWindow

SETTING_BASEDIR = "workdir"
SETTING_EXECUTABLE = "exec"
DEBUG = False
debugPrints = False

DEBOUNCE_DUR = 0.25
t = None

class MainWindow(QMainWindow):

    stop_signal = pyqtSignal()

    def __init__(self, factory, fit_factory, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        self.factory = factory
        self.fit_factory = fit_factory        

        self.setWindowTitle("CutiePie")
        '''
        self.setWindowFlags(
            QtCore.Qt.Window |
            QtCore.Qt.CustomizeWindowHint |
            QtCore.Qt.WindowTitleHint |
            QtCore.Qt.WindowCloseButtonHint |
            QtCore.Qt.WindowStaysOnTopHint
        )
        '''

        #check if there are arguments or not
        try:
            self.args = dict(args)
            if (debugPrints):
                print("self.args",self.args)
        except:
            pass
        
        # global variables
        self.timer = QtCore.QTimer()
        self.originalPalette = QApplication.palette()

        # dictionaries for parameters
        self.param_list = {}
        self.nparams = 0
        # dataframe for spectra
        self.spectrum_list = pd.DataFrame()
        
        # dictionary for histogram
        self.h_dict = {}
        self.h_dict_output = {}  # for saving pane geometry

        # index of the histogram
        self.index = 0 # this one is for self-adding
        self.idx = 0 # this one is global
        
        # click selection of position in canvas
        self.selected_plot_index = None
        self.isSelected = False
        self.selected_row = None
        self.selected_col = None     
        
        # support lists
        self.h_dim = []
        self.h_lst = []
        self.h_lst_cb = {} # this is a dictionary because we have non-consecutive entries
        self.h_zoom_max = []
        self.binList = []
        self.axbkg = {}
        self.artist_dict = {}
        self.artist1D = {}
        self.artist2D = {}                        
        
        # for zooming 
        self.yhigh = 1024
        self.ylow = 0
        self.vmin = 0
        self.vmax = 1024        
        
        # configuration widget flag
        self.isHidden = False

        # flags
        self.isFull = False
        self.Start = False        
        self.forAll = False
        self.isLoaded = False
        self.isZoomed = False
        self.toCreateGate = False
        # tools for selected plots
        self.rec = None
        self.optionAll = False
        self.isCluster = False
        self.onFigure = False
        
        # dictionary for spectcl gates
        self.gate_dict = {}
        self.gateTypeDict = {}
        self.gateType = ""
        
        #dictionary for gates - key:gate name entry: line (with label)
        self.dict_region = {}
        self.counter = 0

        #self.polygon = mlines.Line2D([],[])
        self.listLine = []
        self.xs = []
        self.ys = []        
        self.xstart = 0
        self.ystart = 0        
        
        # editing gates
        self.showverts = True
        self.isPressed = False
        self._ind = None # the active vert
        self.last_vert_ind = None
        
        self.press = None
        self.new_poly_xy = []
        self.tmp = []
        self.pol2line = []
        self.points = []
        self.points_w = []        
        self.old_threshold = 0
        self.old_algo = ""
        self.dbres = None
        self.clusterpts = []
        self.clusterw = []                
        self.clusterptsW = []

        self.LISEpic = None

        self.peak_pos = {}
        self.peak_vl = {}
        self.peak_hl = {}                
        self.peak_txt = {}
        self.isChecked = {}
        
        self.max_ds = 10

        self.copied_line_attributes = ('xdata', 'ydata', 'animated', 'antialiased', 'color',  
                                       'dash_capstyle', 'dash_joinstyle', 
                                       'drawstyle', 'fillstyle', 'linestyle', 'linewidth',
                                       'marker', 'markeredgecolor', 'markeredgewidth', 'markerfacecolor',
                                       'markerfacecoloralt', 'markersize', 'markevery', 'pickradius',
                                       'solid_capstyle', 'solid_joinstyle', 'visible', 'zorder')
        
        #######################
        # Main layout GUI
        #######################        
        
        mainLayout = QVBoxLayout()
        mainLayout.setContentsMargins(0,0,0,0)
        mainLayout.setSpacing(0.5)
        
        # top menu
        self.wTop = Menu()
        self.wTop.setFixedHeight(100)
        
        # config menu
        self.wConf = Configuration()
        self.wConf.setFixedHeight(225)
        
        # plot widget
        self.wPlot = Plot()
        
        # gui composition
        mainLayout.addWidget(self.wTop)
        mainLayout.addWidget(self.wConf)
        mainLayout.addWidget(self.wPlot)        
        
        widget = QWidget()
        widget.setLayout(mainLayout)
        
        self.setCentralWidget(widget)

        # output popup window
        self.resPopup = OutputPopup()
        self.table_row = []

        # peak finder option window 
        self.pPopup = PeakFinder()

        # clustering 
        self.clPopup = Cluster2D()

        # initialize factory from algo_creator
        self.factory.initialize(self.clPopup.clusterAlgo)
        # initialize factory from fit_creator
        self.fit_factory.initialize(self.wConf.fit_list)
        
        #################
        # Signals
        #################

        # top menu signals
        self.wTop.exitButton.clicked.connect(self.closeAll)
        self.wTop.updateButton.clicked.connect(self.update)                
        self.wTop.slider.valueChanged.connect(self.self_update)

        # disable for the moment - serious bug to understand
        self.wTop.configButton.clicked.connect(self.configure)                        
        self.wTop.saveButton.clicked.connect(self.saveGeo)
        self.wTop.loadButton.clicked.connect(self.loadGeo)

        self.wTop.createGate.clicked.connect(self.createGate)
        self.wTop.createGate.setEnabled(False)
        self.wTop.editGate.clicked.connect(self.editGate)
        self.wTop.editGate.setEnabled(False)
        self.wTop.editGate.setToolTip("Key bindings for editing a gate:\n"
                                      "'t' enable dragging mode\n"
                                      "'i' insert vertex\n"
                                      "'d' delete vertex\n"
                                      "'u' update gate\n")
        self.wTop.deleteGate.clicked.connect(self.deleteGate)                        
        self.wTop.drawGate.clicked.connect(lambda: self.drawGate(self.optionAll))
        self.wTop.cleanGate.clicked.connect(self.clearGate)

        # configuration signals
        self.at_startup()

        self.wConf.histo_geo_row.activated.connect(lambda: self.initialize_canvas(int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_col.activated.connect(lambda: self.initialize_canvas(int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_add.clicked.connect(self.add_plot)

        self.wConf.histo_geo_update.clicked.connect(self.update_plot)        
        self.wConf.histo_geo_delete.clicked.connect(self.delete_plot)        
        self.wConf.button2D_option.activated.connect(self.change_bkg)        
        self.wConf.histo_geo_all.stateChanged.connect(self.applyAll)

        self.wConf.peak_option.clicked.connect(self.peakPopup)
        self.pPopup.peak_analysis.clicked.connect(self.analyzePeak)
        self.pPopup.peak_analysis_clear.clicked.connect(self.peakAnalClear)        

        self.wConf.jup_start.clicked.connect(self.jupyterStart)
        self.wConf.jup_stop.clicked.connect(self.jupyterStop)
        
        self.wConf.cluster_option.clicked.connect(self.clusterPopup)
        self.clPopup.threshold_slider.valueChanged.connect(self.thresholdFigure)
        self.clPopup.analyzerButton.clicked.connect(self.analyzeCluster)
        self.clPopup.loadButton.clicked.connect(self.loadFigure)
        self.clPopup.addButton.clicked.connect(self.addFigure)
        self.clPopup.deleteButton.clicked.connect(self.deleteFigure)
        self.clPopup.alpha_slider.valueChanged.connect(self.transFigure)
        self.clPopup.zoomX_slider.valueChanged.connect(self.zoomFigureX)
        self.clPopup.zoomY_slider.valueChanged.connect(self.zoomFigureY)        
        self.clPopup.joystick.mousemoved.connect(self.moveFigure)
        self.clPopup.upButton.clicked.connect(self.fineUpMove)
        self.clPopup.downButton.clicked.connect(self.fineDownMove)
        self.clPopup.leftButton.clicked.connect(self.fineLeftMove)
        self.clPopup.rightButton.clicked.connect(self.fineRightMove)                
        
        self.wConf.fit_button.clicked.connect(self.fit)

        # output
        self.wTop.integrateGate.clicked.connect(self.integrate)        
        self.wTop.outputGate.clicked.connect(self.resultPopup)
        
        # for key_press_event
        self.wPlot.canvas.setFocusPolicy( QtCore.Qt.ClickFocus )
        self.wPlot.canvas.setFocus()
        
        # plotting signals
        self.wPlot.plusButton.clicked.connect(lambda: self.zoomIn(self.wPlot.canvas))
        self.wPlot.minusButton.clicked.connect(lambda: self.zoomOut(self.wPlot.canvas))        

        #################        
        # end of Signals
        #################

        ################################        
        # implementation of Signals
        ################################        

    def closeAll(self):
        self.close()
        self.resPopup.close()
        self.clPopup.close()
        self.pPopup.close()                
        
    def connect(self):
        self.resizeID = self.wPlot.canvas.mpl_connect("resize_event", self.on_resize)
        self.pressID = self.wPlot.canvas.mpl_connect("button_press_event", self.on_press)        

    def edit_connect(self):
        self.e_draw = self.wPlot.canvas.mpl_connect('draw_event', self.draw_callback)
        self.e_press = self.wPlot.canvas.mpl_connect('button_press_event', self.button_press_callback)
        self.e_release = self.wPlot.canvas.mpl_connect('button_release_event', self.button_release_callback)
        self.e_key = self.wPlot.canvas.mpl_connect('key_press_event', self.key_press_callback)
        self.e_motion = self.wPlot.canvas.mpl_connect('motion_notify_event', self.motion_notify_callback)

    def disconnect(self):
        self.wPlot.canvas.mpl_disconnect(self.pressID)        

    def edit_disconnect(self):
        self.wPlot.canvas.mpl_disconnect(self.e_draw)
        self.wPlot.canvas.mpl_disconnect(self.e_press)
        self.wPlot.canvas.mpl_disconnect(self.e_release)
        self.wPlot.canvas.mpl_disconnect(self.e_key)
        self.wPlot.canvas.mpl_disconnect(self.e_motion)
        
    def on_resize(self, event):
        self.wPlot.figure.tight_layout()
        self.wPlot.canvas.draw()

    def on_press(self, event):        
        if not event.inaxes: return
        self.selected_plot_index = list(self.wPlot.figure.axes).index(event.inaxes)
        self.selected_row, self.selected_col = self.plot_position(self.selected_plot_index)
        global t
        if t is None:
            t = threading.Timer(DEBOUNCE_DUR, self.on_singleclick, [event])
            t.start()
        if event.dblclick:
            t.cancel()
            try:
                self.on_dblclick(event)
            except:
                pass
                
    def on_singleclick(self, event):
        global t
        if (debugPrints):
            print("inside on_singleclick")
        if self.isZoomed == False:
            if (debugPrints):
                print("inside on_singleclick - ZOOM false")            
            try:
                if self.rec is not None:
                    self.rec.remove()
            except:
                pass

            for i, plot in enumerate(self.wPlot.figure.axes):
                if (i == self.selected_plot_index):
                    self.isSelected = True
                    self.rec = self.create_rectangle(plot)
            try:
                self.clickToIndex(self.selected_plot_index)
            except:
                pass
            self.wPlot.canvas.draw()
        else:
            if (debugPrints):            
                print("inside on_singleclick - ZOOM true")            
            self.wTop.createGate.setEnabled(True)
            self.wTop.editGate.setEnabled(True)
            self.isSelected = False
            if self.toCreateGate == True:
                if (debugPrints):
                    print("inside createGate mode")
                self.click = [int(float(event.xdata)), int(float(event.ydata))]
                # create interval (1D)
                if self.wConf.button1D.isChecked():
                    self.line = self.addLine(self.click[0])
                    self.listLine.append(self.line)
                    # removes the lines from the plot
                    if len(self.listLine) > 2:
                        self.removeLine()
                else:
                    self.polygon = self.addPolygon(self.click[0], self.click[1])

            self.wPlot.canvas.draw()
        t = None

    def on_dblclick(self, event):
        global t
        # select plot
        if self.isZoomed == False:
            self.wTop.createGate.setEnabled(False)            
            self.wTop.editGate.setEnabled(False)
            self.clickToIndex(self.selected_plot_index)
            # clear existing figure
            self.wPlot.figure.clear()
            self.wPlot.canvas.draw()
            # create selected figure
            try:
                if (debugPrints):
                    print("double click try zoom")
                # we are in zoomed mode
                self.isZoomed = True
                self.wTop.createGate.setEnabled(True)                
                self.wTop.editGate.setEnabled(True)                
                self.add(self.selected_plot_index) # this creates the histogram axes
                self.wPlot.canvas.draw() # this drawing command creates the renderer 
                a = plt.gca()
                self.plot_histogram(a, self.selected_plot_index) # the previous step is fundamental for blitting
                # remove color bar just for gating
                if self.h_dim[self.selected_plot_index] == 2:
                    im = a.images
                    if im is not None:
                        try:
                            cb = im[-1].colorbar
                            cb.remove()
                        except IndexError:
                            pass
                self.drawAllGate()
            except:
                QMessageBox.about(self, "Warning", "There are no histograms defined...")
                self.initialize_canvas(self.wConf.row, self.wConf.col)
        else:
            if self.toCreateGate == True:
                if self.wConf.button1D.isChecked():
                    lst = []
                    # adding gate to dictionary of regions
                    for index in range(len(self.listLine)):
                        lst.append(self.listLine[index].get_xydata())
                    # push gate to shared memory 1D
                    self.formatLinetoShMem(lst)
                    # save the gate for drawing later
                    self.set_line(self.wTop.listGate.currentText(), self.listLine)
                    #adding to list of gates for redrawing
                    key = self.wConf.spectrum_name.text()
                    self.artist1D[self.wTop.listGate.currentText()] = self.listLine
                    self.artist_dict[key] = self.artist1D

                else:
                    self.xs.append(self.xs[-1])
                    self.xs.append(self.xs[0])
                    self.ys.append(self.ys[-1])
                    self.ys.append(self.ys[0])
                    # remove second-last point due to double click
                    self.xs.pop(len(self.xs)-2)
                    self.ys.pop(len(self.ys)-2)
                    self.polygon.set_data(self.xs, self.ys)
                    self.polygon.figure.canvas.draw()
                    self.set_line(self.wTop.listGate.currentText(), [self.polygon])                    
                    if (debugPrints):
                        print(self.wTop.listGate.currentText(), "\n",self.polygon.get_xydata())
                    # push gate to shared memory 2D
                    self.formatLinetoShMem(self.polygon)
                    #adding to list of gates for redrawing
                    key = self.wConf.spectrum_name.text()
                    self.artist2D[self.wTop.listGate.currentText()] = self.polygon
                    self.artist_dict[key] = self.artist2D

                if (debugPrints):
                    print(self.artist_dict)
                    
                # exiting gating mode
                self.toCreateGate = False
                self.timer.start()
            else:
                self.wTop.createGate.setEnabled(False)                
                self.wTop.editGate.setEnabled(False)
                #draw the back the original canvas
                self.wPlot.figure.clear()
                self.wPlot.canvas.draw()
                for index, value in self.h_dict_output.items():
                    a = self.select_plot(index)
                    x,y = self.plot_position(index)
                    a = self.wPlot.figure.add_subplot(self.grid[x,y])
                    self.add(index) # again this creates the histogram axes
                    self.wPlot.canvas.draw() # this drawing command creates the renderer
                    self.plot_histogram(a, index) # the previous step is fundamental for blitting
                self.isZoomed = False
                if (debugPrints):
                    print("self.dict_region", self.dict_region)
                self.drawAllGate()
        t = None

    def draw_callback(self, event):
        self.background = self.wPlot.canvas.copy_from_bbox(self.edit_ax.bbox)
        self.edit_ax.draw_artist(self.poly)
        self.edit_ax.draw_artist(self.l)

    def poly_changed(self, poly):
        #"this method is called whenever the polygon object is called"
        # only copy the artist props to the line (except visibility)
        vis = self.l.get_visible()
        self.l.set_visible(vis)  # don't use the poly visibility state

    def button_press_callback(self, event):
        if not self.showverts:
            if event.inaxes != self.poly.axes: return
            contains, attrd = self.poly.contains(event)
            if not contains: return

            if not self.new_poly_xy:
                x0, y0 = self.poly.xy[0]
            else:
                x0, y0 = self.new_poly_xy[0]

            self.press = x0, y0, event.xdata, event.ydata
            self.isPressed = True
        else:
            #ignore = not self.showverts or event.inaxes is None or event.button != 1
            ignore = event.inaxes is None or event.button != 1
            if ignore:
                return
            self._ind = self.get_ind_under_cursor(event)

    def get_ind_under_cursor(self, event):
        # get the index of the vertex under cursor if within max_ds tolerance'
        # display coords
        xy = np.asarray(self.poly.xy)
        xyt = self.poly.get_transform().transform(xy)
        xt, yt = xyt[:, 0], xyt[:, 1]
        d = np.sqrt((xt - event.x)**2 + (yt - event.y)**2)
        indseq = np.nonzero(np.equal(d, np.amin(d)))[0]
        ind = indseq[0]
        if d[ind] >= self.max_ds:
            ind = None
        return ind

    def button_release_callback(self, event):
        if not self.showverts:
            self.isPressed = False
        else:
            #ignore = not self.showverts or event.button != 1
            ignore = event.button != 1
            if ignore:
                return
            self._ind = None

    def key_press_callback(self, event):
        if not event.inaxes:
            return
        if event.key=='t':
            self.showverts = not self.showverts
            self.l.set_visible(self.showverts)
            if not self.showverts:
                self._ind = None
                self.convert(self.poly)
        elif event.key=='u':
            self._update_line()
            # convert polygon to line2D as the original
            self.pol2line = self.convert(self.poly)
            self.pol2line.append(self.pol2line[0])
            # hide the polygon
            self.poly.set_visible(False)
            x, y = map(list, zip(*self.pol2line))
            self.line_region = plt.Line2D(x, y, color='red', mfc='r')
            self.line_region.set_data(x, y)
            self.edit_ax.add_line(self.line_region)
            self.set_line(self.wTop.listGate.currentText(), [self.line_region])
            if (debugPrints):
                print("update key")
            print(self.wTop.listGate.currentText(), "\n", self.line_region.get_xydata())
            if (debugPrints):
                print("self.dict_region", self.dict_region)
            # push gate to shared memory 2D
            self.formatLinetoShMem(self.line_region)
            # update gate
            gate = self.wTop.listGate.currentText()
            key = self.wConf.spectrum_name.text()
            self.artist2D[gate] = []
            self.artist2D[gate] = self.line_region
            self.artist_dict[key] = self.artist2D
            if (debugPrints):
                print("update gate \n", self.artist_dict)
            self.edit_disconnect()
            self.connect()
        elif event.key=='d':
            if not self.showverts:
                return;
            else:
                #print("d has been pressed")
                ind = self.get_ind_under_cursor(event)
                if ind is None:
                    return
                if ind == 0 or ind == self.last_vert_ind:
                    if (debugPrints):
                        print("Cannot delete root node")
                    return
                self.poly.xy = [tup for i,tup in enumerate(self.poly.xy)
                                if i!=ind]
                self._update_line()
        elif event.key=='i':
            if not self.showverts:
                return;
            else:
                #print("i has been pressed")
                xys = self.poly.get_transform().transform(self.poly.xy)
                p = event.x, event.y # cursor coords
                for i in range(len(xys)-1):
                    s0 = xys[i]
                    s1 = xys[i+1]
                    d = self.distance_p_line(p, s0, s1)
                    if d <= self.max_ds:
                        self.poly.xy = np.array(
                            list(self.poly.xy[:i+1]) +
                            [(event.xdata, event.ydata)] +
                            list(self.poly.xy[i+1:]))
                        self._update_line()
                        break
        self.edit_canvas.draw()
    
    def distance(self, x, y):
        d = x - y
        return np.sqrt(np.dot(d, d))

    def distance_p_line(self, p, s0, s1):
        # Get the distance of a point to a segment.
        v = s1 - s0
        w = p - s0
        c1 = np.dot(w, v)
        if c1 <= 0:
            return self.distance(p, s0)
        c2 = np.dot(v, v)
        if c2 <= c1:
            return self.distance(p, s1)
        b = c1 / c2
        pb = s0 + b * v
        return self.distance(p, pb)

    def motion_notify_callback(self, event):
        if not self.showverts and self.isPressed:
            if event.inaxes != self.poly.axes: return

            x0, y0, xpress, ypress = self.press
            dx = event.xdata - xpress
            dy = event.ydata - ypress
            # this shift from the original position not from the modified one
            xdx = [i+dx for i,_ in self.poly_xy]
            ydy = [i+dy for _,i in self.poly_xy]
            self.new_poly_xy = [[a, b] for a, b in zip(xdx, ydy)]
            self.poly.set_xy(self.new_poly_xy)
            self.poly.figure.canvas.draw()
            self._update_line()
        else:
            #ignore = (not self.showverts or event.inaxes is None or
            ignore = (event.inaxes is None or
                      event.button != 1 or self._ind is None)
            if ignore:
                return
            x,y = event.xdata, event.ydata

            if self._ind == 0 or self._ind == self.last_vert_ind:
                self.poly.xy[0] = x,y
                self.poly.xy[self.last_vert_ind] = x,y
            else:
                self.poly.xy[self._ind] = x,y
            self._update_line()

            self.edit_canvas.restore_region(self.background)
            self.edit_ax.draw_artist(self.poly)
            self.edit_ax.draw_artist(self.l)
            self.edit_canvas.blit(self.edit_ax.bbox)
    
    def convert(self, poly):
        # convert tuple in list
        self.poly_xy.clear()
        for i,tup in enumerate(self.poly.xy):
            if i<len(self.poly.xy)-1:
                self.poly_xy.append(list(itertools.chain(tup)))
        return self.poly_xy

    def _update_line(self):
        # save verts because polygon gets deleted when figure is closed
        self.verts = self.poly.xy
        self.last_vert_ind = len(self.poly.xy) - 1
        self.l.set_data(zip(*self.poly.xy))

    # set slice/contour region        
    def set_line(self, name, line):
        self.dict_region[name] = line
            
    # get slice/contour region        
    def get_line(self, name):
        return self.dict_region[name]

    ######################################
    # end of implementation of Signals
    ######################################

    ######################################
    # start up of GUI
    ######################################

    def at_startup(self):
        self.initialize_canvas(self.wConf.row, self.wConf.col)
        self.connect()
        
    def initialize_canvas(self, row, col):
        # everytime a new pane geometry is created the histogram
        # dictionary and the index of the plot has to be reset
        self.h_dict.clear()
        self.h_dict_output.clear()
        self.index = 0
        self.idx = 0
        self.wPlot.figure.clear()
        self.initialize_figure(self.create_figure(row, col))
        self.wPlot.canvas.draw()
        
    def initialize_figure(self, grid):
        self.h_dim.clear()
        self.h_lst.clear()        
        for i in range(self.wConf.row):
            for j in range(self.wConf.col):
                a = self.wPlot.figure.add_subplot(grid[i,j])
        # initialize list of figure
        for z in range(self.wConf.row*self.wConf.col):
            self.h_dict[z] = self.initialize_histogram()
            self.h_zoom_max.append(self.yhigh)
        self.h_dim = self.get_histo_key_list(self.h_dict, "dim")
        self.h_lst = self.get_histo_key_list(self.h_dict, "name")
        
    def initialize_histogram(self):
        return {"name": 0, "dim": 0, "xmin": 0, "xmax": 0, "xbin": 0,
                "ymin": 0, "ymax": 0, "ybin": 0}
        
    def create_figure(self, row, col):
        self.wConf.row = row
        self.wConf.col = col
        if self.checkVersion(matplotlib.__version__) < self.checkVersion("2.0.0"):
            self.grid = gridspec.GridSpec(ncols=self.wConf.col, nrows=self.wConf.row)
        else:
            self.grid = gridspec.GridSpec(ncols=self.wConf.col, nrows=self.wConf.row, figure=self.wPlot.figure)
        return self.grid
        
    ######################################
    # end of start up of GUI
    ######################################

    ###############################################
    # connection to SpecTcl REST interface
    ###############################################    

    def sendRequest(self, url):
        req = QtNetwork.QNetworkRequest(QtCore.QUrl(url))
        req.setHeader(QtNetwork.QNetworkRequest.ContentTypeHeader,"application/x-www-form-urlencoded")
        self.nam = QtNetwork.QNetworkAccessManager()
        self.nam.finished.connect(self.handleResponse)
        self.nam.get(req)

    def handleResponse(self, reply):
        er = reply.error()
        if er == QtNetwork.QNetworkReply.NoError:
            bytes_string = reply.readAll()
            json_ar = json.loads(str(bytes_string, 'utf-8'))
            data = json_ar['status']
            if (debugPrints):
                print("Operation status: ", data)
        else:
            if (debugPrints):            
                print("Operation error: ", er)
                print(reply.errorString())

    def incrementNumbers(self, parameter, new_number):
        number = (re.findall(r'\d+',parameter))[0]
        out = parameter.replace(number,str(new_number).zfill(int(math.log10(self.nparams))+1))
        return out
        
    def formatLinetoShMem(self, lst):
        server = "http://"+self.wTop.server.text()
        reqStr = server + "/spectcl/gate/edit"
        reqStr += "?name=" + self.wTop.listGate.currentText()
        self.nparams = int(self.getParameterCount())+1

        if self.wConf.button1D.isChecked():
            x, y = map(list, zip(*lst))
            x, y = map(list, zip(*x))
            low = min(x)
            high = max(x)
            reqStr += "&high=" + str(high)
            reqStr += "&low="  + str(low)
            if self.gateType == "s":
                reqStr += "&type=s"
                reqStr += "&parameter=" + self.wConf.listParams[0].currentText()
            else:
                reqStr += "&type=gs"                
                param_str = self.wConf.listParams[0].currentText()
                for i in range(self.nparams):
                    new_param_str = self.incrementNumbers(param_str, i)
                    reqStr += "&parameter=" + new_param_str
            self.sendRequest(reqStr)
        else:
            reqStr += "&type="+self.gateType
            if self.gateType == "c" or self.gateType == "b":
                reqStr += "&xparameter=" + self.wConf.listParams[0].currentText()
                reqStr += "&yparameter=" + self.wConf.listParams[1].currentText()            
            else:
                param_str = self.wConf.listParams[0].currentText()
                for i in range(self.nparams):
                    new_param_str = self.incrementNumbers(param_str, i)
                    reqStr += "&parameter=" + new_param_str                
            
            points = lst.get_xydata()
            points = points[:-1]
            for index in range(len(points)):
                reqStr += "&xcoord("+str(index)+")="+str(points[index][0])
                reqStr += "&ycoord("+str(index)+")="+str(points[index][1])
            self.sendRequest(reqStr)            
        
    def formatShMemToLine(self, name):
        lst_tmp = []
        x = []
        y = []
        for key, value in self.gate_dict.items():
            if key == name:
                if self.checkVersion(httplib2.__version__) < self.checkVersion("0.10"):
                    if "s" in str(value):
                        xmin = min(i for i in value if isinstance(i, float))
                        xmax = max(i for i in value if isinstance(i, float))
                        l = mlines.Line2D([xmin, xmin], [self.ylow,self.yhigh])
                        lst_tmp.append(l)
                        l = mlines.Line2D([xmax, xmax], [self.ylow,self.yhigh])
                        lst_tmp.append(l)
                    elif "c" in str(value):
                        polygon = mlines.Line2D([],[])
                        points = [i for i in value if isinstance(i, list) and len(i)>2]
                        for dic in points[0]:
                            for key, value in dic.items():
                                if key == "x":
                                    x.append(value)
                                elif key == "y":
                                    y.append(value)                                
                        x.append(x[0])
                        y.append(y[0])
                        polygon.set_data(x,y)
                        lst_tmp.append(polygon)
                    else:
                        if (debugPrints):
                            print("Not implemented yet")                        
                else:
                    self.gateTypeDict[key] = value[0]
                    if value[0] == "s":
                        for i in range(2):
                            l = mlines.Line2D([value[i+2],value[i+2]], [self.ylow,self.yhigh])
                            lst_tmp.append(l)
                    elif value[0] == "c":
                        polygon = mlines.Line2D([],[])
                        for dic in value[2]:
                            for key, value in dic.items():
                                if key == "x":
                                    x.append(value)
                                elif key == "y":
                                    y.append(value)                                
                        x.append(x[0])
                        y.append(y[0])
                        polygon.set_data(x,y)
                        lst_tmp.append(polygon)
                    else:
                        if (debugPrints):
                            print("Not implemented yet")

        # adding gate to dictionary of regions
        self.dict_region[name] = lst_tmp
        if (debugPrints):
            print("gate type dict", self.gateTypeDict)
        
    ###############################################
    # end of connection to SpecTcl REST interface
    ###############################################    

    ##########################################
    ## update from ShMem
    ##########################################

    def update(self):
        # this snippet tests if the rest server is up and running
        try:
            # update hostname and port from GUI
            hostnameport = str(self.wTop.server.text()).split(":")
            hostname = hostnameport[0]
            port = hostnameport[1]
            b_hostname = hostname.encode('utf-8')
            b_port = port.encode('utf-8')
            if (debugPrints):
                print(b_hostname, b_port)
                # creates a dataframe for spectrum info
                print("before cpy.CPyConverter().Update")
            s = cpy.CPyConverter().Update(bytes(hostname, encoding='utf-8'), bytes(port, encoding='utf-8'))
            self.spectrum_list = pd.DataFrame(
                {'id': s[0],
                 'names': s[1],
                 'dim' : s[2],
                 'binx': s[3],
                 'minx': s[4],
                 'maxx': s[5],
                 'biny': s[6],
                 'miny': s[7],
                 'maxy': s[8],
                 'data': s[9]}
            )
            # update and create parameter, spectrum, and gate lists
            self.create_parameter_list()
            self.create_spectrum_list()                    
            self.create_gate_list()        
            #self.update_spectrum_info()
            self.isCluster = False

            self.createDf()
        #except Exception as e:
        #    print(e)
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started...")

    # get parameter count
    def getParameterCount(self):
        lst_param = list(self.param_list.values())
        lst_param = [x for x in lst_param if any(c.isdigit() for c in x)]
        res = max(list(map(lambda sub:int(''.join([ele for ele in sub if ele.isnumeric()])), lst_param)))
        return res
        
    # update and create parameter list
    def update_parameter_list(self):
        try:
            server = "http://"+self.wTop.server.text()+"/spectcl/parameter/list"
            tmpl = httplib2.Http().request(server)[1]
            #tmpl = httplib2.Http().request("http://localhost:8080/spectcl/parameter/list")[1]
            tmp = json.loads(tmpl.decode())
            tmpid = []
            tmpname = []
            for dic in tmp['detail']:
                for key in dic:
                    if key == 'id':
                        tmpid.append(dic[key])
                    elif key == 'name':
                        tmpname.append(dic[key])
            ziplst = zip(tmpid, tmpname)
            self.param_list = dict(ziplst)
        except:
            QMessageBox.about(self, "Warning", "update_parameter_list - The rest interface for SpecTcl was not started...")

    def create_parameter_list(self):
        self.update_parameter_list()
        for key, value in self.param_list.items():
            for i in range(2):
                if self.wConf.listParams[i].findText(value) == -1:
                    self.wConf.listParams[i].addItem(value)

    # update and create spectrum list
    def update_spectrum_parameters(self):
        try:
            server = "http://"+self.wTop.server.text()+"/spectcl/spectrum/list"
            tmpl = httplib2.Http().request(server)[1]
            #tmpl = httplib2.Http().request("http://localhost:8080/spectcl/spectrum/list")[1]
            tmp = json.loads(tmpl.decode())
            tmppar = []
            for dic in tmp['detail']:
                for key in dic:
                    if key == 'parameters':
                        tmppar.append(dic[key]) 
                        # adds list to dataframe           
            self.spectrum_list['parameters'] = tmppar
        except:
            QMessageBox.about(self, "Warning", "update_spectrum_parameters - The rest interface for SpecTcl was not started...")
            
    def create_spectrum_list(self):
        self.update_spectrum_parameters()
        for name in self.spectrum_list['names']:
            if self.wConf.histo_list.findText(name) == -1:
                self.wConf.histo_list.addItem(name)

    # update and create gate list
    def update_gate_list(self):
        try:
            server = "http://"+self.wTop.server.text()+"/spectcl/gate/list"
            tmpl = httplib2.Http().request(server)[1]
            #tmpl = httplib2.Http().request("http://localhost:8080/spectcl/gate/list")[1]
            tmp = json.loads(tmpl.decode())
            lst_name = []
            lst_value = []
            lst_all = []
            ll = []                    

            for dic in tmp['detail']:
                for key in dic:
                    lst_all.append(dic[key])

            # index slice gates
            index_s_lst = [i for i, e in enumerate(lst_all) if e == "s"]
            # index contour gates
            index_c_lst = [i for i, e in enumerate(lst_all) if e == "c"]
            # index band gates
            index_b_lst = [i for i, e in enumerate(lst_all) if e == "b"]
            # index gamma slice gates
            index_gs_lst = [i for i, e in enumerate(lst_all) if e == "gs"]
            # index gamma contour gates
            index_gc_lst = [i for i, e in enumerate(lst_all) if e == "gc"]
            # index gamma band gates
            index_gb_lst = [i for i, e in enumerate(lst_all) if e == "gb"]                        

            if (debugPrints):            
                print("index_s_lst", index_s_lst)
                print("index_c_lst", index_c_lst)
                print("index_b_lst", index_b_lst)
                print("index_gs_lst", index_gs_lst)
                print("index_gc_lst", index_gc_lst)
                print("index_gb_lst", index_gb_lst)                        
            
            # adding slice gates
            for i in range(len(index_s_lst)):
                ll.append(lst_all[index_s_lst[i]-1:index_s_lst[i]+4])
            # adding contour gates
            for i in range(len(index_c_lst)):
                ll.append(lst_all[index_c_lst[i]-1:index_c_lst[i]+3])
            # adding band gates
            for i in range(len(index_b_lst)):
                ll.append(lst_all[index_b_lst[i]-1:index_b_lst[i]+3])
            # adding gamma slice gates
            for i in range(len(index_gs_lst)):
                ll.append(lst_all[index_gs_lst[i]-1:index_gs_lst[i]+4])
            # adding gamma contour gates
            for i in range(len(index_gc_lst)):
                ll.append(lst_all[index_gc_lst[i]-1:index_gc_lst[i]+3])
            # adding gamma band gates
            for i in range(len(index_gb_lst)):
                ll.append(lst_all[index_gb_lst[i]-1:index_gb_lst[i]+3])
                
            for lst in ll:
                lst_name.append(lst[0])
                lst_value.append(lst[1:])

            ziplst = zip(lst_name, lst_value)
            self.gate_dict = dict(ziplst)
            if (debugPrints):
                print(self.gate_dict)
            
        except:
            pass

    def create_gate_list(self):
        self.update_gate_list()
        for key, value in self.gate_dict.items():
            if self.wTop.listGate.findText(key) == -1:
                self.wTop.listGate.addItem(key)        
            self.formatShMemToLine(key)

    # update spectrum information
    def update_spectrum_info(self):
        hist_tmp = {}
        hist_name = str(self.wConf.histo_list.currentText())
        hist_dim= 0
        hist_minx = 0
        hist_maxx = 0
        hist_binx = 0
        hist_miny = 0
        hist_maxy = 0
        hist_biny = 0

        try:
            # update info on gui for spectrum name
            self.wConf.spectrum_name.setText(hist_name)
            # extra data from data frame to fill up the gui
            select = self.spectrum_list['names'] == hist_name
            df = self.spectrum_list.loc[select]
            hist_dim = df.iloc[0]['dim']
            if hist_dim == 1:
                self.wConf.button1D.setChecked(True)
            else:
                self.wConf.button2D.setChecked(True)
            self.check_histogram();
            for i in range(hist_dim):
                index = self.wConf.listParams[i].findText(df.iloc[0]['parameters'][i], QtCore.Qt.MatchFixedString)
                if index >= 0:
                    self.wConf.listParams[i].setCurrentIndex(index)
                if i == 0:
                    self.wConf.listParams_low[i].setText(str(df.iloc[0]['minx']))
                    self.wConf.listParams_high[i].setText(str(df.iloc[0]['maxx']))
                    self.wConf.listParams_bins[i].setText(str(df.iloc[0]['binx']))
                    hist_minx = self.wConf.listParams_low[i].text().replace(' ', '')
                    hist_maxx = self.wConf.listParams_high[i].text().replace(' ', '')
                    hist_binx = self.wConf.listParams_bins[i].text().replace(' ', '')
                else :
                    self.wConf.listParams_low[i].setText(str(df.iloc[0]['miny']))
                    self.wConf.listParams_high[i].setText(str(df.iloc[0]['maxy']))
                    self.wConf.listParams_bins[i].setText(str(df.iloc[0]['biny']))
                    hist_miny = self.wConf.listParams_low[i].text().replace(' ', '')
                    hist_maxy = self.wConf.listParams_high[i].text().replace(' ', '')
                    hist_biny = self.wConf.listParams_bins[i].text().replace(' ', '')

            hist_tmp = {"name": hist_name, "dim": hist_dim, "xmin": hist_minx, "xmax": hist_maxx, "xbin": hist_binx,
                        "ymin": hist_miny, "ymax": hist_maxy, "ybin": hist_biny}
            return hist_tmp
        except:
            QMessageBox.about(self, "Warning", "Please click 'Get Data' to access the data...")

    def update_plot(self):
        try:
            if self.isZoomed == False:
                index_list = self.mapDictionaryToIndex(self.h_dict_output)
                for index, value in self.h_dict_output.items():
                    a = self.select_plot(index)
                    time.sleep(0.01)
                    self.plot_histogram(a, index)
            else:
                a = plt.gca()
                time.sleep(0.01)            
                self.plot_histogram(a, self.selected_plot_index)

            self.wPlot.figure.tight_layout()
            self.wPlot.canvas.draw_idle()
        except:
            pass

    def self_update(self):
        self.wTop.slider_label.setText("Refresh interval ({} s)".format(self.wTop.slider.value()))
        if self.wTop.slider.value() != 0:
            self.timer.setInterval(1000*int(self.wTop.slider.value()))
            self.timer.timeout.connect(self.update)
            self.timer.timeout.connect(self.update_plot)            
            self.timer.start()
        else:
            self.timer.stop()
        
            
    ##########################################
    ## end of update from ShMem
    ##########################################    

    ##########################################
    ## load/save geometry window
    ##########################################    
    
    def saveGeo(self):
        fileName = self.saveFileDialog()
        try:
            f = open(fileName,"w")
            tmp = {"row": self.wConf.row, "col": self.wConf.col, "geo": self.h_dict_output}
            QMessageBox.about(self, "Saving...", "Window configuration saved!")
            f.write(str(tmp))
            f.close()
        except TypeError:
            pass

    def loadGeo(self):
        fileName = self.openFileNameDialog()
        try:
            self.isLoaded = True
            f = open(fileName,"r").read()
            infoGeo = eval(f)
            self.wConf.row = infoGeo["row"]
            self.wConf.col = infoGeo["col"]
            # change index in combobox to the actual loaded values
            index_row = self.wConf.histo_geo_row.findText(str(self.wConf.row), QtCore.Qt.MatchFixedString)
            index_col = self.wConf.histo_geo_col.findText(str(self.wConf.col), QtCore.Qt.MatchFixedString)
            if index_row >= 0 and index_col >= 0:
                self.wConf.histo_geo_row.setCurrentIndex(index_row)
                self.wConf.histo_geo_col.setCurrentIndex(index_col)
                self.initialize_canvas(infoGeo["row"],infoGeo["col"])
                self.h_dict_output = infoGeo["geo"]
                if len(self.h_dict_output) == 0:
                    QMessageBox.about(self, "Warning", "You saved an empty pane geometry...")
                if (debugPrints):
                    print(self.h_dict_output)
                self.add_plot()

            self.update_plot()
            
        except TypeError:
            pass

    def openFileNameDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getOpenFileName(self,"Open file...", "","Window Files (*.win);;All Files (*)", options=options)
        if fileName:
            return fileName
        
    def saveFileDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getSaveFileName(self,"Save file...","","Window Files (*.win);;All Files (*)", options=options)
        if fileName:
            return fileName
            
    ##########################################
    ## end of load/save geometry window
    ##########################################    

    ############################
    ## zoom operations
    ############################

    def zoom(self, ax, index, flag):
        ymax = self.get_histo_zoomMax(index)
        if flag == "in":
            ymax /= 2
        elif flag == "out":
            ymax *= 2
        if self.h_dim[index] == 1:
            ax.set_ylim(0,ymax)
            self.set_histo_zoomMax(index, ymax)
        if self.h_dim[index] == 2:
            self.h_lst[index].set_clim(vmax=ymax)
            self.set_histo_zoomMax(index, ymax)
            
    def updateZoom(self):
        for i, ax in enumerate(self.wPlot.figure.axes):
            ymax = self.h_zoom_max[i]
            if self.h_dim[i] == 1:
                ax.set_ylim(0,ymax)
            if self.h_dim[i] == 2:
                self.h_lst[i].set_clim(vmax=ymax)
        
    def zoomIn(self, canvas):
        if self.isZoomed == False:
            for i, ax in enumerate(self.wPlot.figure.axes):
                if (i == self.selected_plot_index):
                    self.zoom(ax, i, "in")
                    try:
                        self.rec.remove()
                        self.rec = self.create_rectangle(ax)                    
                    except:
                        pass
        else:
            ax = plt.gca()
            self.zoom(ax, self.selected_plot_index, "in")            
            
        canvas.draw()

    def zoomOut(self, canvas):
        if self.isZoomed == False:
            for i, ax in enumerate(self.wPlot.figure.axes):
                if (i == self.selected_plot_index):
                    self.zoom(ax, i, "out")
                    try:
                        self.rec.remove()
                        self.rec = self.create_rectangle(ax)                    
                    except:
                        pass
        else:
            ax = plt.gca()
            self.zoom(ax, self.selected_plot_index, "out")            
                
        canvas.draw()
        
    ############################
    ## end of zoom operations
    ############################    

    ##################################
    ## histogram operations
    ##################################

    # get value for a dictionary at index x with key y
    def get_histo_key_value(self, d, index, key):
        if key in d[index]:
            return d[index][key]
        
    # get a list of elements identified by the key y for a dictionary
    def get_histo_key_list(self, d, keys):
        lst = []
        for key, value in d.items():
            lst.append(self.get_histo_key_value(d, key, keys))
        return lst
                
    # get histo name
    def get_histo_name(self, index):
        return self.h_dict[index]["name"]

    # get histo dim
    def get_histo_dim(self, index):
        return int(float(self.h_dict[index]["dim"]))

    # get histo xmin
    def get_histo_xmin(self, index):
        return int(float(self.h_dict[index]["xmin"]))

    # get histo xmax
    def get_histo_xmax(self, index):
        return int(float(self.h_dict[index]["xmax"]))

    # get histo xbin
    def get_histo_xbin(self, index):
        return int(float(self.h_dict[index]["xbin"]))

    # get histo ymin
    def get_histo_ymin(self, index):
        return int(float(self.h_dict[index]["ymin"]))

    # get histo ymax
    def get_histo_ymax(self, index):
        return int(float(self.h_dict[index]["ymax"]))

    # get histo ybin
    def get_histo_ybin(self, index):
        return int(float(self.h_dict[index]["ybin"]))
    
    # get histo zoom max
    def get_histo_zoomMax(self, index):
        return int(float(self.h_zoom_max[index]))

    # set histo zoom max
    def set_histo_zoomMax(self, index, value):
        self.h_zoom_max[index] = value

    # select axes based on indexing
    def select_plot(self, index):
        for i, plot in enumerate(self.wPlot.figure.axes):
            # retrieve the subplot from the click
            if (i == index):
                return plot

    # returns position in grid based on indexing
    def plot_position(self, index):
        cntr = 0
        # convert index to position in geometry
        for i in range(self.wConf.row):
            for j in range(self.wConf.col):            
                if index == cntr:
                    return i, j
                else:
                    cntr += 1

    # looking for type 0 (not set) histograms 
    def check_index(self):
        # support list is full
        if any(x == 0 for x in self.h_dim) == self.isFull:
            self.isFull = True
            self.index = 0
            return self.index
        # support list not full, then look for the next index with value 0
        else:
            idx = next((i for i, x in enumerate(self.h_dim) if x==0), None)
            if idx is not None:
                self.index = idx
                return self.index
            else:
                self.Start = True
                
        if self.Start == True:
            if self.index == self.wConf.row*self.wConf.col-1:
                self.index = 0
            else:
                self.index += 1
                    
        return self.index

    # returns mapping list for window geometry
    def mapDictionaryToIndex(self, my_dict):
        index_list = []
        for key, value in my_dict.items():
            index = self.wConf.histo_list.findText(value, QtCore.Qt.MatchFixedString)
            if index >= 0:
                index_list.append(index)

        return index_list

    # geometrically add plots to the right place
    def add_plot(self):
        try:
            # if we load the geometry from file
            if self.isLoaded:
                counter = 0
                for key, value in self.h_dict_output.items():
                    index = self.wConf.histo_list.findText(value, QtCore.Qt.MatchFixedString)
                    # changing the index to the correct histogram to load
                    self.wConf.histo_list.setCurrentIndex(index)
                    # updating histogram dictionary for fast access to information via get_histo_xxx
                    self.h_dict[counter] = self.update_spectrum_info()
                    # updating support list for histogram dimension
                    self.h_dim = self.get_histo_key_list(self.h_dict, "dim")
                    self.erase_plot(counter)
                    self.add(counter)
                    counter += 1
                self.wPlot.canvas.draw()
                self.isLoaded = False
            else:
                # self adding
                if self.isSelected == False:
                    self.idx = self.check_index()
                # position selected by user
                else:
                    self.idx = self.selected_plot_index

                # updating histogram dictionary for fast access to information via get_histo_xxx       
                self.h_dict[self.idx] = self.update_spectrum_info()        

                self.erase_plot(self.idx)            
                self.add(self.idx)
                self.wPlot.canvas.draw()

                # updating support list for histogram dimension
                self.h_dim = self.get_histo_key_list(self.h_dict, "dim")
                # updating output dictionary
                self.h_dict_output[self.idx] = self.get_histo_name(self.idx)
        except:
            QMessageBox.about(self, "Warning", "Please click 'Get Data' to access the data...")

    # geometrically add plots to the right place and calls plotting    
    def add(self, index):
        if self.isZoomed:
            a = plt.gca()
            self.erase_plot(index)
        else:
            a = self.select_plot(index)
            x,y = self.plot_position(index)
            self.erase_plot(index)
            if self.isSelected == True:
                a = self.wPlot.figure.add_subplot(self.grid[self.selected_row,self.selected_col])
            else:
                a = self.wPlot.figure.add_subplot(self.grid[x,y])
                self.isSelected = False
        self.setup_histogram(a, index)
        
    # erase plot
    def erase_plot(self, index):
        if self.isZoomed:
            a = plt.gca()
        else:
            a = self.select_plot(index)
        # if 2d histo I need a bit more efforts for the colorbar
        if self.h_dim[index] == 2:
            im = a.images
            if im is not None:
                try:
                    cb = im[-1].colorbar
                    cb.remove()
                except IndexError:
                    pass

        a.clear()

    # deletes all the plots and reinitializes the canvas
    def delete_plot(self):
        if self.forAll == True:
            for index in range(self.wConf.col*self.wConf.row):
                self.erase_plot(index)
                # reinitialize the histograms
                self.h_dict_output[index] = self.initialize_histogram()
                # reset support list
                self.isFull = False
        else:
            if self.isSelected == False:
                idx = next((i for i, x in enumerate(self.h_dim) if x), None)
                if idx is not None:
                    self.idx = idx
            else:
                self.idx = self.selected_plot_index
                
            self.erase_plot(self.idx)
            self.isSelected = False
            # reinitialize the histogram at index
            self.h_dict_output[self.idx] = self.initialize_histogram()
            # zero the element in the support list
            self.h_dim[self.idx] = 0            
            self.isFull = False

        if sum(self.h_dim) == 0:
            self.h_dict.clear()            
            self.h_dict_output.clear()
            self.Start = False
            
        self.wPlot.canvas.draw()

    def setup_histogram(self, axis, index):
        hdim = self.get_histo_dim(index)                    
        minx = self.get_histo_xmin(index)
        maxx = self.get_histo_xmax(index)
        binx = self.get_histo_xbin(index)

        if hdim == 1:
            # update axis
            self.yhigh = self.get_histo_zoomMax(index)
            axis.set_xlim(left=minx, right=maxx)
            axis.set_ylim(0,self.yhigh)
            # create histogram
            self.h_lst[index],  = axis.plot([], [], drawstyle='steps')

            self.h_zoom_max[index] = self.yhigh
        else:
            miny = self.get_histo_ymin(index)
            maxy = self.get_histo_ymax(index)
            biny = self.get_histo_ybin(index)

            # empty data for initialization
            w = 0*np.random.random_sample((binx,biny))
            
            # setup up Z gradient 
            self.vmax = self.get_histo_zoomMax(index)

            # setup up palette
            if (self.wConf.button2D_option.currentText() == 'Dark'):
                if self.checkVersion(matplotlib.__version__) < self.checkVersion("2.0.0"):
                    self.palette = 'afmhot'
                else:
                    self.palette = 'plasma'
            else:
                if self.checkVersion(matplotlib.__version__) < self.checkVersion("2.0.0"):
                    self.palette = copy(plt.cm.afmhot)                    
                else:
                    self.palette = copy(plt.cm.plasma)
                w = np.ma.masked_where(w < 0.1, w)
                self.palette.set_bad(color='white')

                
            # create histogram
            self.h_lst[index] = axis.imshow(w,
                                            interpolation='none',
                                            extent=[float(minx),float(maxx),float(miny),float(maxy)],
                                            aspect='auto',
                                            origin='lower',
                                            vmin=self.vmin, vmax=self.vmax,
                                            cmap=self.palette)

        self.axbkg[index] = self.wPlot.figure.canvas.copy_from_bbox(axis.bbox)
        
    # histo plotting
    def plot_histogram(self, axis, index, threshold=0.1):
        hdim = self.get_histo_dim(index)                    
        minx = self.get_histo_xmin(index)
        maxx = self.get_histo_xmax(index)
        binx = self.get_histo_xbin(index)
        w = self.get_data(index)
        if hdim == 1:
            X = self.create_range(binx, minx, maxx)            
            self.h_lst[index].set_data(X, w)
        else:
            if (self.wConf.button2D_option.currentText() == 'Light'):
                if self.checkVersion(matplotlib.__version__) < self.checkVersion("2.0.0"):
                    self.palette = copy(plt.cm.afmhot)
                else:
                    self.palette = copy(plt.cm.plasma)
                    w = np.ma.masked_where(w < threshold, w)
                    self.palette.set_bad(color='white')
                    self.h_lst[index].set_cmap(self.palette)
            self.h_lst[index].set_data(w)

        self.wPlot.figure.canvas.restore_region(self.axbkg[index])
        axis.draw_artist(self.h_lst[index])
        self.wPlot.figure.canvas.blit(axis.bbox)

        # setup colorbar only for 2D
        if hdim == 2:
            divider = make_axes_locatable(axis)
            cax = divider.append_axes('right', size='5%', pad=0.05)
            # add colorbar
            self.wPlot.figure.colorbar(self.h_lst[index], cax=cax, orientation='vertical')
        
        self.wPlot.canvas.draw_idle()
        
    # options for clustering 2D
    def peakPopup(self):
        self.pPopup.show()
        
    # options for clustering 2D
    def clusterPopup(self):
        self.clPopup.show()

    # check histogram dimension from GUI
    def check_histogram(self):
        if self.wConf.button1D.isChecked():
            self.create_disable2D(True)
        else:
            self.create_disable2D(False)
                
    ##################################
    ## end of histogram operations
    ##################################    

    #######################
    # Misc Tools
    #######################
    
    def convertToList2D(self, line):
        poly = []
        for x,y in line.get_xydata():
            poly.append([x,y])
        poly.pop()  # removing the last element because I need just a list of vertices
        return poly

    def convertToList1D(self, line):
        poly = []
        for x,y in line.get_xydata():
            poly.append([x,y])
        return poly
    
    def create_rectangle(self, plot):
        autoAxis = plot.axis()
        percentTB = 0.04;
        percentLR = 0.06;
        tb = (autoAxis[3]-autoAxis[2])*percentTB;        
        lr = (autoAxis[1]-autoAxis[0])*percentLR;
        rec = matplotlib.patches.Rectangle((autoAxis[0]-lr,autoAxis[2]-tb),
                                           (autoAxis[1]-autoAxis[0])+2*lr,
                                           (autoAxis[3]-autoAxis[2])+2*tb, fill=False, color='red', alpha=0.3, lw=2)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)        
        return rec

    # hide/show configuration of the canvas
    def configure(self):
        if self.isHidden == False:
            try:
                self.clearAllGate()
            except:
                pass
            self.wConf.hide()
            try:
                self.drawAllGate()
            except:
                pass
            self.isHidden = True
        else:
            try:
                self.clearAllGate()            
            except:
                pass
            self.wConf.show()
            try:
                self.drawAllGate()            
            except:
                pass
            self.isHidden = False

    # set axis properties
    def set_axis_properties(self, index):
        ax = self.select_plot(index)            

        self.yhigh = self.get_histo_zoomMax(index)
        self.h_zoom_max[index] = self.yhigh

        hname = self.get_histo_name(index)        
        hdim = self.get_histo_dim(index)
        minx = self.get_histo_xmin(index)
        maxx = self.get_histo_xmax(index)
        binx = self.get_histo_xbin(index)

        select = self.spectrum_list['names'] == hname
        df = self.spectrum_list.loc[select]
        x_label = str(df.iloc[0]['parameters'])

        ax.set_xlim(minx,maxx)
        ax.set_ylim(0,self.yhigh)
        ax.set_xlabel(x_label,fontsize=10)
        
        if hdim == 2:
            miny = self.get_histo_ymin(index)
            maxy = self.get_histo_ymax(index)
            binsy = self.get_histo_ybin(index)

            self.vmax = self.get_histo_zoomMax(index)

        return ax
            
    # getting data for plotting
    def get_data(self, index):
        hname = self.get_histo_name(index)
        hdim = self.get_histo_dim(index)
        empty = 0
        w = []
        if hname == "":
            return
        else:
            select = self.spectrum_list['names'] == hname
            df = self.spectrum_list.loc[select]
            w = df.iloc[0]['data']

        if (debugPrints):            
            print("hist:", hname)
            print("hdim:", hdim)
            print("data for ", hname)
            print(type(w))
            print(w)
            print("sum ", sum(w), "len", len(w))
        
        if hdim == 1:
            empty = sum(w)
        else:
            if (debugPrints):
                print(len(w[0]))
            empty = len(w[0])

        if (empty == 0):
            self.isEmpty = True
            QMessageBox.about(self, "Warning", "Please attach to a ring or load an evt file...")
        else:
            self.isEmpty = False
            return w
    
    def change_bkg(self):
        if any(x == 2 for x in self.h_dim) == True:
            indices = [i for i, x in enumerate(self.h_dim) if x == 2]
            for index in indices:
                self.isSelected = False # this line is important for automatic conversion from dark to light and viceversa
                self.add(index)
        self.wPlot.canvas.draw()

    def applyAll(self, state):
        if state == QtCore.Qt.Checked:
            self.forAll = True
        else:
            self.forAll = False

    # if we have already created a 2d image, we just update the dataset
    def checkValue(self, key, value):
        if key in self.h_dict_output.keys() and value == self.h_dict_output[key]: 
            return True
        else: 
            return False

        
    def create_range(self, bins, vmin, vmax):
        x = []
        step = (float(vmax)-float(vmin))/float(bins)
        for i in np.arange(float(vmin), float(vmax), step):
            x.append(i)
        return x

    def create_disable2D(self, value):
        if value==True:
            self.wConf.listParams[1].setEnabled(False)
            self.wConf.listParams_low[1].setText("")
            self.wConf.listParams_low[1].setReadOnly(True)
            self.wConf.listParams_high[1].setText("")
            self.wConf.listParams_high[1].setReadOnly(True)
            self.wConf.listParams_bins[1].setText("")
            self.wConf.listParams_bins[1].setReadOnly(True)            
        else:
            self.wConf.listParams[1].setEnabled(True)
            self.wConf.listParams_low[1].setReadOnly(False)
            self.wConf.listParams_high[1].setReadOnly(False)            
            self.wConf.listParams_bins[1].setReadOnly(False)

    def checkVersion(self, version):
        l = [int(x, 10) for x in version.split('.')]
        l.reverse()
        version = sum(x * (10 ** i) for i, x in enumerate(l))
        return version
    
    #######################
    # end of Misc Tools
    #######################

    ##############
    # Gates 
    ##############

    # helper function that converts index of geometry into index of histo list and updates info
    def clickToIndex(self, idx):
        index = self.wConf.histo_list.findText(str(self.h_dict[idx]['name']), QtCore.Qt.MatchFixedString)
        self.wConf.histo_list.setCurrentIndex(index)
        dim = self.get_histo_dim(idx)
        name = self.h_dict[idx]['name']
        minx = str(self.h_dict[idx]['xmin'])
        maxx = str(self.h_dict[idx]['xmax'])
        binx = str(self.h_dict[idx]['xbin'])
        miny = str(self.h_dict[idx]['ymin'])
        maxy = str(self.h_dict[idx]['ymax'])
        biny = str(self.h_dict[idx]['ybin'])

        self.wConf.spectrum_name.setText(name)

        self.wConf.listParams_low[0].setText(minx)
        self.wConf.listParams_high[0].setText(maxx)
        self.wConf.listParams_bins[0].setText(binx)

        if dim == 1:
            self.wConf.button1D.setChecked(True)
            self.wConf.listParams_low[1].setText('')
            self.wConf.listParams_high[1].setText('')
            self.wConf.listParams_bins[1].setText('')
        else:
            self.wConf.button2D.setChecked(True)
            self.wConf.listParams_low[1].setText(miny)
            self.wConf.listParams_high[1].setText(maxy)
            self.wConf.listParams_bins[1].setText(biny)
        self.check_histogram();
    
    def createGate(self):
        if self.wTop.slider.value() != 0:
            self.timer.stop()         

        # check for histogram existance
        name = self.wConf.histo_list.currentText()
        gate_name = self.wTop.listGate.currentText()
        gate_parameter = self.wConf.listParams[0].currentText()
        
        if (name==""):
            return QMessageBox.about(self,"Warning!", "Please create at least one spectrum")
        else:
            # creating entry in combobox if it doesn't exist
            allItems = [self.wTop.listGate.itemText(i) for i in range(self.wTop.listGate.count())]
            result = gate_name in allItems
            while result:
                self.counter += 1
                gate_name = "default_gate_"+str(self.counter)
                result = gate_name in allItems

            # gate name
            text, okPressed = QInputDialog.getText(self, "Gate name", "Please choose a name for the gate:", QLineEdit.Normal, gate_name)
            if okPressed:
                gate_name = text
                self.wTop.listGate.addItem(gate_name)
                # update boxes
                self.wTop.listGate.setCurrentText(gate_name)

            # gate type
            items = ("s", "c", "b", "gs", "gc", "gb")
            item, okPressed = QInputDialog.getItem(self, "Gate type", "Please choose a type for the gate:", items, 0, False)
            if okPressed:
                self.gateType = item
                
                # gate parameters for gamma gates
                if self.gateType == "gs" or self.gateType == "gc" or self.gateType == "gb":
                    text, okPressed = QInputDialog.getText(self, "Gate parameter", "Please choose a parameter for the gamma gate:", QLineEdit.Normal, gate_parameter) 
                    if okPressed:
                        gate_parameter = text

        # adding gate
        self.gateTypeDict[gate_name] = self.gateType
        if (debugPrints):
            print(self.gateTypeDict)
        self.wConf.resetGateType()
        (self.wConf.buttonGateTypeDict[self.gateType]).setChecked(True)
                

        self.toCreateGate = True;
        self.createRegion()
        self.polygon = self.createPolygon()
        if (debugPrints):
            print("end of create gate")

    def createRegion(self):
        ax = plt.gca()        
        self.cleanRegion()

    def cleanRegion(self):
        self.xs.clear()
        self.ys.clear()

    def createPolygon(self):
        poly = mlines.Line2D([],[])
        return poly
        
    def addLine(self, posx):
        ax = plt.gca()
        ymin, ymax = ax.get_ybound()        
        l = mlines.Line2D([posx,posx], [ymin,ymax])
        ax.add_line(l)
        l.set_color('r')
        return l
        
    def addPolygon(self, posx, posy):
        ax = plt.gca()
        self.xs.append(posx)
        self.ys.append(posy)
        self.polygon.set_data(self.xs, self.ys)
        ax.add_line(self.polygon)
        self.polygon.set_color('r')
        return self.polygon
        
    def removeLine(self):
        l = self.listLine[0]
        self.listLine.pop(0)
        l.remove()

    def editGate(self):
        try:
            if self.wConf.button1D.isChecked():
                self.toCreateGate = True
            else:
                self.disconnect()
                # enabling editing of the gate
                self.edit_connect()
                # initialize editing
                self.edit_init()
        except:
            QMessageBox.about(self, "Warning", "Please create/load a gate...")

    def edit_init(self):
        self.edit_ax = plt.gca()
        # select the gate from the menu and apply the following code
        name_gate = self.wTop.listGate.currentText()
        polygon = (self.get_line(name_gate))[0]
        self.polygon.set_visible(False)
        polygon.set_visible(False)
        self.poly_xy = self.convertToList2D(polygon)
        if (debugPrints):
            print(self.poly_xy)
        self.poly = Polygon(self.poly_xy, animated=True, facecolor='r', ec='none', alpha=0.2)
        self.edit_ax.add_patch(self.poly)
        self.edit_ax.set_clip_on(False)
        self.edit_canvas = self.poly.figure.canvas
        
        x, y = zip(*self.poly.xy)
        self.l = plt.Line2D(x, y, color='red', marker='o', mfc='r', alpha=0.2, animated=True)
        self._update_line()
        self.edit_ax.add_line(self.l)
        
        self.poly.add_callback(self.poly_changed)

        self.wPlot.canvas.draw()
            
    def deleteGateShMem(self, name):
        server = "http://"+self.wTop.server.text()
        reqStr = server + "/spectcl/gate/delete?name=" + name
        self.sendRequest(reqStr)
            
    def deleteGate(self):
        try:
            if self.wTop.slider.value() != 0:
                self.timer.stop()
            # delete from combobox and change index
            name = self.wTop.listGate.currentText()
            index = self.wTop.listGate.findText(name, QtCore.Qt.MatchFixedString)
            if index >= 0:
                self.wTop.listGate.removeItem(index)
            # delete from shared memory
            self.deleteGateShMem(name)
            self.wPlot.canvas.draw()

            if self.wTop.slider.value() != 0:
                self.timer.start()
        except:
            pass
        
    def zoomGate(self, line, ymax):
        x = line.get_xdata()
        y = line.get_ydata()
        y[1] = ymax
        line.set_data(x,y)

    def copy_attributes(self, obj2, obj1, attr_list):
        for i_attribute  in attr_list:
            getattr(obj2, 'set_' + i_attribute)( getattr(obj1, 'get_' + i_attribute)() )

    def clearAllGate(self):
        if self.wTop.slider.value() != 0:
            self.timer.stop()

        # remove all lines
        for k1 in self.artist_dict:
            for k2 in self.artist_dict[k1]:
                artist = (self.artist_dict[k1])[k2]
                isList = isinstance(artist, list)
                if (debugPrints):
                    print(k2, artist)
                if isList:
                    for art in artist:
                        if (debugPrints):
                            print("removing ->", art)
                        art.remove()
                else:
                    if (debugPrints):                    
                        print("removing ->", artist)                    
                    artist.remove()

        self.wPlot.canvas.draw()            

        if self.wTop.slider.value() != 0:
            self.timer.start()
                
    def clearGate(self):
        if self.wTop.slider.value() != 0:
            self.timer.stop() 

        self.wConf.resetGateType()
        hname = self.wConf.spectrum_name.text()
        # remove lines
        for k1 in self.artist_dict:
            if k1 == hname:
                for k2 in self.artist_dict[k1]:
                    artist = (self.artist_dict[k1])[k2]
                    isList = isinstance(artist, list)
                    if isList:
                        for art in artist:
                            art.remove()
                    else:
                        artist.remove()

        try:
            del self.artist_dict[hname]
        except:
            pass

        self.wPlot.canvas.draw()
        if (debugPrints):
            print(self.artist_dict)
        
        if self.wTop.slider.value() != 0:
            self.timer.start()

    def existGate(self, hname):
        name = self.wTop.listGate.currentText()
        for k1 in self.artist_dict:
            if k1 == hname:
                for k2 in self.artist_dict[k1]:        
                    if k2 == name:
                        return True

        return False

    def plot1DGate(self, axis, key, name):
        tmp = []
        new_line = [mlines.Line2D([],[]), mlines.Line2D([],[])]
        cntr = 0
        for line in self.dict_region[name]:
            ymin, ymax = axis.get_ybound()
            self.copy_attributes(new_line[cntr], line, self.copied_line_attributes)
            self.zoomGate(new_line[cntr], ymax)
            # when drawing, we then allow to edit if needed
            self.listLine.append(tmp)
            new_line[cntr].set_color('r')
            axis.add_artist(new_line[cntr])
            tmp.append(new_line[cntr])
            cntr += 1
            if cntr == 2:
                break

        if key in self.artist_dict:
            if (debugPrints):
                print("self.artist_dict[key]",self.artist_dict[key])
            if any(x == name for x in self.artist_dict[key]):
                if (debugPrints):
                    print("Replace the gate")
                    print("self.artist1D[",name,"] = []")
                self.artist1D[name] = []                
        self.artist1D[name] = tmp
        self.artist_dict[key] = self.artist1D
            
    def plot2DGate(self, axis, key, name):
        new_line = mlines.Line2D([],[])
        for line in self.dict_region[name]:                
            self.copy_attributes(new_line, line, self.copied_line_attributes)
            # when drawing, we then allow to edit if needed
            self.polygon = new_line
            new_line.set_color('r')
            axis.add_artist(new_line)

        if key in self.artist_dict:
            if (debugPrints):
                print("self.artist_dict[key]",self.artist_dict[key])
            if any(x == name for x in self.artist_dict[key]):
                if (debugPrints):
                    print("Replace the gate")
                    print("self.artist2D[",name,"] = []")
                self.artist2D[name] = []                
        self.artist2D[name] = new_line
        self.artist_dict[key] = self.artist2D            

    def drawAllGate(self):
        if self.wTop.slider.value() != 0:
            self.timer.stop()

        # remove all gates from canvas
        try:
            self.clearAllGate()
        except:
            pass
            
        # loop over panel
        if self.isZoomed:
            a = plt.gca()
            h = self.wConf.spectrum_name.text()
            if h in self.artist_dict:
                for gate in self.artist_dict[h]:
                    if (debugPrints):
                        print("gate name:", gate, " to be added in ", h)
                    if self.wConf.button1D.isChecked():
                        self.plot1DGate(a, h, gate)
                    else:
                        self.plot2DGate(a, h, gate)            
        else:
            for index in self.h_dict_output:
                h = self.h_dict_output[index]
                a = self.select_plot(index)            
                self.clickToIndex(index)
                if (debugPrints):
                    print("histogram at panel", index, "named: ", h) 
                if h in self.artist_dict:
                    for gate in self.artist_dict[h]:
                        if (debugPrints):
                            print("gate name:", gate, " to be added in ", h)
                        if self.wConf.button1D.isChecked():
                            self.plot1DGate(a, h, gate)
                        else:
                            self.plot2DGate(a, h, gate)

        self.wPlot.canvas.draw()            

        if (debugPrints):        
            print(self.artist_dict)
        
        if self.wTop.slider.value() != 0:
            self.timer.start()

    def drawGate(self, option):
        try:
            if option == False:
                if self.wTop.slider.value() != 0:
                    self.timer.stop()
                
                if self.selected_plot_index is not None:
                    name = self.wTop.listGate.currentText()
                    key = self.get_histo_name(self.selected_plot_index)
                    gtype = self.gateTypeDict[name]
                    self.wConf.resetGateType()
                    (self.wConf.buttonGateTypeDict[gtype]).setChecked(True)
                    if self.isZoomed:
                        a = plt.gca()
                    else:
                        a = self.select_plot(self.selected_plot_index)
                    if self.wConf.button1D.isChecked():
                        if (debugPrints):
                            print("does the gate", name, "of type", gtype, " exists in", key, "?", self.existGate(key))
                        self.plot1DGate(a, key, name)
                    else:
                        if (debugPrints):                        
                            print("does the gate", name, "of type", gtype, " exists in", key, "?", self.existGate(key))
                        self.plot2DGate(a, key, name)
                    
            self.wPlot.canvas.draw()
            if (debugPrints):
                print(self.artist_dict)
        
            if self.wTop.slider.value() != 0:
                self.timer.start()        
        except:
            pass
                
    ##################
    # end of Gates 
    ##################

    ##############################
    # 1D/2D region integration 
    ##############################
    
    def integrate(self):
        try:
            # get the selected region from the combobox
            self.wPlot.canvas.draw()
            name = self.wTop.listGate.currentText()
            if self.wConf.button1D.isChecked():
                self.integrate1D(name)
            else:
                self.start = time.time()
                self.integrate2D(name)
                #self.multiproc2D(25, name)            
                #self.thread2D(10, name)
                self.stop = time.time()
                if (debugPrints):
                    print("Time elapsed:", self.stop-self.start)
        except:
            QMessageBox.about(self, "Warning", "No gate defined for integration...")
            
    def multiproc2D(self, n, name):
        processes = []

        for i in range(n):
            p = multiprocessing.Process(target=self.integrate2D, args=(name, ))
            processes.append(p)
            p.start()

        for p in processes:
            p.join()
        
    def thread2D(self, n, name):
        threads = []

        for i in range(n):
            t = threading.Thread(target=self.integrate2D, args=(name, ))
            threads.append(t)
            t.start()
            
        for t in threads:
            t.join()

    def integrate2D(self, name):
        # histo name
        name_histo = self.wConf.histo_list.currentText()
        # gate name
        name_gate = name
        # create polygon from line2D
        poly_xy = self.convertToList2D(self.get_line(name)[0])
        polygon = Polygon(poly_xy)
        # remove the duplicated last vertex
        poly = polygon.xy[:-1]
        p = Path(poly)
        # converts data to points
        select = self.spectrum_list['names'] == name_histo
        df = self.spectrum_list.loc[select]
        w = df.iloc[0]['data']

        x = []
        y = []
        if len(self.points) == 0:
            self.fillPoints(w, x, y, self.points_w, self.points)
        isInside = p.contains_points(self.points)
        x = list(compress(x, isInside))
        y = list(compress(y, isInside))        
        z = list(compress(self.points_w, isInside))
        
        area = sum(z)
        centroid_x, centroid_y = self.centroid(x, y)
        fwhm = self.fwhm(x, y)
        # print to output window and file
        self.addRegion(area, centroid_x, centroid_y, fwhm)            
        
    def fillPoints(self, data, xx, yy, zz, points):
        for i, col in enumerate(data):
            if sum(col):
                for j, val in enumerate(col):
                    x,y = self.indexToPos(j,i)
                    if val>=self.clPopup.threshold_slider.value():
                        #print(x,y,val)
                        xx.append(x)
                        yy.append(y)
                        zz.append(val)                                                
                        points.append([x,y])
                        
    def indexToPos(self, i, j):
        minx = float(self.wConf.listParams_low[0].text())-1
        maxx = float(self.wConf.listParams_high[0].text())
        binx = float(self.wConf.listParams_bins[0].text())
        miny = float(self.wConf.listParams_low[1].text())-1
        maxy = float(self.wConf.listParams_high[1].text())
        biny = float(self.wConf.listParams_bins[1].text())
        xstep = (maxx-minx)/binx
        ystep = (maxy-miny)/biny        
        return minx+(i+0.5)*xstep, miny+(j+0.5)*ystep

        
    def findInterval(self, name_histo, name_gate):
        interval = []
        for line in self.dict_region[name_gate]:
            interval.append(line.get_xdata()[0])
        return interval
        
    def integrate1D(self, name):
        # histo name
        name_histo = self.wConf.histo_list.currentText()
        index_histo = self.wConf.histo_list.currentIndex()
        # gate name
        name_gate = name
        a = [] #interval limits
        if (name_gate==""):
            return QMessageBox.about(self,"Warning!", "Inside integrate1D - Please create at least one gate")
        else:
            x = []
            y = []
            fwhm = 0
            minx = self.get_histo_xmin(index_histo)
            maxx = self.get_histo_xmax(index_histo)
            binx = self.get_histo_xbin(index_histo)
            X = self.create_range(binx, minx, maxx)
            tmp = self.get_data(index_histo)
            try:
                a = sorted(self.findInterval(name_histo, name_gate))
                index_list = [i for i, e in enumerate(X) if e > a[0] and e < a[1]]
                x = X[index_list[0]:index_list[-1]]
                y = tmp[index_list[0]:index_list[-1]]
            except:
                pass

            area = sum(y)
            centroid_x, centroid_y = self.centroid(x, y)
            fwhm = self.fwhm(x, y)
            # print to output window and file
            self.addRegion(area, centroid_x, centroid_y, fwhm)            
            
    def fwhm(self, xx, yy):
        if self.wConf.button1D.isChecked():
            max_y = max(yy) # Find the maximum y value
            xs = []
            i = 0
            while i < len(xx):
                if yy[i] > max_y/2.0:
                    xs.append(xx[i])
                i += 1            
            diff = max(xs)-min(xs)
            return round(diff,2)
        else:
            return "-"        
        
    def centroid(self, x, y):
        length = len(x)
        sum_x = np.sum(x)
        sum_y = np.sum(y)
        return round(sum_x/length,2), round(sum_y/length,2)

    def resultPopup(self):
        self.resPopup.setGeometry(100,100,724,500)
        self.resPopup.show()

    def addRegion(self, area, centx, centy, fwhm):
        #["ID", "Spectrum", "Name", "centroid X", "centroid Y", "FWHM", "Area"]
        self.resPopup.tableWidget.setRowCount(0);
        self.table_row.append([str(self.wTop.listGate.currentIndex()),
                               str(self.wConf.histo_list.currentText()),
                               str(self.wTop.listGate.currentText()),
                               centx,
                               centy,
                               fwhm,
                               area])
        for row in self.table_row:
            inx = self.table_row.index(row)
            self.resPopup.tableWidget.insertRow(inx)
            for i in range(len(row)):
                self.resPopup.tableWidget.setItem(inx,i,QTableWidgetItem(str(row[i])))
        header = self.resPopup.tableWidget.horizontalHeader()       
        header.setSectionResizeMode(QHeaderView.Stretch)        
        self.resPopup.tableWidget.resizeColumnsToContents()

    ##############################
    # end of 1D/2D region integration 
    ##############################
        
    ############################
    ## Fitting functions
    ############################
    '''
    def gauss(self, x, amplitude, mean, standard_deviation):
        return amplitude*np.exp(-(x-mean)**2.0 / (2*standard_deviation**2))

    def exp(self, x, a, b, c):
        return a+b*np.exp(x*c)

    def pol1(self, x, p0, p1):
        return p0+p1*x

    def pol2(self, x, p0, p1, p2):
        return p0+p1*x+p2*x**2

    def pol3(self, x, p0, p1, p2, p3):
        return p0+p1*x+p2*x**2+p3*x**3

    def gpol1(self, x, amplitude, mean, standard_deviation, p0, p1, f):
        g = self.gauss(x, amplitude, mean, standard_deviation)
        pol1 = self.pol1(x,p0,p1)
        return f*g+(1-f)*pol1

    def gpol2(self, x, amplitude, mean, standard_deviation, p0, p1, p2, f):
        g = self.gauss(x, amplitude, mean, standard_deviation)
        pol2 = self.pol1(x,p0,p1,p2)
        return f*g+(1-f)*pol2        
    '''
    ############################
    ## end of Fitting functions
    ############################    

    ############################
    ## Fitting
    ############################    
    
    def axislimits(self, ax):
        left, right = ax.get_xlim()
        if self.wConf.fit_range_min.text():
            left = int(self.wConf.fit_range_min.text())
        if self.wConf.fit_range_max.text():            
            right = int(self.wConf.fit_range_max.text())            
        return left, right
    
    def fit(self):
        x = []
        y = []
        x_fit = []
        y_fit = []
        histo_name = str(self.wConf.histo_list.currentText())
        fit_funct = self.wConf.fit_list.currentText()
        if self.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(self.selected_plot_index)

        config = self.fit_factory._configs.get(fit_funct)
        if (debugPrints):
            print("Fit function", config)
        fit = self.fit_factory.create(fit_funct, **config)

        # remove any previous fit on plot
        try:
            self.fitln.remove()
        except:
            pass
        if histo_name == "":
            QMessageBox.about(self, "Warning", "Histogram not existing. Please load an histogram...")
        else:
            if self.wConf.button1D.isChecked():
                # input points for fitting function
                xtmp = self.create_range(int(float(self.wConf.listParams_bins[0].text())), int(float(self.wConf.listParams_low[0].text())), int(float(self.wConf.listParams_high[0].text())))                
                select = self.spectrum_list['names'] == histo_name
                df = self.spectrum_list.loc[select]
                ytmp = df.iloc[0]['data']
                xmin, xmax = self.axislimits(ax)
                if self.wConf.fit_range_min.text():
                    xmin = float(self.wConf.fit_range_min.text())
                if self.wConf.fit_range_max.text():
                    xmax = float(self.wConf.fit_range_max.text())
                if (debugPrints):
                    print("Sub range:", xmin, xmax)
                # create new tmp list with subrange for fitting
                for i in range(len(xtmp)):
                    if (xtmp[i]>=xmin and xtmp[i]<xmax):
                        x.append(xtmp[i])
                        y.append(ytmp[i])
                x = np.array(x)
                y = np.array(y)
                try:
                    self.fitln = fit.start(x, y, xmin, xmax, ax, self.wConf.fit_results)
                except:
                    pass
            else:
                if (debugPrints):
                    print("2D fitting is not implemented")                

        self.wPlot.canvas.draw()

    ############################
    ## end of Fitting
    ############################

    ############################
    ## begin of Peak Analyzer
    ############################

    def peakState(self, state):
        for i, btn in enumerate(self.pPopup.peak_cbox):
            if btn.isChecked() == False:
                try:
                    self.removePeak(i)
                    self.isChecked[i] = False                    
                except:
                    pass
            else:
                if self.isChecked[i] == False:
                    self.drawSinglePeaks(self.peaks, self.properties, self.dataw, i)
                    self.isChecked[i] = True

        '''
        checkAll = False in self.isChecked.values()
        if checkAll == False:
            self.pPopup.show_box.setChecked(True)
        else:
            self.pPopup.show_box.setChecked(False)
        '''
        self.wPlot.canvas.draw()                
                
    def create_peak_signals(self):
        for i in range(self.pPopup.npeaks):
            self.isChecked[i] = False                    
            self.pPopup.peak_cbox[i].stateChanged.connect(self.peakState)
            self.pPopup.peak_cbox[i].setChecked(True)
            
    def update_peak_output(self, peaks, properties):
        for i in range(len(peaks)):
            s = "Peak"+str(i+1)+"\n\tpeak @ " + str(peaks[i])+", FWHM="+str(int(properties['widths'][i]))
            self.pPopup.peak_results.append(s)

    def peakAnalClear(self):
        self.pPopup.peak_results.clear()
        self.removeAllPeaks()
        #self.pPopup.show_box.setChecked(False)
        self.pPopup.remove_peakChecks()

        self.resetPeakDict()
        
        
    def removePeak(self, i):
        self.peak_pos[i][0].remove()
        del self.peak_pos[i]
        self.peak_vl[i].remove()
        del self.peak_vl[i]
        self.peak_hl[i].remove()
        del self.peak_hl[i]
        self.peak_txt[i].remove()
        del self.peak_txt[i]        
        
    def resetPeakDict(self):
        self.peak_pos = {}
        self.peak_vl = {}
        self.peak_hl = {}
        self.peak_txt = {}
        
    def removeAllPeaks(self):
        try:
            for i in self.peak_pos:
                self.pPopup.peak_cbox[i].setChecked(False)                                    
        except:
            pass

        self.resetPeakDict()
        self.wPlot.canvas.draw()

    def allOn(self):
        for i in self.peak_pos:
            self.pPopup.peak_cbox[i].setChecked(False)                                    
        
    def showAll(self,b):
        if b.text() == "Show Peaks":
            if b.isChecked() == True:
                if (debugPrints):
                    print("all ON",self.isChecked)
                # if all(value == False for value in self.isChecked.values()):
                #     self.allOn()
            else:
                if (debugPrints):                
                    print("all OFF",self.isChecked)                
                #if all(value == True for value in self.isChecked.values()):                
                #    self.removeAllPeaks()
                
    def drawSinglePeaks(self, peaks, properties, data, index):
        a = None
        if self.isZoomed:
            a = plt.gca()
        else:
            a = self.select_plot(self.selected_plot_index)

        self.peak_pos[index] = a.plot(peaks[index], data[peaks[index]], "v", color="red")
        self.peak_vl[index] = a.vlines(x=peaks[index], ymin=data[peaks[index]] - properties["prominences"][index], ymax = data[peaks[index]], color = "red")
        self.peak_hl[index] = a.hlines(y=properties["width_heights"][index], xmin=properties["left_ips"][index], xmax=properties["right_ips"][index], color = "red")        
        self.peak_txt[index] = a.text(peaks[index], int(data[peaks[index]]*1.1), str(peaks[index]))

    def analyzePeak(self):
        try:
            width = int(self.pPopup.peak_width.text())
            minx = self.get_histo_xmin(self.selected_plot_index)
            maxx = self.get_histo_xmax(self.selected_plot_index)
            binx = self.get_histo_xbin(self.selected_plot_index)
            X = self.create_range(binx, minx, maxx)
            self.dataw = self.get_data(self.selected_plot_index)
            self.peaks, self.properties = find_peaks(self.dataw, prominence=1, width=width)
            
            self.update_peak_output(self.peaks, self.properties)

            self.pPopup.npeaks = len(self.peaks)
            self.pPopup.create_peakChecks()
            self.create_peak_signals()

        except:
            pass
        
    ############################
    ## end of Peak Analyzer
    ############################                    
    
    ############################
    ## begin of Clustering
    ############################                

    def initializeCluster(self):
        # histo name
        name_histo = self.wConf.histo_list.currentText()
        # converts data to points
        select = self.spectrum_list['names'] == name_histo
        df = self.spectrum_list.loc[select]
        w = df.iloc[0]['data']

        a = None
        if self.isZoomed:
            a = plt.gca()
        else:
            a = self.select_plot(self.selected_plot_index)

        xmin, xmax = a.get_xlim()            
        ymin, ymax = a.get_ylim()
        polygon = Polygon([(xmin,ymin), (xmax,ymin), (xmax,ymax), (xmin,ymax)])
        # remove the duplicated last vertex
        poly = polygon.xy[:-1]
        p = Path(poly)

        # set value for threshold and algo
        self.old_threshold = self.clPopup.threshold_slider.value()
        self.old_algo = self.clPopup.clusterAlgo.currentText()
        
        x = []
        y = []
        if (debugPrints):
            print("Filling points..")
        self.fillPoints(w, x, y, self.points_w, self.points)

        #print(self.points)
        isInside = p.contains_points(self.points)
        self.clusterpts = list(compress(self.points, isInside))
        self.clusterw = list(compress(self.points_w, isInside))

        #print(len(self.clusterpts), len(self.clusterw))
        #print(self.clusterpts[0:10], self.clusterw[0:10])
        
        self.isCluster = True
        
    def analyzeCluster(self):
        algo = self.clPopup.clusterAlgo.currentText()        
        if self.isCluster == False or (self.old_threshold != self.clPopup.threshold_slider.value()):
            self.start = time.time()
            self.initializeCluster()
            self.stop = time.time()
            print("Time elapsed for initialization of clustering:", self.stop-self.start)

        cluster_center=[]
        nclusters = int(self.clPopup.clusterN.currentText())
        algo = self.clPopup.clusterAlgo.currentText()

        self.start = time.time()

        a = None
        if self.isZoomed:
            a = plt.gca()
        else:
            a = self.select_plot(self.selected_plot_index)

        config = self.factory._configs.get(algo)
        print("ML algo config", config)
        MLalgo = self.factory.create(algo, **config)
        # add hooks for popup windows i.e. more arguments that won't be used
        MLalgo.start(self.clusterpts, self.clusterw, nclusters, a, self.wPlot.figure)
        self.stop = time.time()
        print("Time elapsed for clustering:", self.stop-self.start)

        self.wPlot.canvas.draw()

    def thresholdFigure(self):
        self.clPopup.threshold_label.setText("Threshold Level ({})".format(self.clPopup.threshold_slider.value()))
        try:
            a = None
            if self.isZoomed:
                a = plt.gca()
            else:
                a = self.select_plot(self.selected_plot_index)

            self.plot_histogram(a, self.selected_plot_index,self.clPopup.threshold_slider.value())
            self.wPlot.canvas.draw()
        except:
            pass
        
    def loadFigure(self):
        fileName = self.openLoadFileNameDialog()
        try:
            #self.LISEpic = mpimg.imread(fileName)
            self.LISEpic = cv2.imread(fileName)
            cv2.resize(self.LISEpic, (200, 100))
            self.clPopup.loadLISE_name.setText(fileName)
        except TypeError:
            pass

    def fineUpMove(self):
        self.imgplot.remove()        
        self.ystart += 0.002
        self.drawFigure()
        
    def fineDownMove(self):
        self.imgplot.remove()        
        self.ystart -= 0.002
        self.drawFigure()
        
    def fineLeftMove(self):
        self.imgplot.remove()        
        self.xstart -= 0.002
        self.drawFigure()

    def fineRightMove(self):        
        self.imgplot.remove()        
        self.xstart += 0.002
        self.drawFigure()
        
    def moveFigure(self):
        #print(self.clPopup.joystick.direction, self.clPopup.joystick.distance)
        try:
            self.imgplot.remove()        
            if self.clPopup.joystick.direction == "up":
                self.ystart += self.clPopup.joystick.distance*0.03
            elif self.clPopup.joystick.direction == "down":
                self.ystart -= self.clPopup.joystick.distance*0.03
            elif self.clPopup.joystick.direction == "left":
                self.xstart -= self.clPopup.joystick.distance*0.03
            else:
                self.xstart += self.clPopup.joystick.distance*0.03        
            self.drawFigure()
        except:
            pass
            
    def indexToStartPosition(self, index):
        row = self.wConf.row
        col = self.wConf.col
        xoffs = float(1/(2*col))
        yoffs = float(1/(2*row))       
        i, j = self.plot_position(index)
        xstart = xoffs*(2*j+1)-0.1
        ystart = yoffs*(2*i+1)+0.1       

        self.xstart = xstart
        self.ystart = 1-ystart
        
    def drawFigure(self):
        self.alpha = self.clPopup.alpha_slider.value()/10
        self.zoomX = self.clPopup.zoomX_slider.value()/10
        self.zoomY = self.clPopup.zoomY_slider.value()/10        

        ax = plt.axes([self.xstart, self.ystart, self.zoomX, self.zoomY], frameon=True)
        ax.axis('off') 
        self.imgplot = ax.imshow(self.LISEpic,
                                 aspect='auto',
                                 alpha=self.alpha)

        self.wPlot.canvas.draw()

    def deleteFigure(self):
        self.imgplot.remove()
        self.onFigure = False
        self.wPlot.canvas.draw()

    def transFigure(self):
        self.clPopup.alpha_label.setText("Transparency Level ({} %)".format(self.clPopup.alpha_slider.value()*10))
        try:
            self.deleteFigure()
            self.drawFigure()
        except:
            pass

    def zoomFigureX(self):
        self.clPopup.zoomX_label.setText("Zoom X Level ({} %)".format(self.clPopup.zoomX_slider.value()*10))
        try:
            self.deleteFigure()
            self.drawFigure()
        except:
            pass

    def zoomFigureY(self):
        self.clPopup.zoomY_label.setText("Zoom Y Level ({} %)".format(self.clPopup.zoomY_slider.value()*10))
        try:
            self.deleteFigure()
            self.drawFigure()
        except:
            pass                
            
    def addFigure(self):
        try:
            self.indexToStartPosition(self.selected_plot_index)

            if self.onFigure == False:
                self.drawFigure()
                self.onFigure = True
        except:
            pass
            
    def openLoadFileNameDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getOpenFileName(self,"Open file...", "","Image Files (*.png *.jpg);;All Files (*)", options=options)
        if fileName:
            return fileName
        
    ############################
    ## end of Clustering
    ############################

    ############################
    ## Jupyter Notebook
    ############################                    

    def createDf(self):
        if (debugPrints):
            print("Create dataframe for Jupiter and web")
        data_to_list = []
        for index, row in self.spectrum_list.iterrows():
            tmp = row['data'].tolist()
            data_to_list.append(tmp)
            if (debugPrints):
                print("len(data_to_list) --> ", row['names'], " ", len(tmp))

        if (debugPrints):
            print([list((i, len(data_to_list[i]))) for i in range(len(data_to_list))])
        self.spectrum_list = self.spectrum_list.drop('data', 1)
        self.spectrum_list['data'] = np.array(data_to_list)

        self.spectrum_list.to_csv(self.wConf.jup_df_filename.text())
        #self.spectrum_list.to_csv("df-updated.csv")
    
    def jupyterStop(self):
        # stop the notebook process
        log("Sending interrupt signal to jupyter-notebook")
        self.wConf.jup_start.setEnabled(True)
        self.wConf.jup_stop.setEnabled(False)        
        self.wConf.jup_start.setStyleSheet("background-color:#3CB371;")
        self.wConf.jup_stop.setStyleSheet("")        
        stopnotebook()

    def jupyterStart(self):
        # dump dataframe to compressed file
        #self.spectrum_list.to_pickle(self.wConf.jup_df_filename.text(), compression="bz2")                

        s = QSettings()
        execname = s.value(SETTING_EXECUTABLE, "jupyter-notebook")
        if not testnotebook(execname):
            while True:
                QMessageBox.information(None, "Error", "It appears that Jupyter Notebook isn't where it usually is. " +
                                        "Ensure you've installed Jupyter correctly and then press Ok to " +
                                        "find the executable 'jupyter-notebook'", QMessageBox.Ok)
                if testnotebook(execname):
                    break
                execname = QFileDialog.getOpenFileName(None, "Find jupyter-notebook executable", QDir.homePath())
                if not execname:
                    # user hit cancel
                    sys.exit(0)
                else:
                    execname = execname[0]
                    if testnotebook(execname):
                        log("Jupyter found at %s" % execname)
                        #save setting
                        s.setValue(SETTING_EXECUTABLE, execname)
                        break

        # setup logging
        # try to write to a log file, or redirect to stdout if debugging
        logname = "JupyterQtPy-"+time.strftime("%Y%m%d-%H%M%S")+".log"
        logfile = os.path.join(str(QDir.currentPath()), ".JupyterQtPy", logname)
        if not os.path.isdir(os.path.dirname(logfile)):
            os.mkdir(os.path.dirname(logfile))
            try:
                if DEBUG:
                    raise IOError()  # force logging to console
                setup_logging(logfile)
            except IOError:
                # no writable directory, log to console
                setup_logging(None)
                    
        # workdir
        directory = s.value(SETTING_BASEDIR, QDir.currentPath())

        # setting window
        view = WebWindow(None, None)
        view.setWindowTitle("Jupyter CutiePie: %s" % directory)

        # logging on docked console
        qtlogger = QtLogger(view)
        qtlogger.newlog.connect(view.loggerdock.log)
        set_logger(lambda message: qtlogger.newlog.emit(message))
        
        log("Setting home directory --> "+str(directory))
        
        # start the notebook process
        webaddr = startnotebook(execname, directory=directory)
        view.loadmain(webaddr)

        # resume regular logging
        setup_logging(logfile)
        
        self.wConf.jup_start.setEnabled(False)
        self.wConf.jup_stop.setEnabled(True)        
        self.wConf.jup_start.setStyleSheet("")        
        self.wConf.jup_stop.setStyleSheet("background-color:#DC143C;")
        
# redirect logging 
class QtLogger(QObject):
    newlog = pyqtSignal(str)
    
    def __init__(self, parent):
        super(QtLogger, self).__init__(parent)

        
