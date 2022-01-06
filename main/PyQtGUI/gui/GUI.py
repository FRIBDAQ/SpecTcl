#!/usr/bin/env python3
# import modules and packages
import importlib
import io, pickle, traceback, sys, os, subprocess
import signal, logging, ctypes, copy, json, httplib2, cv2
import threading, itertools, time, multiprocessing, math, re
from ctypes import *
from copy import copy
from itertools import chain, compress, zip_longest
import pandas as pd
import numpy as np

sys.path.append(os.getcwd())
sys.path.append("./Lib")
sys.path.append(str(os.environ.get("INSTDIR"))+"/Lib")

from PyQt5 import QtCore, QtNetwork
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

from sklearn import metrics
from sklearn.cluster import KMeans
from sklearn.mixture import GaussianMixture
from sklearn.preprocessing import StandardScaler

import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.lines as mlines
import matplotlib.mlab as mlab
import matplotlib.image as mpimg
import matplotlib.gridspec as gridspec

from matplotlib.patches import Polygon, Circle, Ellipse
from matplotlib.path import Path
from scipy.optimize import curve_fit
from scipy.signal import find_peaks
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from mpl_toolkits.axes_grid1 import make_axes_locatable
from mpl_toolkits.axes_grid1.inset_locator import inset_axes

import CPyConverter as cpy

# List of implementation topics
# 0) Class definition
# 1) Main layout GUI
# 2) Signals
# 3) Implementation of Signals
# 4) GUI on startup
# 5) Connection to REST for gates
# 6) Accessing the ShMem
# 7) Load/save geometry window
# 8) Zoom operations
# 9) Histogram operations
# 10) Gates
# 11) 1D/2D region integration
# 12) Fitting
# 13) Peak Finding
# 14) Clustering
# 15) Overlaying pic
# 16) Jupyter Notebook
# 17) Misc Tools

# import widgets
from MenuGUI import Menu #include server and mirror config, refresh, geometry, extra
from ConfigGUI import Configuration # include spectrum/gate info and output popup buttons
from SpecialFunctionsGUI import SpecialFunctions # all the extra functions we defined
from OutputGUI import OutputPopup # popup output window
from PlotGUI import Plot # area defined for the histograms 
from PyREST import PyREST # class interface for SpecTcl REST plugin

from logger import log, setup_logging, set_logger
from notebook_process import testnotebook, startnotebook, stopnotebook
from WebWindow import WebWindow

SETTING_BASEDIR = "workdir"
SETTING_EXECUTABLE = "exec"
DEBUG = False
DEBOUNCE_DUR = 0.25
t = None

# 0) Class definition
class MainWindow(QMainWindow):

    stop_signal = pyqtSignal()

    def __init__(self, factory, fit_factory, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        self.factory = factory
        self.fit_factory = fit_factory        

        self.setWindowTitle("CutiePie(QtPy)")

        #check if there are arguments or not
        try:
            self.args = dict(args)
            if (DEBUG):
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
        self.h_dict_bak = {}        
        self.h_dict_geo = {}  # for saving pane geometry
        self.h_setup = {} # bool dict for setting histograms (false - histogram modified, true - 
        # index of the histogram
        self.index = 0 # this one is for self-adding
        self.idx = 0 # this one is global
        # click selection of position in canvas
        self.selected_plot_index = None
        self.isSelected = False
        self.selected_row = None
        self.selected_col = None     
        self.old_row = 1
        self.old_col = 1        
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
        self.isCreated = False
        # tools for selected plots
        self.rec = None
        self.isCluster = False
        self.onFigure = False
        # dictionary for spectcl gates
        self.gate_dict = {}
        self.gateTypeDict = {}
        self.gateType = ""
        #dictionary for gates - key:gate name entry: line (with label)
        self.dict_region = {}
        self.counter = 0
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
        # 1) Main layout GUI
        #######################        
        
        mainLayout = QVBoxLayout()
        mainLayout.setContentsMargins(0,0,0,0)
        
        # top menu
        self.wTop = Menu()
        self.wTop.setFixedHeight(50)
        
        # config menu
        self.wConf = Configuration()
        self.wConf.setFixedHeight(50)
        
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

        # extra popup window
        self.extraPopup = SpecialFunctions()

        # initialize factory from algo_creator
        self.factory.initialize(self.extraPopup.imaging.clusterAlgo)
        # initialize factory from fit_creator
        self.fit_factory.initialize(self.extraPopup.fit_list)
        
        #################
        # 2) Signals
        #################

        # configuration signals
        self.at_startup()        
        
        # top menu signals
        self.wTop.updateButton.clicked.connect(self.update)                
        self.wTop.slider.valueChanged.connect(self.self_update)
        self.wTop.extraButton.clicked.connect(self.spfunPopup)                        
        self.wTop.saveButton.clicked.connect(self.saveGeo)
        self.wTop.loadButton.clicked.connect(self.loadGeo)
        self.wTop.exitButton.clicked.connect(self.closeAll)

        # config menu signals
        self.wConf.histo_geo_add.clicked.connect(self.add_plot)
        self.wConf.histo_geo_update.clicked.connect(self.update_plot)        
        self.wConf.histo_geo_delete.clicked.connect(self.clear_plot)        
        self.wConf.histo_geo_row.activated.connect(lambda: self.initialize_canvas(int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_col.activated.connect(lambda: self.initialize_canvas(int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.button2D_option.activated.connect(self.change_bkg)        
        self.wConf.histo_geo_all.stateChanged.connect(self.applyAll)
        
        self.wConf.listGate.currentIndexChanged.connect(self.updateGateType)
        self.wConf.histo_list.currentIndexChanged.connect(self.updateHistoInfo)
        
        self.wConf.createGate.clicked.connect(self.createGate)
        self.wConf.createGate.setEnabled(False)
        self.wConf.editGate.clicked.connect(self.editGate)
        self.wConf.editGate.setEnabled(False)
        self.wConf.editGate.setToolTip("Key bindings for editing a gate:\n"
                                      "'t' enable dragging mode\n"
                                      "'i' insert vertex\n"
                                      "'d' delete vertex\n"
                                      "'u' update gate\n")
        self.wConf.deleteGate.clicked.connect(self.deleteGate)                        
        self.wConf.drawGate.clicked.connect(self.drawGate)
        self.wConf.cleanGate.clicked.connect(self.clearGate)

        # output
        self.wConf.integrateGate.clicked.connect(self.integrate)        
        self.wConf.outputGate.clicked.connect(self.resultPopup)
        
        self.extraPopup.peak.peak_analysis.clicked.connect(self.analyzePeak)
        self.extraPopup.peak.peak_analysis_clear.clicked.connect(self.peakAnalClear)        

        self.extraPopup.peak.jup_start.clicked.connect(self.jupyterStart)
        self.extraPopup.peak.jup_stop.clicked.connect(self.jupyterStop)

        self.extraPopup.imaging.threshold_slider.valueChanged.connect(self.thresholdFigure)
        self.extraPopup.imaging.analyzerButton.clicked.connect(self.analyzeCluster)
        self.extraPopup.imaging.loadButton.clicked.connect(self.loadFigure)
        self.extraPopup.imaging.addButton.clicked.connect(self.addFigure)
        self.extraPopup.imaging.deleteButton.clicked.connect(self.deleteFigure)
        self.extraPopup.imaging.alpha_slider.valueChanged.connect(self.transFigure)
        self.extraPopup.imaging.zoomX_slider.valueChanged.connect(self.zoomFigureX)
        self.extraPopup.imaging.zoomY_slider.valueChanged.connect(self.zoomFigureY)        
        self.extraPopup.imaging.joystick.mousemoved.connect(self.moveFigure)
        self.extraPopup.imaging.upButton.clicked.connect(self.fineUpMove)
        self.extraPopup.imaging.downButton.clicked.connect(self.fineDownMove)
        self.extraPopup.imaging.leftButton.clicked.connect(self.fineLeftMove)
        self.extraPopup.imaging.rightButton.clicked.connect(self.fineRightMove)                
        
        self.extraPopup.fit_button.clicked.connect(self.fit)

        # for key_press_event
        self.wPlot.canvas.setFocusPolicy( QtCore.Qt.ClickFocus )
        self.wPlot.canvas.setFocus()
        
        # plotting signals
        self.wPlot.plusButton.clicked.connect(lambda: self.zoomIn(self.wPlot.canvas))
        self.wPlot.minusButton.clicked.connect(lambda: self.zoomOut(self.wPlot.canvas))        

        ################################        
        # 3) Implementation of Signals
        ################################        

    def closeAll(self):
        self.close()
        self.resPopup.close()
        self.extraPopup.close()
        
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
        if (DEBUG):
            print("Inside on_singleclick")
        # If we are not zooming on one histogram we can select one histogram
        # and a red rectangle will contour the plot
        if self.isZoomed == False:
            if (DEBUG):
                print("Inside on_singleclick - ZOOM false")            
            try:
                if self.rec is not None:
                    self.rec.remove()
            except:
                pass

            # by single clicking we select the plot with index selected_plot_index and draw a
            # red rectangle on the axes to show choice
            for i, plot in enumerate(self.wPlot.figure.axes):
                if (i == self.selected_plot_index):
                        self.isSelected = True
                        self.rec = self.create_rectangle(plot)
            try:
                self.clickToIndex(self.selected_plot_index)
            except:
                pass
            self.wPlot.canvas.draw()
        else: # we are inside the zoomed histogram
            if (DEBUG):
                print("Inside on_singleclick - ZOOM true")            
            self.isSelected = False
            try:
                if self.rec is not None:
                    self.rec.remove()
            except:
                pass

            # we can create the gate now..
            if self.toCreateGate == True: # inside gate creation mode            
                if (DEBUG):
                    print("create the gate...")
                self.click = [int(float(event.xdata)), int(float(event.ydata))]
                # create interval (1D)            
                if self.wConf.button1D.isChecked():
                    self.line = self.addLine(self.click[0])
                    self.listLine.append(self.line)
                    # removes the lines from the plot
                    if len(self.listLine) > 2:
                        self.removeLine()
                else:
                    if (DEBUG):                    
                        print("inside create gate 2d")
                    self.polygon = self.addPolygon(self.click[0], self.click[1])
                        
            self.wPlot.canvas.draw()
        t = None
            
    def on_dblclick(self, event):
        global t
        if (DEBUG):
            print("Inside on_dblclick")        
        t = None        
        if self.isZoomed == False: # entering zooming mode
            if (DEBUG):
                print("Entering zooming mode...")           
            self.isZoomed = True
            # disabling adding histograms
            self.wConf.histo_geo_add.setEnabled(False)
            # enabling gate creation
            self.wConf.createGate.setEnabled(True)                
            self.wConf.editGate.setEnabled(True)                                
            if (DEBUG):
                print("inside dblclick: self.selected_plot_index", self.selected_plot_index)
            self.wPlot.figure.clear()
            self.wPlot.canvas.draw()
            # backing up list of histogram
            self.h_dict_bak = self.h_dict.copy()
            # plot corresponding histogram
            if (DEBUG):
                print("plot the histogram at index", self.selected_plot_index, "with name", (self.h_dict[self.selected_plot_index])["name"])
            a = self.update_plot()
            self.removeCb(a)

        else:
            # enabling adding histograms
            self.wConf.histo_geo_add.setEnabled(True)
            # disabling gate creation
            self.wConf.createGate.setEnabled(False)                
            self.wConf.editGate.setEnabled(False)                                                
            
            if self.toCreateGate == True:
                if self.wConf.button1D.isChecked():
                    lst = []
                    # adding gate to dictionary of regions
                    for index in range(len(self.listLine)):
                        lst.append(self.listLine[index].get_xydata())
                    # push gate to spectcl
                    self.formatLinetoREST(lst)
                    # save the gate for drawing later
                    self.set_line(self.wConf.listGate.currentText(), self.listLine)
                    #adding to list of gates for redrawing
                    #key = self.wConf.spectrum_name.text()
                    key = self.wConf.histo_list.currentText()
                    self.artist1D[self.wConf.listGate.currentText()] = self.listLine
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
                    self.set_line(self.wConf.listGate.currentText(), [self.polygon])                    
                    if (DEBUG):
                        print(self.wConf.listGate.currentText(), "\n",self.polygon.get_xydata())
                    # push gate to shared memory 2D
                    self.formatLinetoREST(self.polygon)
                    #adding to list of gates for redrawing
                    #key = self.wConf.spectrum_name.text()
                    key = self.wConf.histo_list.currentText()
                    self.artist2D[self.wConf.listGate.currentText()] = self.polygon
                    self.artist_dict[key] = self.artist2D

                if (DEBUG):
                    print("Exiting gating mode...")
                    print(self.artist_dict)
                
                # exiting gating mode
                self.toCreateGate = False
                self.timer.start()
            else:
                if (DEBUG):
                    print("Exiting zooming mode...")                
                self.isCreated = False
                self.isZoomed = False
                if (DEBUG):
                    print("Reinitialization self.h_setup", self.h_setup)
                #draw the back the original canvas
                self.initialize_canvas(self.wConf.row, self.wConf.col)
                if (DEBUG):
                    print("ready to print")
                self.h_dict = self.h_dict_bak.copy()
                if (DEBUG):
                    print(self.h_dict)
                n = self.wConf.row*self.wConf.col
                if (DEBUG):
                    print(n)
                    print({k: True for k in range(self.wConf.row*self.wConf.col)})
                self.h_setup = {k: True for k in range(n)}
                self.selected_plot_index = None # this will allow to call drawGate and loop over all the gates
                self.update_plot()

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
            self.set_line(self.wConf.listGate.currentText(), [self.line_region])
            if (DEBUG):
                print("update key")
                print(self.wConf.listGate.currentText(), "\n", self.line_region.get_xydata())
            if (DEBUG):
                print("self.dict_region", self.dict_region)
            # push gate to shared memory 2D
            self.formatLinetoREST(self.line_region)
            # update gate
            gate = self.wConf.listGate.currentText()
            #key = self.wConf.spectrum_name.text()
            key = self.wConf.histo_list.currentText()
            self.artist2D[gate] = []
            self.artist2D[gate] = self.line_region
            self.artist_dict[key] = self.artist2D
            if (DEBUG):
                print("update gate \n", self.artist_dict)
            self.edit_disconnect()
            self.connect()
        elif event.key=='d':
            if not self.showverts:
                return;
            else:
                ind = self.get_ind_under_cursor(event)
                if ind is None:
                    return
                if ind == 0 or ind == self.last_vert_ind:
                    if (DEBUG):
                        print("Cannot delete root node")
                    return
                self.poly.xy = [tup for i,tup in enumerate(self.poly.xy)
                                if i!=ind]
                self._update_line()
        elif event.key=='i':
            if not self.showverts:
                return;
            else:
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
    # 4) GUI on startup
    ######################################

    def at_startup(self):
        self.initialize_canvas(self.wConf.row, self.wConf.col)
        self.connect()

    def initialize_histogram(self):
        return {"name": "empty", "dim": 1, "xmin": 0, "xmax": 1, "xbin": 1,
                "ymin": 0, "ymax": 1, "ybin": 1, "parameters": [], "type": ""}        
        
    def initialize_figure(self, grid):
        if (DEBUG):
            print("initialize_figure and dictionaries")
        # creating plots in the grid
        for i in range(self.wConf.row):
            for j in range(self.wConf.col):
                a = self.wPlot.figure.add_subplot(grid[i,j])
        self.h_dim.clear()
        self.h_lst.clear()
        for z in range(self.old_row*self.old_col):
            self.h_dict[z] = self.initialize_histogram()
            self.h_dict_geo[z] = "empty"
            # dictionary for setup flags (i.e. if an histogram is modified then we need to set it up first)
            self.h_setup[z] = False
            self.h_zoom_max.append(self.yhigh)
        self.h_dim = self.get_histo_key_list(self.h_dict, "dim")
        self.h_lst = self.get_histo_key_list(self.h_dict, "name")

        if (DEBUG):
            print(self.h_dict)
        
    def create_figure(self, row, col):
        self.wConf.row = row
        self.wConf.col = col
        if self.checkVersion(matplotlib.__version__) < self.checkVersion("2.0.0"):
            self.grid = gridspec.GridSpec(ncols=self.wConf.col, nrows=self.wConf.row)
        else:
            self.grid = gridspec.GridSpec(ncols=self.wConf.col, nrows=self.wConf.row, figure=self.wPlot.figure)
        return self.grid

    def isGeoChanged(self):
        if (self.old_row != int(self.wConf.histo_geo_row.currentText()) or self.old_col != int(self.wConf.histo_geo_col.currentText())):
            if (DEBUG):
                print("the geometry has changed")
            self.old_row = int(self.wConf.histo_geo_row.currentText())
            self.old_col = int(self.wConf.histo_geo_col.currentText())
            return True
        else:
            return False
        
    def initialize_canvas(self, row, col):
        # everytime a new pane geometry is created the histogram
        # dictionary and the index of the plot has to be reset
        if (DEBUG):
            print("initialize_canvas with dimensions", row, col)
            print("new", row, col)
            print("old", self.old_row, self.old_col)
        # check if geometry changed
        if (self.isGeoChanged()):
            self.h_dict.clear()
            self.h_dict_geo.clear()

        if (DEBUG):
            print(self.h_dict)

        self.isCreated = False
        self.index = 0
        self.idx = 0
        self.wPlot.figure.clear()
        self.initialize_figure(self.create_figure(row, col))
        self.wPlot.canvas.draw()
        
    ###############################################
    # 5) Connection to REST for gates
    ###############################################    

    def incrementNumbers(self, parameter, new_number):
        number = (re.findall(r'\d+',parameter))[0]
        out = parameter.replace(number,str(new_number).zfill(int(math.log10(self.nparams))+1))
        return out

    # to be rewritten completely using PyREST
    def formatLinetoREST(self, lst):
        name = self.wConf.listGate.currentText()
        boundaries = []
        parameters = []

        if self.wConf.button1D.isChecked():
            x, y = map(list, zip(*lst))
            x, y = map(list, zip(*x))
            low = min(x)
            high = max(x)
            boundaries = [low,high]
            # access list of parameters
            for index, value in self.h_dict.items():
                if (value["name"] ==  self.wConf.histo_list.currentText()):
                    if (self.gateType == "s"):
                        parameters = value["parameters"][0]
                    else:
                        for i in value["parameters"]:
                            parameters.append(i)
        else:
            for index, value in self.h_dict.items():
                if (value["name"] ==  self.wConf.histo_list.currentText()):
                    parameters = value["parameters"]
            points = lst.get_xydata()
            boundaries = (points[:-1]).tolist()
            
        if (DEBUG):
            print(boundaries)
            print(parameters)            

        self.rest.createGate(self.wConf.listGate.currentText(), self.gateType, parameters, boundaries)

    # this has to be modified to allow to load gate previously created and being able to apply them correctly
    def formatRESTToLine(self, name):
        if (DEBUG):        
            print("inside formatRESTToLine")
        lst_tmp = []
        x = []
        y = []
        #'type': 's', 'parameters': ['tdc.06'], 'low': 6281.0, 'high':        
        types = (self.gate_dict[name])["type"]
        parameters = (self.gate_dict[name])["parameters"]
        if (DEBUG):
            print(types, parameters)
        if types == "s" or types == "gs":
            xmin = (self.gate_dict[name])["low"]
            xmax = (self.gate_dict[name])["high"]
            if (DEBUG):
                print(xmin, xmax)
            l = mlines.Line2D([xmin, xmin], [self.ylow,self.yhigh])
            lst_tmp.append(l)
            l = mlines.Line2D([xmax, xmax], [self.ylow,self.yhigh])
            lst_tmp.append(l)            
        else:
            polygon = mlines.Line2D([],[])
            points = (self.gate_dict[name])["points"]
            if (DEBUG):
                print(points)
            for dic in points:
                if (DEBUG):
                    print(dic)
                for key, value in dic.items():
                    if (DEBUG):
                        print(key, value)
                    if key == "x":
                        x.append(value)
                    elif key == "y":
                        y.append(value)
            x.append(x[0])
            y.append(y[0])
            if (DEBUG):
                print(x,y)
            polygon.set_data(x,y)
            lst_tmp.append(polygon)
            
        # adding gate to dictionary of regions
        self.dict_region[name] = lst_tmp
        self.gateTypeDict[name] = types
        if (DEBUG):
            print("adding gate to dictionary of regions")
            print(self.dict_region)
            print("gate type dict", self.gateTypeDict)

    ##########################################
    # 6) Accessing the ShMem
    ##########################################

    def update(self):
        # trying to access the shared memory through SpecTcl Mirror Client
        try:
            # update host name and port, mirror port, and user name from GUI
            hostname = str(self.wTop.server.text())
            port = str(self.wTop.rest.text())
            user = str(self.wTop.user.text())
            mirror = str(self.wTop.mirror.text())

            # configuration of the REST plugin
            self.rest = PyREST(hostname,port)
            # set traces
            self.token =self.rest.startTraces(30)
            print("trace token", self.token)
            print(self.rest.pollTraces(self.token))
            
            if (hostname == "hostname" or port == "port" or mirror == "mirror"):
                raise ValueError("hostname/port/mirror are not configured!")
            
            if (DEBUG):
                print(hostname.encode('utf-8'), port.encode('utf-8'), user.encode('utf-8'), mirror.encode('utf-8'))
                print("before cpy.CPyConverter().Update")
            s = cpy.CPyConverter().Update(bytes(hostname, encoding='utf-8'), bytes(port, encoding='utf-8'), bytes(mirror, encoding='utf-8'), bytes(user, encoding='utf-8'))

            # creates a dataframe for spectrum info            
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

            # order the dataframe by id to avoid mismatch later on with id of new spectra
            self.spectrum_list = self.spectrum_list.sort_values(by=['id'], ascending=True)

            # replace names in the dataframe for the spectra
            nlst = self.getSpectrumNames()
            old_values = self.spectrum_list["names"].tolist()
            self.spectrum_list["names"] = self.spectrum_list["names"].replace(old_values, nlst)
            
            # update and create parameter, spectrum, and gate lists
            self.create_spectrum_list()                    
            self.create_parameter_list()
            self.create_gate_list()        
            self.update_spectrum_info()
            self.isCluster = False

            #self.createDf()
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started or hostname/port/mirror are not configured!")

    # extract correct spectrum name
    def getSpectrumNames(self):
        nlst = []
        lstdict = self.rest.listSpectrum()
        for el in lstdict:
            nlst.append(el["name"])

        return nlst
            
    # get parameter count
    def getParameterCount(self):
        lst_param = list(self.param_list.values())
        lst_param = [x for x in lst_param if any(c.isdigit() for c in x)]
        res = max(list(map(lambda sub:int(''.join([ele for ele in sub if ele.isnumeric()])), lst_param)))
        return res

    # update and create spectrum list 
    def update_spectrum_parameters(self):
        try:
            spec_dict = self.rest.listSpectrum()
            tmppar = []
            tmppar2 = []            
            for dic in spec_dict:
                for key in dic:
                    if key == 'parameters':
                        tmppar.append(dic[key]) 
                    if key == 'type':
                        tmppar2.append(dic[key])                         

            # adds list of parameters to dataframe           
            self.spectrum_list['parameters'] = tmppar            
            # add list of types to dataframe
            self.spectrum_list['type'] = tmppar2            
            if (DEBUG):
                print(self.spectrum_list['parameters'])
                print("Reset QComboBox")
            # resetting ComboBox 
            self.wConf.histo_list.clear()
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started...")

    def create_spectrum_list(self):
        self.update_spectrum_parameters()
        for name in self.spectrum_list['names']:
            if self.wConf.histo_list.findText(name) == -1:
                self.wConf.histo_list.addItem(name)
            
    # update and create parameter list
    def update_parameter_list(self):
        try:
            par_dict = self.rest.listParameter()
            tmpid = []
            tmpname = []
            for dic in par_dict:
                for key in dic:
                    if key == 'id':
                        tmpid.append(dic[key])
                    elif key == 'name':
                        tmpname.append(dic[key])
            ziplst = zip(tmpid, tmpname)
            self.param_list = dict(ziplst)
            # resetting ComboBox 
            for i in range(2):
                self.wConf.listParams[i].clear()
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started...")

    def create_parameter_list(self):
        self.update_parameter_list()
        for key, value in self.param_list.items():
            for i in range(2):
                if self.wConf.listParams[i].findText(value) == -1:
                    self.wConf.listParams[i].addItem(value)

    # update and create gate list
    def update_gate_list(self):
        try:
            if (DEBUG):
                print("Inside update_gate_list")
            gate_list = self.rest.listGate()
            self.gate_dict = {d["name"]: d for d in gate_list}

            # resetting ComboBox
            self.wConf.listGate.clear()
        except:
            pass

    def create_gate_list(self):
        if (DEBUG):
            print("Inside create_gate_list")
        self.update_gate_list()
        for key, value in self.gate_dict.items():
            if (DEBUG):
                print(key,"->", value)
            if self.wConf.listGate.findText(key) == -1:
                self.wConf.listGate.addItem(key)        
            self.formatRESTToLine(key)
        if (DEBUG):
            print("Inside update_gate_list")
            print(self.gate_dict)
        self.updateGateType()
        
    # update spectrum information
    def updateHistoInfo(self):
        try:
            hist_name = self.wConf.histo_list.currentText()
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
        except:
            pass
        
    # add parameter list and spectrum type 
    def update_spectrum_info(self):
        hist_dim = 0
        hist_type = ""
        hist_params = []
        hist_tmp = {}
        hist_name = str(self.wConf.histo_list.currentText())
        hist_minx = 0
        hist_maxx = 0
        hist_binx = 0
        hist_miny = 0
        hist_maxy = 0
        hist_biny = 0
        try:
            # update info on gui for spectrum name
            # self.wConf.spectrum_name.setText(hist_name)
            # extra data from data frame to fill up the gui
            select = self.spectrum_list['names'] == hist_name
            df = self.spectrum_list.loc[select]
            hist_dim = df.iloc[0]['dim']
            hist_type = df.iloc[0]['type']
            hist_params = df.iloc[0]['parameters']
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
                    hist_minx = str(df.iloc[0]['minx'])
                    hist_maxx = str(df.iloc[0]['maxx'])
                    hist_binx = str(df.iloc[0]['binx'])
                else :
                    hist_miny = str(df.iloc[0]['miny'])
                    hist_maxy = str(df.iloc[0]['maxy'])
                    hist_biny = str(df.iloc[0]['biny'])
            hist_tmp = {"name": hist_name, "dim": hist_dim, "xmin": hist_minx, "xmax": hist_maxx, "xbin": hist_binx,
                        "ymin": hist_miny, "ymax": hist_maxy, "ybin": hist_biny, "parameters": hist_params, "type": hist_type}
            return hist_tmp
        except:
            QMessageBox.about(self, "Warning", "Please click 'Get Data' to access the data...")

    def update_plot(self):
        if (DEBUG):
            print("inside update plot")
            print(self.h_dict)
        try:
            a = None
            if self.isZoomed == True:
                self.add(self.selected_plot_index) # this creates the histogram axes
                self.wPlot.canvas.draw() # this drawing command creates the renderer
                a = plt.gca()
                time.sleep(0.01)
                if (DEBUG):
                    print("has it changed?", self.h_setup[self.selected_plot_index])
                    print(self.h_setup)
                if (self.h_setup[self.selected_plot_index]):
                    self.setup_histogram(a, self.selected_plot_index)                
                    self.h_setup[self.selected_plot_index] = False
                self.plot_histogram(a, self.selected_plot_index) # the previous step is fundamental for blitting
                self.removeCb(a)
            else:
                self.selected_plot_index = None
                for index, value in self.h_dict.items():                
                    if (DEBUG):
                        print(index, value)
                        print(value["name"])
                        print(self.h_setup[index])
                    if (value["name"] != "empty"):
                        a = self.select_plot(index)
                        time.sleep(0.01)
                        if (self.h_setup[index]):
                            self.setup_histogram(a, index)
                            self.h_setup[index] = False
                        self.plot_histogram(a, index)
                        
            self.wPlot.figure.tight_layout()
            self.wPlot.canvas.draw_idle()

            #reset index
            self.selected_plot_index = None
            if (DEBUG):
                print("Reset selected index", self.selected_plot_index)
            return a
                
        except:
            pass

    def self_update(self):
        self.wTop.slider_label.setText("Refresh interval ({} s)".format(self.wTop.slider.value()))
        if self.wTop.slider.value() != 0:
            self.timer.setInterval(1000*int(self.wTop.slider.value()))
            # this line below depends on the REST trace
            #self.timer.timeout.connect(self.update)
            self.timer.timeout.connect(self.update_plot)
            self.timer.start()
        else:
            self.timer.stop()
        
            
    ##########################################
    # 7) Load/save geometry window
    ##########################################    
    
    def saveGeo(self):
        fileName = self.saveFileDialog()
        try:
            f = open(fileName,"w")
            tmp = {"row": self.wConf.row, "col": self.wConf.col, "geo": self.h_dict_geo}
            if (DEBUG):
                print(tmp)
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
                self.h_dict_geo = infoGeo["geo"]
                if len(self.h_dict_geo) == 0:
                    QMessageBox.about(self, "Warning", "You saved an empty pane geometry...")
                if (DEBUG):
                    print(self.h_dict_geo)
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
            
    ############################
    # 8) Zoom operations
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
        
    ##################################
    ## 9) Histogram operations
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

    # get histo type
    def get_histo_type(self, index):
        return self.h_dict[index]["type"]

    # get histo parameters
    def get_histo_parameters(self, index):
        return self.h_dict[index]["parameters"]
    
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

    # looking for first available index to add an histogram
    def check_index(self):
        if (DEBUG):
            print("inside check index")
        keys=list(self.h_dict.keys())
        values = []
        for index, value in self.h_dict.items():
            values.append(value["name"])

        if (DEBUG):
            print(keys, values)
        if "empty" in values:
            self.index = keys[values.index("empty")]
        else:
            if (DEBUG):
                print("list is full, set index to full")
            self.index = keys[-1]
            self.isFull = True

        if self.isFull == True:
            if self.index == self.wConf.row*self.wConf.col-1:
                self.index = 0
            else:
                self.index += 1            

        if (DEBUG):
            print("index to fill", self.index)
        return self.index

    # remove colorbar
    def removeCb(self, axis):
        im = axis.images
        if im is not None:
            try:
                cb = im[-1].colorbar
                cb.remove()
            except IndexError:
                pass
    
    # geometrically add plots to the right place
    def add_plot(self):
        if (DEBUG):
            print("Inside plot")
        try:
            # if we load the geometry from file
            if self.isLoaded:
                counter = 0
                for key, value in self.h_dict_geo.items():
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
                if (DEBUG):
                    print("Inside plot - not loaded")
                # self adding
                if self.isSelected == False:
                    if (DEBUG):
                        print("Inside plot - self adding")                    
                    self.idx = self.check_index()
                # position selected by user
                else:
                    if (DEBUG):
                        print("Inside plot - user adding")                                        
                    self.idx = self.selected_plot_index

                if (DEBUG):
                    print("Adding plot at index ", self.idx)
                    
                # updating histogram dictionary for fast access to information via get_histo_xxx       
                self.h_dict[self.idx] = self.update_spectrum_info()        
                self.h_dict_geo[self.idx] = (self.h_dict[self.idx])["name"]
                self.h_dim[self.idx] = (self.h_dict[self.idx])["dim"]
                if (DEBUG):
                    print(self.h_dict)
                self.h_dict_bak = self.h_dict.copy()
                
                self.erase_plot(self.idx)            
                self.add(self.idx)
                self.wPlot.canvas.draw()
                self.isSelected = False
        except:
            QMessageBox.about(self, "Warning", "Please click 'Get Data' to access the data...")

    # geometrically add plots to the right place and calls plotting    
    def add(self, index):
        if (DEBUG):
            print("adding histogram...")
        self.h_setup[index] = True # ready to be setup
        if (DEBUG):
            print(self.h_setup)
        if self.isZoomed:
            a = plt.gca()
            self.erase_plot(index)
        else:
            a = self.select_plot(index)
            x,y = self.plot_position(index)
            if (DEBUG):
                print("in position",x,y)
            self.erase_plot(index)
            a = self.wPlot.figure.add_subplot(self.grid[x,y])            
        
    # erase plot
    def erase_plot(self, index):
        a = None
        try:
            if self.isZoomed:
                a = plt.gca()
            else:
                a = self.select_plot(index)
            # if 2d histo I need a bit more efforts for the colorbar
            if self.h_dim[index] == 2:
                self.removeCb(a)
            a.clear()
            return a
        except:
            pass

    # deletes all the plots and reinitializes the canvas
    def clear_plot(self):
        if self.forAll == True:
            self.rest.spectrumAllClear()
        else:
            name = self.wConf.histo_list.currentText()
            self.rest.spectrumClear(name)

        self.wPlot.canvas.draw()

    # sets up histogram
    def setup_histogram(self, axis, index):
        try:
            if (DEBUG):
                print("Inside setup histogram")
            hdim = self.get_histo_dim(index)                    
            minx = self.get_histo_xmin(index)
            maxx = self.get_histo_xmax(index)
            binx = self.get_histo_xbin(index)
            if (DEBUG):
                print(hdim,minx,maxx,binx)
        
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
            if (DEBUG):
                print(self.h_lst)
        except:
            pass
        
    # histo plotting
    def plot_histogram(self, axis, index, threshold=0.1):
        hdim = self.get_histo_dim(index)                    
        minx = self.get_histo_xmin(index)
        maxx = self.get_histo_xmax(index)
        binx = self.get_histo_xbin(index)
        if (DEBUG):
            print("inside plot_histogram")
        w = self.get_data(index)
        if (DEBUG):
            print("data",sum(w))
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
        if self.isCreated == False:
            if hdim == 2:
                divider = make_axes_locatable(axis)
                cax = divider.append_axes('right', size='5%', pad=0.05)
                # add colorbar
                self.wPlot.figure.colorbar(self.h_lst[index], cax=cax, orientation='vertical')
                self.isCreated = True
                
        self.wPlot.canvas.draw_idle()

    # check histogram dimension from GUI
    def check_histogram(self):
        if self.wConf.button1D.isChecked():
            self.create_disable2D(True)
        else:
            self.create_disable2D(False)

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

        if (DEBUG):            
            print("hist:", hname)
            print("hdim:", hdim)
            print("data for ", hname)
            print(type(w))
            print(w)
            print("sum ", sum(w), "len", len(w))
        
        if hdim == 1:
            empty = sum(w)
        else:
            if (DEBUG):
                print(len(w[0]))
            empty = len(w[0])

        if (empty == 0):
            self.isEmpty = True
            QMessageBox.about(self, "Warning", "Please attach to a ring or load an evt file...")
        else:
            self.isEmpty = False
            return w
            
    ##############
    # 10) Gates 
    ##############

    # helper function that converts index of geometry into index of histo list and updates info
    def clickToIndex(self, idx):
        index = self.wConf.histo_list.findText(str(self.h_dict[idx]['name']), QtCore.Qt.MatchFixedString)
        # this is to avoid the histogram combobox to not show the first histogram at first
        if index < 0:
            index = 0
        if (DEBUG):
            print("index of combobox", index)
        self.wConf.histo_list.setCurrentIndex(index)
        name = self.get_histo_name(idx)
        dim = self.get_histo_dim(idx)
        minx = str(self.get_histo_xmin(idx))
        maxx = str(self.get_histo_xmax(idx))
        binx = str(self.get_histo_xbin(idx))
        miny = str(self.get_histo_ymin(idx))
        maxy = str(self.get_histo_ymax(idx))
        biny = str(self.get_histo_ybin(idx))

        if dim == 1:
            self.wConf.button1D.setChecked(True)
        else:
            self.wConf.button2D.setChecked(True)
        self.check_histogram();
    
    def createGate(self):
        if self.wTop.slider.value() != 0:
            self.timer.stop()         

        # check for histogram existance
        name = self.wConf.histo_list.currentText()
        gate_name = self.wConf.listGate.currentText()
        gate_parameter = self.wConf.listParams[0].currentText()
        
        if (name==""):
            return QMessageBox.about(self,"Warning!", "Please create at least one spectrum")
        else:
            # creating entry in combobox if it doesn't exist
            allItems = [self.wConf.listGate.itemText(i) for i in range(self.wConf.listGate.count())]
            result = gate_name in allItems
            while result:
                self.counter += 1
                gate_name = "default_gate_"+str(self.counter)
                result = gate_name in allItems

            # gate name
            text, okPressed = QInputDialog.getText(self, "Gate name", "Please choose a name for the gate:", QLineEdit.Normal, gate_name)
            if okPressed:
                if text:
                    gate_name = text
                else:
                    gate_name = "default_gate_"+str(self.counter)
            else:
                gate_name = "default_gate_"+str(self.counter)
                if (DEBUG):
                    print("You clicked cancel but I saved you man...gate_name is", gate_name)
            self.wConf.listGate.addItem(gate_name)
            # update boxes
            self.wConf.listGate.setCurrentText(gate_name)
                
            # gate type
            items = None
            if self.wConf.button1D.isChecked():
                items = ("s", "gs")
            else:
                items = ("c", "b", "gc", "gb")

            item, okPressed = QInputDialog.getItem(self, "Gate type", "Please choose a type for the gate:", items, 0, False)
            if okPressed:
                self.gateType = item
                
        # adding gate
        self.gateTypeDict[gate_name] = self.gateType
        if (DEBUG):
            print(self.gateTypeDict)
        gindex = self.findGateType(gate_name)
        if (DEBUG):
            print("index of the gate in combobox", gindex)
        self.wConf.listGate_type.setCurrentIndex(gindex)
              
        self.toCreateGate = True;
        self.createRegion()
        self.polygon = self.createPolygon()
        if (DEBUG):
            print("end of create gate")

    def findGateType(self, gate_name):
        # if gate is loaded and doesn't exist then add it beforehand
        
        gate_type = self.gateTypeDict[gate_name]
        gate_index = self.wConf.listGate_type.findText(gate_type, QtCore.Qt.MatchFixedString)
        if gate_index < 0:
            gate_index = 0
        return gate_index
        
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
        if (DEBUG):
            print("Inside addPolygon", posx, posy)
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
        name_gate = self.wConf.listGate.currentText()
        polygon = (self.get_line(name_gate))[0]
        self.polygon.set_visible(False)
        polygon.set_visible(False)
        self.poly_xy = self.convertToList2D(polygon)
        if (DEBUG):
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
            
    def deleteGate(self):
        try:
            if self.wTop.slider.value() != 0:
                self.timer.stop()
            # delete from combobox and change index
            name = self.wConf.listGate.currentText()
            index = self.wConf.listGate.findText(name, QtCore.Qt.MatchFixedString)
            if index >= 0:
                self.wConf.listGate.removeItem(index)
            # delete from spectcl
            self.rest.deleteGate(name)
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
                if (DEBUG):
                    print(k2, artist)
                if isList:
                    for art in artist:
                        if (DEBUG):
                            print("removing ->", art)
                        art.remove()
                else:
                    if (DEBUG):                    
                        print("removing ->", artist)                    
                    artist.remove()

        self.wPlot.canvas.draw()            

        if self.wTop.slider.value() != 0:
            self.timer.start()

    def clearGate(self):
        if self.wTop.slider.value() != 0:
            self.timer.stop() 

        #self.wConf.resetGateType()
        #hname = self.wConf.spectrum_name.text()
        hname = self.wConf.histo_list.currentText()
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
        if (DEBUG):
            print(self.artist_dict)
        
        if self.wTop.slider.value() != 0:
            self.timer.start()

    def existGate(self, hname):
        name = self.wConf.listGate.currentText()
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

        if (DEBUG):
            print("Inside plot1DGate", key)
            
        if key in self.artist_dict:
            if (DEBUG):
                print("self.artist_dict[key]",self.artist_dict[key])
            if any(x == name for x in self.artist_dict[key]):
                if (DEBUG):
                    print("Replace the gate")
                    print("self.artist1D[",name,"] = []")
                self.artist1D[name] = []                

        if (key != 'empty'):
            self.artist1D[name] = tmp
            self.artist_dict[key] = self.artist1D
        if (DEBUG):
            print(self.artist_dict)
        
    def plot2DGate(self, axis, key, name):
        new_line = mlines.Line2D([],[])
        for line in self.dict_region[name]:                
            self.copy_attributes(new_line, line, self.copied_line_attributes)
            # when drawing, we then allow to edit if needed
            self.polygon = new_line
            new_line.set_color('r')
            axis.add_artist(new_line)

        if key in self.artist_dict:
            if (DEBUG):
                print("self.artist_dict[key]",self.artist_dict[key])
            if any(x == name for x in self.artist_dict[key]):
                if (DEBUG):
                    print("Replace the gate")
                    print("self.artist2D[",name,"] = []")
                self.artist2D[name] = []                

        if (key != 'empty'):            
            self.artist2D[name] = new_line
            self.artist_dict[key] = self.artist2D
        
    def drawAllGate(self):
        if (DEBUG):
            print("Inside drawAllGate")
        if self.wTop.slider.value() != 0:
            self.timer.stop()
        
        if self.wTop.slider.value() != 0:
            self.timer.start()

    def updateGateType(self):
        try: 
            gate_name = self.wConf.listGate.currentText()
            gindex = self.findGateType(gate_name)
            self.wConf.listGate_type.setCurrentIndex(gindex)
        except:
            pass
        
    def drawGate(self):
        if (DEBUG):
            print("inside drawGate")
        if self.wTop.slider.value() != 0:
            self.timer.stop()

        if self.selected_plot_index is not None:
            name = self.wConf.listGate.currentText()
            key = self.get_histo_name(self.selected_plot_index)
            gtype = self.gateTypeDict[name]
            if self.isZoomed:
                a = plt.gca()
            else:
                a = self.select_plot(self.selected_plot_index)
            if (self.wConf.button1D.isChecked() and (gtype == "s" or gtype == "gs")):
                #if (DEBUG):
                print("does the gate", name, "of type", gtype, " exists in", key, "?", self.existGate(key))
                self.plot1DGate(a, key, name)
            else:
                if (DEBUG):                        
                    print("does the gate", name, "of type", gtype, " exists in", key, "?", self.existGate(key))
                self.plot2DGate(a, key, name)

        self.wPlot.canvas.draw()
        if self.wTop.slider.value() != 0:
            self.timer.start()        

    ##############################
    # 11) 1D/2D region integration 
    ##############################
    
    def integrate(self):
        try:
            histo_name = self.wConf.histo_list.currentText()
            gate_name = self.wConf.listGate.currentText()
            results = self.rest.integrateGate(histo_name, gate_name)
            self.addRegion(results)
        except:
            QMessageBox.about(self, "Warning", "No gate defined for integration...")

    def resultPopup(self):
        self.resPopup.setGeometry(100,100,724,500)
        self.resPopup.show()

    def spfunPopup(self):
        self.extraPopup.show()
        
    def addRegion(self, results):
        # Schema for output window
        #["ID", "Spectrum", "Name", "centroid X", "centroid Y", "FWHM X", "FWHM Y", "Area"]
        self.resPopup.tableWidget.setRowCount(0);
        if self.wConf.button1D.isChecked():
            self.table_row.append([str(self.wConf.listGate.currentIndex()),
                                   str(self.wConf.histo_list.currentText()),
                                   str(self.wConf.listGate.currentText()),
                                   str(results["centroid"]),
                                   "",
                                   str(results["fwhm"]),
                                   "",
                                   str(results["counts"])])
        else:
            self.table_row.append([str(self.wConf.listGate.currentIndex()),
                                   str(self.wConf.histo_list.currentText()),
                                   str(self.wConf.listGate.currentText()),
                                   str((results["centroid"])[0]),
                                   str((results["centroid"])[1]),
                                   str(results["fwhm"][0]),
                                   str(results["fwhm"][1]),
                                   str(results["counts"])])

        for row in self.table_row:
            inx = self.table_row.index(row)
            self.resPopup.tableWidget.insertRow(inx)
            for i in range(len(row)):
                self.resPopup.tableWidget.setItem(inx,i,QTableWidgetItem(str(row[i])))
        header = self.resPopup.tableWidget.horizontalHeader()       
        header.setSectionResizeMode(QHeaderView.Stretch)        
        self.resPopup.tableWidget.resizeColumnsToContents()
        
    ############################
    # 12)  Fitting
    ############################    
    
    def axislimits(self, ax):
        left, right = ax.get_xlim()
        if self.extraPopup.fit_range_min.text():
            left = int(self.extraPopup.fit_range_min.text())
        if self.extraPopup.fit_range_max.text():            
            right = int(self.extraPopup.fit_range_max.text())            
        return left, right

    def fit(self):
        x = []
        y = []
        x_fit = []
        y_fit = []
        histo_name = str(self.wConf.histo_list.currentText())
        fit_funct = self.extraPopup.fit_list.currentText()
        if self.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(self.selected_plot_index)

        config = self.fit_factory._configs.get(fit_funct)
        if (DEBUG):
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
                index = self.wConf.histo_list.findText(str(self.h_dict[self.idx]['name']), QtCore.Qt.MatchFixedString)
                minx = str(self.get_histo_xmin(self.idx))
                maxx = str(self.get_histo_xmax(self.idx))
                binx = str(self.get_histo_xbin(self.idx))

                # giordano
                #xtmp = self.create_range(int(float(self.wConf.listParams_bins[0].text())), int(float(self.wConf.listParams_low[0].text())), int(float(self.wConf.listParams_high[0].text())))
                xtmp = self.create_range(int(binx), int(minx), int(maxx))

                self.fitpar = [float(self.extraPopup.fit_p0.text()), float(self.extraPopup.fit_p1.text()),
                               float(self.extraPopup.fit_p2.text()), float(self.extraPopup.fit_p3.text()),
                               float(self.extraPopup.fit_p4.text()), float(self.extraPopup.fit_p5.text()),
                               float(self.extraPopup.fit_p6.text()), float(self.extraPopup.fit_p7.text())]

                if (DEBUG):
                    print(self.fitpar)
                
                select = self.spectrum_list['names'] == histo_name
                df = self.spectrum_list.loc[select]
                ytmp = df.iloc[0]['data']
                xmin, xmax = self.axislimits(ax)
                if self.extraPopup.fit_range_min.text():
                    xmin = float(self.extraPopup.fit_range_min.text())
                if self.extraPopup.fit_range_max.text():
                    xmax = float(self.extraPopup.fit_range_max.text())
                if (DEBUG):
                    print("Sub range:", xmin, xmax)
                # create new tmp list with subrange for fitting
                for i in range(len(xtmp)):
                    if (xtmp[i]>=xmin and xtmp[i]<xmax):
                        x.append(xtmp[i])
                        y.append(ytmp[i])
                x = np.array(x)
                y = np.array(y)
                try:
                    self.fitln = fit.start(x, y, xmin, xmax, self.fitpar, ax, self.extraPopup.fit_results)
                except:
                    pass
            else:
                if (DEBUG):
                    print("2D fitting is not implemented")                

        self.wPlot.canvas.draw()

    ############################
    # 13) Peak Finding
    ############################

    def peakState(self, state):
        for i, btn in enumerate(self.extraPopup.peak.peak_cbox):
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

        self.wPlot.canvas.draw()                

    def create_peak_signals(self, peaks):
        for i in range(len(peaks)):
            self.isChecked[i] = False                    
            self.extraPopup.peak.peak_cbox[i].stateChanged.connect(self.peakState)
            self.extraPopup.peak.peak_cbox[i].setChecked(True)
            
    def update_peak_output(self, peaks, properties):
        for i in range(len(peaks)):
            s = "Peak"+str(i+1)+"\n\tpeak @ " + str(peaks[i])+", FWHM="+str(int(properties['widths'][i]))
            self.extraPopup.peak.peak_results.append(s)

    def peakAnalClear(self):
        self.extraPopup.peak.peak_results.clear()
        self.removeAllPeaks()
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
            for i in range(len(self.peaks)):
                self.extraPopup.peak.peak_cbox[i].setChecked(False)
                self.isChecked[i] = False
        except:
            pass

        self.wPlot.canvas.draw()

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
            width = int(self.extraPopup.peak.peak_width.text())
            minx = self.get_histo_xmin(self.selected_plot_index)
            maxx = self.get_histo_xmax(self.selected_plot_index)
            binx = self.get_histo_xbin(self.selected_plot_index)
            X = self.create_range(binx, minx, maxx)
            self.dataw = self.get_data(self.selected_plot_index)
            self.peaks, self.properties = find_peaks(self.dataw, prominence=1, width=width)
            
            self.update_peak_output(self.peaks, self.properties)
            self.create_peak_signals(self.peaks)

        except:
            pass


    ############################
    # 14) Clustering
    ############################                

    def initializeCluster(self):

        self.clusterpts = []
        self.clusterw = []                
        
        # histo name
        name_histo = self.wConf.histo_list.currentText()
        # converts data to points
        select = self.spectrum_list['names'] == name_histo
        df = self.spectrum_list.loc[select]
        w = df.iloc[0]['data']
        
        # convert np array to matrix
        m = np.asmatrix(w)

        print(m.shape[0],m.shape[1])
        
        histo_minx = int(df.iloc[0]['minx'])
        histo_maxx = int(df.iloc[0]['maxx'])
        histo_binx = int(df.iloc[0]['binx'])-2
        histo_miny = int(df.iloc[0]['miny'])
        histo_maxy = int(df.iloc[0]['maxy'])
        histo_biny = int(df.iloc[0]['biny'])-2

        a = None
        if self.isZoomed:
            a = plt.gca()
        else:
            a = self.select_plot(self.selected_plot_index)

        xmin, xmax = a.get_xlim()            
        ymin, ymax = a.get_ylim()
        print(xmin, xmax, ymin, ymax)

        polygon = Polygon([(xmin,ymin), (xmax,ymin), (xmax,ymax), (xmin,ymax)])
        # remove the duplicated last vertex
        poly = polygon.xy[:-1]
        p = Path(poly)

        # set value for threshold and algo
        self.old_threshold = self.extraPopup.imaging.threshold_slider.value()
        self.old_algo = self.extraPopup.imaging.clusterAlgo.currentText()

        print(histo_minx, histo_maxx, histo_binx, histo_miny, histo_maxy, histo_biny)

        # convert bin coordinates to axis coordinates        
        x, y = np.meshgrid(np.arange(histo_minx, histo_maxx, int((histo_maxx-histo_minx+1)/histo_binx), dtype=int),
                           np.arange(histo_miny, histo_maxy, int((histo_maxy-histo_miny+1)/histo_biny), dtype=int)) # make a canvas with coordinates in bins
        x, y = x.flatten(), y.flatten()
        print(len(x),len(y))
        points = np.vstack((x, y)).T
        isInside = p.contains_points(points)

        self.clusterpts = list(compress(points, isInside))
        for point in self.clusterpts:
            # convert x and y in bin units
            x = int((point[0]-histo_minx)*(histo_binx/(histo_maxx-histo_minx)))
            y = int((point[1]-histo_miny)*(histo_biny/(histo_maxy-histo_miny)))
            #print(x,y)
            val = m[(y,x)]
            self.clusterw.append(val)

        # creating masking list
        mask_val = [int(self.extraPopup.imaging.threshold_slider.value())]*len(self.clusterpts)

        mask = []
        for x, y in zip(mask_val, self.clusterw):
            if x < y:
                mask.append(True)
            else:
                mask.append(False)                

        if (DEBUG):                
            print("before masking...")        
            print(len(self.clusterpts), len(self.clusterw))                
       
        self.clusterpts = list(compress(self.clusterpts, mask))
        self.clusterw = list(compress(self.clusterw, mask))
        
        if (DEBUG):
            print("after masking...")        
            print(len(self.clusterpts), len(self.clusterw))
            print(self.clusterpts[0:10], self.clusterw[0:10])
        
        print("Done initializing cluster")
        self.isCluster = True

    def analyzeCluster(self):
        try:
            a = None
            if self.isZoomed:
                a = plt.gca()
            else:
                a = self.select_plot(self.selected_plot_index)

            # remove ellipses on plot if they exist
            if a.patches:
                a.patches = []
                self.wPlot.canvas.draw()
                
            # check if new algo has been called 
            algo = self.extraPopup.imaging.clusterAlgo.currentText()        
            if (algo != self.old_algo):
                self.isCluster = False

            # initialization of the clusters (if needed)
            if (self.isCluster == False or (self.old_threshold != self.extraPopup.imaging.threshold_slider.value())) and self.wConf.button2D.isChecked():
                self.start = time.time()
                self.initializeCluster()
                self.stop = time.time()
                print("Time elapsed for initialization of clustering:", self.stop-self.start)

            cluster_center=[]
            nclusters = int(self.extraPopup.imaging.clusterN.currentText())

            self.start = time.time()

            config = self.factory._configs.get(algo)
            print("ML algo config", config)
            MLalgo = self.factory.create(algo, **config)
            # add hooks for popup windows i.e. more arguments that won't be used
            MLalgo.start(self.clusterpts, self.clusterw, nclusters, a, self.wPlot.figure)

            self.stop = time.time()
            print("Time elapsed for clustering:", self.stop-self.start)

            self.wPlot.canvas.draw()
        except:
            QMessageBox.about(self, "Warning", "Is it a 2D histogram?")
            
    def thresholdFigure(self):
        self.extraPopup.imaging.threshold_label.setText("Threshold Level ({})".format(self.extraPopup.imaging.threshold_slider.value()))
        try:
            a = None
            if self.isZoomed:
                a = plt.gca()
            else:
                a = self.select_plot(self.selected_plot_index)

            self.plot_histogram(a, self.selected_plot_index,self.extraPopup.imaging.threshold_slider.value())
            self.wPlot.canvas.draw()
        except:
            pass

    ############################
    # 15) Overlaying pic
    ############################                

    def openFigureDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getOpenFileName(self,"Open file...", "","Image Files (*.png *.jpg);;All Files (*)", options=options)
        if fileName:
            return fileName
    
    def loadFigure(self):
        fileName = self.openFigureDialog()
        self.extraPopup.imaging.loadLISE_name.setText(fileName)
        print(fileName)
        if os.path.isfile(fileName):
            self.LISEpic = cv2.imread(fileName, 0)
            cv2.resize(self.LISEpic, (200, 100))
        else:
            print ("The file " + fileName + " does not exist.")
        
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
        if (DEBUG):
            print(self.extraPopup.imaging.joystick.direction, self.extraPopup.imaging.joystick.distance)
        try:
            self.imgplot.remove()        
            if self.extraPopup.imaging.joystick.direction == "up":
                self.ystart += self.extraPopup.imaging.joystick.distance*0.03
            elif self.extraPopup.imaging.joystick.direction == "down":
                self.ystart -= self.extraPopup.imaging.joystick.distance*0.03
            elif self.extraPopup.imaging.joystick.direction == "left":
                self.xstart -= self.extraPopup.imaging.joystick.distance*0.03
            else:
                self.xstart += self.extraPopup.imaging.joystick.distance*0.03        
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
        self.alpha = self.extraPopup.imaging.alpha_slider.value()/10
        self.zoomX = self.extraPopup.imaging.zoomX_slider.value()/10
        self.zoomY = self.extraPopup.imaging.zoomY_slider.value()/10        
        
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
        self.extraPopup.imaging.alpha_label.setText("Transparency Level ({} %)".format(self.extraPopup.imaging.alpha_slider.value()*10))
        try:
            self.deleteFigure()
            self.drawFigure()
        except:
            pass

    def zoomFigureX(self):
        self.extraPopup.imaging.zoomX_label.setText("Zoom X Level ({} %)".format(self.extraPopup.imaging.zoomX_slider.value()*10))
        try:
            self.deleteFigure()
            self.drawFigure()
        except:
            pass

    def zoomFigureY(self):
        self.extraPopup.imaging.zoomY_label.setText("Zoom Y Level ({} %)".format(self.extraPopup.imaging.zoomY_slider.value()*10))
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
            QMessageBox.about(self, "Warning", "Please select one histogram...")
                
    ############################
    # 16) Jupyter Notebook
    ############################                    

    def createDf(self):
        if (DEBUG):
            print("Create dataframe for Jupiter and web")
        data_to_list = []
        for index, row in self.spectrum_list.iterrows():
            tmp = row['data'].tolist()
            data_to_list.append(tmp)
            if (DEBUG):
                print("len(data_to_list) --> ", row['names'], " ", len(tmp))

        if (DEBUG):
            print([list((i, len(data_to_list[i]))) for i in range(len(data_to_list))])
        self.spectrum_list = self.spectrum_list.drop('data', 1)
        self.spectrum_list['data'] = np.array(data_to_list)

        self.spectrum_list.to_csv(self.extraPopup.peak.jup_df_filename.text())
    
    def jupyterStop(self):
        # stop the notebook process
        log("Sending interrupt signal to jupyter-notebook")
        self.extraPopup.peak.jup_start.setEnabled(True)
        self.extraPopup.peak.jup_stop.setEnabled(False)        
        self.extraPopup.peak.jup_start.setStyleSheet("background-color:#3CB371;")
        self.extraPopup.peak.jup_stop.setStyleSheet("")        
        stopnotebook()

    def jupyterStart(self):

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
        
        self.extraPopup.peak.jup_start.setEnabled(False)
        self.extraPopup.peak.jup_stop.setEnabled(True)        
        self.extraPopup.peak.jup_start.setStyleSheet("")        
        self.extraPopup.peak.jup_stop.setStyleSheet("background-color:#DC143C;")

    #######################
    # 17) Misc Tools
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
        rec = matplotlib.patches.Rectangle((0, 0),
                                           1,
                                           1, ls="-", lw="2", ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)        
        return rec

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
        else:
            self.wConf.listParams[1].setEnabled(True)
            
    def checkVersion(self, version):
        l = [int(x, 10) for x in version.split('.')]
        l.reverse()
        version = sum(x * (10 ** i) for i, x in enumerate(l))
        return version
    
        
# redirect logging 
class QtLogger(QObject):
    newlog = pyqtSignal(str)
    
    def __init__(self, parent):
        super(QtLogger, self).__init__(parent)

        
