#!/usr/bin/env python3
# import modules and packages
import importlib
import io, pickle, traceback, sys, os, subprocess
import signal, logging, ctypes, copy, json, httplib2, cv2
import threading, itertools, time, multiprocessing, math, re
from ctypes import *
from copy import copy, deepcopy
from itertools import chain, compress, zip_longest
import pandas as pd
import numpy as np

sys.path.append(os.getcwd())
sys.path.append("./Lib")
sys.path.append(str(os.environ.get("INSTDIR"))+"/Lib")

# removes the webproxy from spdaq machines
os.environ['NO_PROXY'] = ""

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
from PlotGUI import Tabs # area defined for the Tabs
from PyREST import PyREST # class interface for SpecTcl REST plugin
from CopyPropertiesGUI import CopyProperties

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

        self.setWindowFlag(Qt.WindowMinimizeButtonHint, True)
        self.setWindowFlag(Qt.WindowMaximizeButtonHint, True)
        
        self.factory = factory
        self.fit_factory = fit_factory        

        self.setWindowTitle("CutiePie(QtPy) - It's not a bug, it's a feature (cit.)")
        self.setMouseTracking(True)
        
        #check if there are arguments or not
        try:
            self.args = dict(args)
            if (DEBUG):
                print("self.args",self.args)
        except:
            pass
        
        #######################
        # 1) Main layout GUI
        #######################        

        mainLayout = QVBoxLayout()
        mainLayout.setContentsMargins(0,0,0,0)
        mainLayout.setSpacing(0)

        # top menu
        self.wTop = Menu()
        self.wTop.setFixedHeight(50)

        # config menu
        self.wConf = Configuration()
        self.wConf.setFixedHeight(70)
        
        # plot widget
        self.wTab = Tabs()
        self.currentPlot = None
        
        # gui composition
        mainLayout.addWidget(self.wTop)
        mainLayout.addWidget(self.wConf)
        mainLayout.addWidget(self.wTab)
        
        widget = QWidget()
        widget.setLayout(mainLayout)        
        self.setCentralWidget(widget)

        # output popup window
        self.resPopup = OutputPopup()
        self.table_row = []

        # extra popup window
        self.extraPopup = SpecialFunctions()

        # copy attributes windows
        self.copyAttr = CopyProperties()

        # initialize factory from algo_creator
        self.factory.initialize(self.extraPopup.imaging.clusterAlgo)
        # initialize factory from fit_creator
        self.fit_factory.initialize(self.extraPopup.fit_list)

        # global variables
        self.timer = QtCore.QTimer()
        self.originalPalette = QApplication.palette()
        # dictionaries for parameters
        self.param_list = {}
        self.nparams = 0
        # dataframe for spectra
        self.spectrum_list = pd.DataFrame()

        # max for y
        self.minY = 0
        self.maxY = 1024        
        # gradient for 2d plots
        self.minZ = 0
        self.maxZ = 256
        
        #################
        # 2) Signals
        #################

        # top menu signals
        self.wTop.updateButton.clicked.connect(self.update)
        self.wTop.saveButton.clicked.connect(self.saveGeo)
        self.wTop.loadButton.clicked.connect(self.loadGeo)
        self.wTop.exitButton.clicked.connect(self.closeAll)

        # new tab creation
        self.wTab.tabBarClicked.connect(self.clickedTab)
        
        # config menu signals
        self.wConf.histo_geo_add.clicked.connect(self.addPlot)
        self.wConf.histo_geo_update.clicked.connect(self.updatePlot)
        #self.wConf.histo_geo_delete.clicked.connect(self.clear_plot)
        
        self.wConf.histo_geo_row.activated.connect(lambda: self.wTab.wPlot[self.wTab.currentIndex()].InitializeCanvas(
            int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_col.activated.connect(lambda: self.wTab.wPlot[self.wTab.currentIndex()].InitializeCanvas(
            int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))

        # home callback
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[0].triggered.connect(self.homeCallback)
        # copy properties
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)
        # summing region
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.clicked.connect(self.createSRegion)
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.setEnabled(False)
        # autoscale
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.setAutoscaleAxis)
        # log button
        self.wTab.wPlot[self.wTab.currentIndex()].histo_log.clicked.connect(self.setLogAxis)
        # plus button
        #self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomIn(self.wTab[self.tabIndex].wPlot.canvas))
        # minus button
        #self.wTab[self.tabIndex].wPlot.minusButton.clicked.connect(lambda: self.zoomOut(self.wTab[self.tabIndex].wPlot.canvas))

        # key press event
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.setFocusPolicy( QtCore.Qt.ClickFocus )
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.setFocus()

        # other signals
        self.resizeID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("resize_event", self.on_resize)
        self.pressID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.on_press)

        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.histoHover)
        
        # create helpers
        self.wConf.histo_list.installEventFilter(self)
        for i in range(2):
                self.wConf.listParams[i].installEventFilter(self)
        self.wConf.listGate.installEventFilter(self)
        self.wConf.listGate.installEventFilter(self)
        
        self.currentPlot = self.wTab.wPlot[self.wTab.currentIndex()] # definition of current plot
        
    ################################
    # 3) Implementation of Signals
    ################################

    def bindDynamicSignal(self):
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[0].triggered.connect(self.homeCallback)        
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.setAutoscaleAxis)
        self.wTab.wPlot[self.wTab.currentIndex()].histo_log.clicked.connect(self.setLogAxis)
        
        self.resizeID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("resize_event", self.on_resize)
        self.pressID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.on_press)

        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.histoHover)
        
    def eventFilter(self, obj, event):
        if (obj == self.wConf.histo_list or self.wConf.listParams[0] or self.wConf.listParams[1] or self.wConf.listGate) and event.type() == QtCore.QEvent.HoverEnter:
            self.onHovered(obj)
        return super(MainWindow, self).eventFilter(obj, event)

    def onHovered(self, obj):
        if (obj == self.wConf.histo_list):
            self.wConf.histo_list.setToolTip(self.wConf.histo_list.currentText())
        elif (obj == self.wConf.listParams[0]):
            self.wConf.listParams[0].setToolTip(self.wConf.listParams[0].currentText())
        elif (obj == self.wConf.listParams[1]):
            self.wConf.listParams[1].setToolTip(self.wConf.listParams[1].currentText())
        elif (obj == self.wConf.listGate):
            self.wConf.listGate.setToolTip(self.wConf.listGate.currentText())

    def histoHover(self, event):
        try:
            index = 0
            if not event.inaxes: return
            if self.currentPlot.isZoomed:
                if (DEBUG):
                    print("Inside histoHover isZoomed")
                index = self.currentPlot.selected_plot_index_bak
            else:
                index = list(self.currentPlot.figure.axes).index(event.inaxes)
            self.currentPlot.histoLabel.setText("Histogram:"+self.currentPlot.h_dict_geo[index])
        except:
            pass

    def on_resize(self, event):
        self.currentPlot.figure.tight_layout()
        self.currentPlot.canvas.draw()

    def on_press(self, event):
        if not event.inaxes: return
        self.currentPlot.selected_plot_index = list(self.currentPlot.figure.axes).index(event.inaxes)
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
            print("Inside on_singleclick in tab",self.wTab.currentIndex())
        # If we are not zooming on one histogram we can select one histogram
        # and a red rectangle will contour the plot
        if self.currentPlot.isZoomed == False:
            if (DEBUG):
                print("Inside on_singleclick - ZOOM false")
            try:
                if self.currentPlot.rec is not None:
                    self.currentPlot.rec.remove()
            except:
                pass
            # by single clicking we select the plot with index selected_plot_index and draw a
            # red rectangle on the axes to show choice
            for i, plot in enumerate(self.currentPlot.figure.axes):
                if (i == self.currentPlot.selected_plot_index):
                        self.currentPlot.isSelected = True
                        self.currentPlot.rec = self.createRectangle(plot)
                        '''
                        if self.h_log[i]:
                            self.wTab[self.tabIndex].wPlot.histo_log.setChecked(True)
                        else:
                            self.wTab[self.tabIndex].wPlot.histo_log.setChecked(False)
                        '''
                        self.clickToIndex(self.currentPlot.selected_plot_index)
            self.currentPlot.canvas.draw()
        else:
            if (DEBUG):
                print("Inside on_singleclick - ZOOM true")            
            
        t=None
        
    def on_dblclick(self, event):
        global t
        if (DEBUG):
            print("Inside on_dblclick in tab", self.wTab.currentIndex())
        if self.currentPlot.h_dict_geo[0] == "empty":
            self.currentPlot.h_dict_geo = deepcopy(self.currentPlot.h_dict_geo_bak)
            
        name = self.currentPlot.h_dict_geo[self.currentPlot.selected_plot_index]
        index = self.wConf.histo_list.findText(name)
        self.wConf.histo_list.setCurrentIndex(index)
        self.updateHistoInfo()
        if (DEBUG):
            print("UPDATE plot the histogram at index", self.currentPlot.selected_plot_index, "with name", self.wConf.histo_list.currentText())
    
        if self.currentPlot.isZoomed == False: # entering zooming mode
            if (DEBUG):
                print("###### Entering zooming mode...")
            self.currentPlot.isZoomed = True
            # disabling adding histograms
            self.wConf.histo_geo_add.setEnabled(False)
            # enabling gate creation
            '''
            self.wTab[self.tabIndex].wPlot.createSRegion.setEnabled(True)
            self.wConf.createGate.setEnabled(True)
            self.wConf.editGate.setEnabled(True)
            if (DEBUG):
                print("inside dblclick: self.selected_plot_index", self.selected_plot_index)
            self.wTab[self.tabIndex].wPlot.figure.clear()
            self.wTab[self.tabIndex].wPlot.canvas.draw()
            '''
            # backing up list of histogram
            self.currentPlot.h_dict_bak = self.currentPlot.h_dict.copy()
            # plot corresponding histogram
            if (DEBUG):
                print("plot the histogram at index", self.currentPlot.selected_plot_index, "with name", (self.currentPlot.h_dict[self.currentPlot.selected_plot_index])["name"])
            self.currentPlot.selected_plot_index_bak = deepcopy(self.currentPlot.selected_plot_index)
            a = self.updatePlot()
            #self.removeCb(a)
            '''
            self.drawAllGates()
            '''
        else:
            # enabling adding histograms
            self.wConf.histo_geo_add.setEnabled(True)
            '''
            if self.toCreateGate == True or self.toCreateSRegion == True:
                if (DEBUG):
                    print("Fixing index before closing the gate")
            else:
            '''
            if (DEBUG):
                print("##### Exiting zooming mode...")
            self.currentPlot.isZoomed = False
            # disabling gate creation
            '''
            self.wTab[self.tabIndex].wPlot.createSRegion.setEnabled(False)
                self.wConf.createGate.setEnabled(False)
                self.wConf.editGate.setEnabled(False)
            '''
            if (DEBUG):
                print("Reinitialization self.h_setup", self.h_setup)
                print("original geometry", self.currentPlot.old_row, self.currentPlot.old_col)
            #draw the back the original canvas
            self.currentPlot.InitializeCanvas(self.currentPlot.old_row, self.currentPlot.old_col, False)
            if (DEBUG):
                print("Ready to reload the multipanel", self.currentPlot.h_dict)
            n = self.currentPlot.old_row*self.currentPlot.old_col
            self.currentPlot.h_setup = {k: True for k in range(n)}
            self.currentPlot.selected_plot_index = None # this will allow to call drawGate and loop over all the gates
            '''
            self.h_log = deepcopy(self.h_log_bak)
            '''
            self.updatePlot()

        t=None

    def get_key(self, val):
        for key, value in self.currentPlot.h_dict.items():
            for key2, value2, in value.items():
                if val == value2:
                    return key
        
    def homeCallback(self, event):
        #if (DEBUG):
        print("Clicked homecallback in tab", self.wTab.currentIndex())
        name = str(self.wConf.histo_list.currentText())
        index = self.get_key(name)
        self.resetAxisLimits(index)
        self.currentPlot.canvas.draw()
        
        '''
        try:
            if self.currentPlot.isZoomed:
                #if (DEBUG):
                print("homeCallback - zoom mode")

                name = str(self.wConf.histo_list.currentText())
                self.currentPlot.selected_plot_index = self.get_key(name)
                #if (DEBUG):
                print("zoom mode for histogram ", name, " with index " , self.currentPlot.selected_plot_index)
                print("cleaning and setting up the zoomed pic...")

                self.wTab[self.tabIndex].wPlot.figure.clear()
                self.wTab[self.tabIndex].wPlot.canvas.draw()
                if (DEBUG):
                    print("plot the histogram at index", self.selected_plot_index, "with name", (self.h_dict[self.selected_plot_index])["name"])
                a = self.update_plot()
                self.reset_axis_properties(self.selected_plot_index)
                self.drawAllGates()
                self.wTab[self.tabIndex].wPlot.canvas.draw()
            else:
                #if (DEBUG):                
                print("homeCallback - multipanel mode")                
                #if (DEBUG):
                print("plot the histogram at index", self.selected_plot_index, "with name", (self.h_dict[self.selected_plot_index])["name"])
                self.reset_axis_properties(self.selected_plot_index)
                self.wTab[self.tabIndex].wPlot.canvas.draw()
        except:
            pass
        '''
        
    def closeAll(self):
        self.close()

    def clickedTab(self, index):
        if (DEBUG):
            print("Clicked tab", index, "with name", self.wTab.tabText(index))
        self.wTab.setCurrentIndex(index)
        if (DEBUG):
            print("verification tab index", self.wTab.currentIndex())
        try:
            if self.wTab.tabText(index) != "+":
                self.currentPlot = self.wTab.wPlot[self.wTab.currentIndex()]
                if (DEBUG):
                    print("self.currentPlot.h_dict", self.currentPlot.h_dict)
                    print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
                    print("self.currentPlot.h_limits",self.currentPlot.h_limits)
                    print("self.currentPlot.h_log",self.currentPlot.h_log)
                    print("self.currentPlot.h_setup",self.currentPlot.h_setup)
                    print("self.currentPlot.isLoaded", self.currentPlot.isLoaded)
                    print("Histo dimensions", self.currentPlot.old_row, self.currentPlot.old_col)
                    print("Histo dimensions index", self.currentPlot.old_row_idx, self.currentPlot.old_col_idx)            
                    print("Inside clickedTab: row.currentText(), col.currentText()",
                          int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText()))                
                self.wConf.histo_geo_row.setCurrentIndex(self.currentPlot.old_row_idx)
                self.wConf.histo_geo_col.setCurrentIndex(self.currentPlot.old_col_idx)                
            self.bindDynamicSignal()
        except:
            pass
        
        '''
        for ww in self.wTab.children():
            print(">>",ww.metaObject().className())
            if ("QStackedWidget" == ww.metaObject().className()):
                print("looping over QStackedWidget")
                x = ww.count()
                print("number of widgets", x)
                for i in range(ww.count()):       
                    w = ww.widget(i) 
                    print(w.metaObject().className(),type(w))
                    #if w.metaObject().className() == "Plot":
                    #    print("Found plot")
                    #    self.wPlot = w
        ''' 

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
            if (DEBUG):
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

            if (DEBUG):
                print(self.spectrum_list)
            
            # update and create parameter, spectrum, and gate lists
            self.create_spectrum_list()
            self.create_parameter_list()
            self.update_spectrum_info()
            '''
            self.create_gate_list()
            self.updateGateType()
            '''
            
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started or hostname/port/mirror are not configured!")
            
    # extract correct spectrum name
    def getSpectrumNames(self):
        if (DEBUG):
            print("Inside getSpectrumNames")        
        nlst = []
        lstdict = self.rest.listSpectrum()
        for el in lstdict:
            nlst.append(el["name"])

        return nlst
    
    # update spectrum list for GUI
    def update_spectrum_parameters(self):
        if (DEBUG):
            print("Inside update_spectrum_parameters")
        try:
            spec_dict = self.rest.listSpectrum()
            if (DEBUG):
                print(spec_dict)
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

    # create spectrum list for GUI
    def create_spectrum_list(self):
        if (DEBUG):
            print("Inside create_spectrum_list")
        self.update_spectrum_parameters()
        for name in self.spectrum_list['names']:
            if self.wConf.histo_list.findText(name) == -1:
                self.wConf.histo_list.addItem(name)

    # update parameter list for GUI
    def update_parameter_list(self):
        if (DEBUG):
            print("Inside update_parameter_list")
        try:
            par_dict = self.rest.listParameter()
            if (DEBUG):
                print(par_dict)
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
                
    # create parameter list for GUI
    def create_parameter_list(self):
        if (DEBUG):
            print("Inside create_parameter_list")
        self.update_parameter_list()
        if (DEBUG):
            print(self.param_list)
        for key, value in self.param_list.items():
            for i in range(2):
                if self.wConf.listParams[i].findText(value) == -1:
                    self.wConf.listParams[i].addItem(value)

    # update spectrum information
    def update_spectrum_info(self):
        if (DEBUG):
            print("Inside update_spectrum_info")
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
            QMessageBox.about(self, "Warning", "update_spectrum_info - Please click 'Get Data' to access the data...")

    # aux function for histo size
    def create_disable2D(self, value):
        if value==True:
            self.wConf.listParams[1].setEnabled(False)
        else:
            self.wConf.listParams[1].setEnabled(True)
            
    # check histogram dimension from GUI
    def check_histogram(self):
        if self.wConf.button1D.isChecked():
            self.create_disable2D(True)
        else:
            self.create_disable2D(False)

    # update spectrum information
    def updateHistoInfo(self):
        if (DEBUG):
            print("Inside updateHistoInfo")
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
            if (DEBUG):
                print(hist_name, hist_dim, df.iloc[0]['parameters'][0], df.iloc[0]['parameters'][1])
            for i in range(hist_dim):
                index = self.wConf.listParams[i].findText(df.iloc[0]['parameters'][i], QtCore.Qt.MatchFixedString)
                if index >= 0:
                    self.wConf.listParams[i].setCurrentIndex(index)
        except:
            pass

    ##########################################
    # 7) Load/save geometry window
    ##########################################

    def findWholeWord(self, w):
        return re.compile(r'\b({0})\b'.format(w), flags=re.IGNORECASE).search

    def findNumbers(self, w):
        return [int(s) for s in re.findall(r'\b\d+\b', w)]

    def findHistoName(self, w):
        return re.findall('"([^"]*)"', w)

    # definition for both legacy and not window defs
    def openGeo(self, filename):
        cntr = 0
        coords = []
        spec_dict = {}
        info_scale = {}
        info_range = {}

        x_range = {}
        y_range = {}
        ax_scale = {}
        properties = {}
        if (DEBUG):
            print("Inside openGeo")
        if (len(open(filename).readlines()) == 1):
            return eval(open(filename,"r").read())
        else:
            with open(filename) as f:
                for line in f:
                    if (self.findWholeWord("Geometry")(line)):
                        # find geo x,y in line
                        coords = self.findNumbers(line)
                    elif (self.findWholeWord("Window")(line)):
                        spectrum = self.findHistoName(line)
                        spec_dict[cntr] = spectrum[0]
                        if (DEBUG):
                            print(cntr, spec_dict[cntr])
                        cntr+=1
                    elif (self.findWholeWord("COUNTSAXIS")(line)):
                        if (DEBUG):
                            print(cntr-1, True)
                        info_scale[cntr-1] = True
                    elif (self.findWholeWord("Expanded")(line)):
                        tmp = self.findNumbers(line)
                        if (DEBUG):
                            print(cntr-1, tmp)
                        info_range[cntr-1] = tmp

            for index, value in spec_dict.items():
                h_name = value
                select = self.spectrum_list['names'] == h_name
                df = self.spectrum_list.loc[select]
                scale = False
                x_range = [int(df.iloc[0]['minx']), int(df.iloc[0]['maxx'])]
                y_range = [self.minY, self.maxY]
                if df.iloc[0]['dim'] == 2:
                    y_range = [int(df.iloc[0]['miny']), int(df.iloc[0]['maxy'])]

                if index in info_scale:
                    scale = info_scale[index]
                    if y_range[0] == 0:
                        y_range[0] = 0.001
                if index in info_range:
                    x_range = info_range[index][0:2]
                    y_range = info_range[index][2:4]
                if (DEBUG):
                    print("after correction", x_range, y_range)

                properties[index] = {"name": h_name, "x": x_range, "y": y_range, "scale": scale}
                
            return {'row': coords[0], 'col': coords[1], 'geo': properties}

    def saveGeo(self):
        fileName = self.saveFileDialog()
        try:
            f = open(fileName,"w")
            properties = {}
            for index in range(len(self.currentPlot.h_dict_geo)):
                h_name = self.currentPlot.h_dict_geo[index]
                x_range, y_range = self.getAxisProperties(index)
                scale = False
                #scale = self.h_log[index]
                properties[index] = {"name": h_name, "x": x_range, "y": y_range, "scale": scale}

            tmp = {"row": self.wConf.row, "col": self.wConf.col, "geo": properties}
            QMessageBox.about(self, "Saving...", "Window configuration saved!")
            f.write(str(tmp))
            f.close()
        except TypeError:
            pass
        
    def loadGeo(self):
        fileName = self.openFileNameDialog()
        if (DEBUG):
            print("Inside loadGeo")
        try:
            if (DEBUG):
                print("fileName:",fileName)
                print("openGeo output", self.openGeo(fileName))
            infoGeo = self.openGeo(fileName)
            self.wConf.row = infoGeo["row"]
            self.wConf.col = infoGeo["col"]
            # change index in combobox to the actual loaded values
            index_row = self.wConf.histo_geo_row.findText(str(self.wConf.row), QtCore.Qt.MatchFixedString)
            index_col = self.wConf.histo_geo_col.findText(str(self.wConf.col), QtCore.Qt.MatchFixedString)
            if index_row >= 0 and index_col >= 0:
                self.wConf.histo_geo_row.setCurrentIndex(index_row)
                self.wConf.histo_geo_col.setCurrentIndex(index_col)
                self.currentPlot.InitializeCanvas(infoGeo["row"],infoGeo["col"])
                for index, val_dict in infoGeo["geo"].items():
                    if (DEBUG):
                        print("---->",index, val_dict)
                        print(self.currentPlot.h_dict)
                    
                    self.currentPlot.h_dict_geo[index] = val_dict["name"]
                    self.currentPlot.h_log[index] = val_dict["scale"]
                    self.currentPlot.h_limits[index] = {}
                    self.currentPlot.h_limits[index]["x"] = val_dict["x"]
                    self.currentPlot.h_limits[index]["y"] = val_dict["y"]
                self.currentPlot.isLoaded = True
                
                if len(self.currentPlot.h_dict_geo) == 0:
                    QMessageBox.about(self, "Warning", "You saved an empty pane geometry...")

            if (DEBUG):
                print("After loading geo win")
                print("self.currentPlot.h_dict", self.currentPlot.h_dict)
                print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)                
                print("self.currentPlot.h_limits",self.currentPlot.h_limits)
                print("self.currentPlot.h_log",self.currentPlot.h_log)
                print("self.currentPlot.h_setup",self.currentPlot.h_setup)
                print("self.currentPlot.isLoaded", self.currentPlot.isLoaded)                    

            self.currentPlot.h_dict_geo_bak = deepcopy(self.currentPlot.h_dict_geo)
            self.currentPlot.h_log_bak = deepcopy(self.currentPlot.h_log)

            self.currentPlot.old_row = self.wConf.row
            self.currentPlot.old_col = self.wConf.col
            self.currentPlot.old_row_idx = index_row
            self.currentPlot.old_col_idx = index_col            

            if (DEBUG):            
                print("self.currentPlot.old_row",self.currentPlot.old_row)
                print("self.currentPlot.old_col",self.currentPlot.old_col)
                print("self.currentPlot.old_row_idx",self.currentPlot.old_row_idx)
                print("self.currentPlot.old_col_idx",self.currentPlot.old_col_idx)                        
            
            self.addPlot()
            self.updatePlot()
            self.currentPlot.isLoaded = False
            '''
            try:
                self.drawAllGates()
                self.updateGateType()
            except:
                pass
            '''
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

    # modify axes
    def axisScale(self, ax, index):
        if (DEBUG):
            print("Inside axisScale")
        if self.currentPlot.logScale:
            if (DEBUG):
                print("needs to become log...")
            if (self.currentPlot.h_dict[index]["dim"] == 1) :
                if ax.get_yscale() == "linear":
                    ymin, ymax = ax.get_ylim()
                    if ymin == 0:
                        ymin = 0.001                        
                    ax.set_ylim(ymin,ymax)
                    ax.set_yscale("log")
            else:
                zmin = 0
                if self.minZ == 0:
                    zmin = 0.001
                else:
                    zmin = self.minZ
                zmin = math.log10(zmin)
                zmax = math.log10(self.maxZ)
                self.currentPlot.h_lst[index].set_clim(vmin=zmin, vmax=zmax)
        else:
            if (DEBUG):
                print("needs to become linear...")
            if (self.currentPlot.h_dict[index]["dim"] == 1) :
                if ax.get_yscale() == "log":
                    if (self.currentPlot.h_limits[index]):
                        ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])
                    else:
                        ax.set_ylim(self.minY,self.maxY)
                    ax.set_yscale("linear")
            else:
                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)
    
    # setting log/linear axes
    def setLogAxis(self):
        if (DEBUG):
            print("Clicked setLogAxis in tab", self.wTab.currentIndex())
            print("Log scale is:", self.currentPlot.logScale, "for histogram @ index", self.currentPlot.selected_plot_index)
        if self.currentPlot.logScale:
            self.currentPlot.h_log[self.currentPlot.selected_plot_index] = True
        else:
            self.currentPlot.h_log[self.currentPlot.selected_plot_index] = False            

        try:
            ax = None
            if self.currentPlot.isZoomed:
                ax = plt.gca()
            else:
                ax = self.select_plot(self.currentPlot.selected_plot_index)

            if (DEBUG):                
                print("self.currentPlot.h_log", self.currentPlot.h_log)
                print("self.currentPlot.h_log_bak", self.currentPlot.h_log_bak)            
            
            # loop over the two log lists, and modify the axis of the different one
            for idx, (first, second) in enumerate(zip(list(self.currentPlot.h_log.values()),list(self.currentPlot.h_log_bak.values()))):
                if first != second:
                    if (DEBUG):
                        print("----> histogram in index", idx, "time to change axis")
                    self.axisScale(ax, idx)
                    if (DEBUG):
                        print("before backing", self.currentPlot.h_log, "\n", self.currentPlot.h_log_bak)
                    self.currentPlot.h_log_bak = deepcopy(self.currentPlot.h_log)
                    if (DEBUG):
                        print("after backing", self.currentPlot.h_log, "\n", self.currentPlot.h_log_bak)
                #else:
                #    print("there are no differences..")

            self.currentPlot.canvas.draw()
        except NameError:
            raise
        
    # setting autoscale+axis properties with h_limits
    def setAutoscaleAxis(self):
        if (DEBUG):
            print("Clicked tab", self.wTab.currentIndex(), "with name", self.wTab.tabText(self.wTab.currentIndex()))
            print("Autoscale is:", self.currentPlot.autoScale)

        try:
            ax = None
            if self.currentPlot.isZoomed:
                if (DEBUG):
                    print("inside isZoomed")
                ax = plt.gca()
                if self.currentPlot.autoScale:
                    if (DEBUG):
                        print("Inside self.autoScale for tab with index", self.wTab.currentIndex())
                    if (self.currentPlot.h_dict[index]["name"]) != "empty":
                        data = self.get_data(self.currentPlot.selected_plot_index)
                        if (self.currentPlot.h_dict[index]["dim"] == 1) :
                            ymax_new = max(data)*1.1
                            ax.set_ylim((ax.get_ylim())[0], ymax_new)
                        else:
                            maxZ = np.max(data)*1.1
                            self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=maxZ)
                else:
                    if (DEBUG):
                        print("Inside not self.autoScale for tab with index", self.wTab.currentIndex())
                    # if h_limits exist
                    if (self.currentPlot.h_limits[self.currentPlot.selected_plot_index]):
                        ax.set_xlim(self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["x"][0], self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["x"][1])
                        ax.set_ylim(self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["y"][0], self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["y"][1])
                    else:
                        ax.set_xlim(float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmin"][0]), float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmin"][1]))
                        ax.set_ylim(float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["ymin"][0]), float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["ymin"][1]))
                        self.currentPlot.h_lst[self.currentPlot.selected_plot_index].set_clim(vmin=self.minZ, vmax=self.maxZ)
            else:
                if (DEBUG):
                    print("Inside multipanel option")
                    print("self.currentPlot.autoScale", self.currentPlot.autoScale)
                if self.currentPlot.autoScale:
                    if (DEBUG):
                        print("Inside self.autoScale for tab with index", self.wTab.currentIndex())

                    for index, values in self.currentPlot.h_dict.items():
                        if (self.currentPlot.h_dict[index]["name"]) != "empty":                        
                            data = self.get_data(index)
                            ax = self.select_plot(index)
                            if self.currentPlot.h_dict[index]["dim"] == 1:
                                ymax_new = max(data)*1.1
                                ax.set_ylim((ax.get_ylim())[0], ymax_new)
                            else:
                                maxZ = np.max(data)*1.1
                                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=maxZ)
                else:
                    if (DEBUG):
                        print("Inside not self.autoScale for tab with index", self.wTab.currentIndex())
                    for index, values in self.currentPlot.h_dict.items():
                        if (self.currentPlot.h_dict[index]["name"]) != "empty":                        
                            ax = self.select_plot(index)                        
                            if (DEBUG):
                                print(index, values)
                            if self.currentPlot.h_dict[index]["dim"] == 1:
                                if (DEBUG):
                                    print("inside 1d")
                                    print(self.currentPlot.h_limits[index])
                                # if h_limits exist
                                if (self.currentPlot.h_limits[index]):
                                    ax.set_xlim(self.currentPlot.h_limits[index]["x"][0], self.currentPlot.h_limits[index]["x"][1])
                                    ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])
                                else:
                                    if (DEBUG):                                    
                                        print("empty limits")
                                        print(self.currentPlot.h_dict[index]["xmin"], self.currentPlot.h_dict[index]["xmax"])
                                    ax.set_xlim(float(self.currentPlot.h_dict[index]["xmin"]), float(self.currentPlot.h_dict[index]["xmax"]))
                                    ax.set_ylim(self.minY, self.maxY)                                
                            else:
                                if (DEBUG):                                
                                    print("inside 2d")                                
                                # if h_limits exist
                                if (self.currentPlot.h_limits[index]):
                                    ax.set_xlim(self.currentPlot.h_limits[index]["x"][0], self.currentPlot.h_limits[index]["x"][1])
                                    ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])
                                else:                                            
                                    ax.set_xlim(float(self.currentPlot.h_dict[index]["xmin"]), float(self.currentPlot.h_dict[index]["xmax"]))
                                    ax.set_ylim(float(self.currentPlot.h_dict[index]["ymin"]), float(self.currentPlot.h_dict[index]["ymax"]))
                                    self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)                        

            self.currentPlot.canvas.draw()

        except:
            pass

    def getAxisProperties(self, index):
        if (DEBUG):
            print("Inside getAxisProperties")
        try:
            ax = None
            if self.currentPlot.isZoomed:
                ax = plt.gca()
            else:
                ax = self.select_plot(index)

            if (DEBUG):
                print(type(ax.get_xlim()))
                print(ax.get_xlim(), ax.get_xaxis().get_scale())
                print(ax.get_ylim(), ax.get_yaxis().get_scale())

            return list(ax.get_xlim()), list(ax.get_ylim())
        except:
            pass

    def resetAxisLimits(self, index):
        #if (DEBUG):
        print("Inside resetAxisLimits", self.currentPlot.h_limits[index])
        print("original axes", self.currentPlot.h_dict[index])
        print("original limits", self.currentPlot.h_limits[index])
        ax = None
        if self.currentPlot.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(index)

        if self.currentPlot.h_dim[index] == 1:            
            ax.set_xlim(float(self.currentPlot.h_dict[index]["xmin"]), float(self.currentPlot.h_dict[index]["xmax"]))
            ax.set_ylim(self.minY, self.maxY)
        else:
            ax.set_xlim(float(self.currentPlot.h_dict[index]["xmin"]), float(self.currentPlot.h_dict[index]["xmax"]))
            ax.set_ylim(float(self.currentPlot.h_dict[index]["ymin"]), float(self.currentPlot.h_dict[index]["ymax"]))
            self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)

        # reset limits
        self.currentPlot.h_limits[index]["x"] = []
        self.currentPlot.h_limits[index]["y"] = []
        
        self.currentPlot.canvas.draw()
        
        '''
        orig_histo = self.update_spectrum_info()
        if (DEBUG):
            print(orig_histo)

        if self.h_limits[index]:
            try:
                ax = None
                if self.isZoomed:
                    ax = plt.gca()
                else:
                    ax = self.select_plot(index)

                if (DEBUG):
                    print(self.h_limits)
                    print(ax.get_xlim(), ax.get_ylim())
                    print(orig_histo["xmin"],orig_histo["xmax"])
                    print(orig_histo["ymin"],orig_histo["ymax"])
                self.h_limits[index]["x"] = [float(orig_histo["xmin"]),float(orig_histo["xmax"])]
                self.h_limits[index]["y"] = [float(orig_histo["ymin"]),float(orig_histo["ymax"])]
                if (DEBUG):
                    print(self.h_limits)
                ax.set_xlim(self.h_limits[index]["x"][0], self.h_limits[index]["x"][1])
                if self.h_dim[index] == 2:
                    ax.set_ylim(self.h_limits[index]["y"][0], self.h_limits[index]["y"][1])
            except:
                QMessageBox.about(self, "Warning", "Something has failed in reset_axis_properties")
        '''
        
    ##################################
    ## 9) Histogram operations
    ##################################

    # remove colorbar
    def removeCb(self, axis):
        im = axis.images
        if im is not None:
            try:
                cb = im[-1].colorbar
                cb.remove()
            except IndexError:
                pass

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
            self.currentPlot.index = keys[values.index("empty")]
        else:
            if (DEBUG):
                print("list is full, set index to full")
            self.currentPlot.index = keys[-1]
            self.currentPlot.isFull = True

        if self.currentPlot.isFull == True:
            if self.currentPlot.index == self.wConf.row*self.wConf.col-1:
                self.currentPlot.index = 0
            else:
                self.currentPlot.index += 1

        if (DEBUG):
            print("index to fill", self.currentPlot.index)
        return self.currentPlot.index
            
    # select axes based on indexing
    def select_plot(self, index):
        for i, plot in enumerate(self.currentPlot.figure.axes):
            # retrieve the subplot from the click
            if (i == index):
                return plot

    # returns position in grid based on indexing
    def plot_position(self, index):
        cntr = 0
        # convert index to position in geometry
        for i in range(self.currentPlot.old_row):
            for j in range(self.currentPlot.old_col):
                if index == cntr:
                    return i, j
                else:
                    cntr += 1
            
    # erase plot
    def erasePlot(self, index):
        if (DEBUG):
            print("Inside erasePlot")
        a = None
        if self.currentPlot.isZoomed:
            a = plt.gca()
        else:
            a = self.select_plot(index)
        # if 2d histo I need a bit more efforts for the colorbar
        try:
            self.removeCb(a)
        except:
            pass
        
        a.clear()
        return a

    # setup histogram
    def setupPlot(self, axis, index):
        if (DEBUG):
            print("Inside setupPlot")
            print("histoname -->", self.currentPlot.h_dict[index]["name"])
        if (DEBUG):
            print("self.currentPlot.h_dict", self.currentPlot.h_dict)
            print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
            print("self.currentPlot.h_limits",self.currentPlot.h_limits)
            print("self.currentPlot.h_log",self.currentPlot.h_log)
            print("self.currentPlot.h_setup",self.currentPlot.h_setup)
            print("self.currentPlot.h_dim",self.currentPlot.h_dim)        
            print("----------------------")

        if self.currentPlot.h_dict_geo[index] != "empty":
            if (DEBUG):
                print("not empty --> ",self.currentPlot.h_dict_geo)

            dim = int(self.currentPlot.h_dim[index])
            minx = float(self.currentPlot.h_dict[index]["xmin"])
            maxx = float(self.currentPlot.h_dict[index]["xmax"])
            binx = int(self.currentPlot.h_dict[index]["xbin"])

            if (DEBUG):            
                print("Histo", self.currentPlot.h_dict_geo[index], "dim", dim, minx, maxx, binx)

            # update axis
            if dim == 1:
                if (DEBUG):                
                    print("1d case...")
                axis.set_xlim(minx,maxx)
                axis.set_ylim(self.minY,self.maxY)
                # create histogram
                line, = axis.plot([], [], drawstyle='steps')
                self.currentPlot.h_lst.insert(index, line)
            else:
                if (DEBUG):                
                    print("2d case...")

                miny = self.currentPlot.h_dict[index]["ymin"]
                maxy = self.currentPlot.h_dict[index]["ymax"]
                biny = self.currentPlot.h_dict[index]["ybin"]
                    
                # empty data for initialization
                w = 0*np.random.random_sample((int(binx),int(biny)))

                # setup up palette
                if (self.wConf.button2D_option.currentText() == 'Dark'):
                        self.palette = 'plasma'
                else:
                        self.palette = copy(plt.cm.plasma)
                w = np.ma.masked_where(w < 0.1, w)
                self.palette.set_bad(color='white')

                # create histogram
                self.currentPlot.h_lst.insert(index, axis.imshow(w,
                                                                 interpolation='none',
                                                                 extent=[float(minx),float(maxx),float(miny),float(maxy)],
                                                                 aspect='auto',
                                                                 origin='lower',
                                                                 vmin=self.minZ, vmax=self.maxZ,
                                                                 cmap=self.palette))

            self.currentPlot.axbkg[index] = self.currentPlot.figure.canvas.copy_from_bbox(axis.bbox)
            if (DEBUG):
                print("self.currentPlot.h_lst",self.currentPlot.h_lst)

        if (DEBUG):                
            print("done setting up the histos")
            
    # geometrically add plots to the right place and calls plotting
    def add(self, index):
        if (DEBUG):
            print("Ready to add and initialize histograms...")
        self.currentPlot.h_setup[index] = True # ready to be setup
        a = None
        if (DEBUG):
            print(self.currentPlot.h_setup)
        if self.currentPlot.isZoomed:
            a = plt.gca()
            self.erasePlot(index)
        else:
            a = self.select_plot(index)
            x,y = self.plot_position(index)
            if (DEBUG):
                print("Plot", self.currentPlot.h_dict[index],"with index", index, "is in position",x,y)
            self.erasePlot(index)
        if (DEBUG):
            print("Before setting up plots...")
        self.setupPlot(a, index)
        
    def updateSinglePlot(self, index):
        if (DEBUG):
            print("Inside updateSinglePlot")
            print("self.currentPlot.h_dict[index]", self.currentPlot.h_dict[index])        
            print("self.currentPlot.h_setup", self.currentPlot.h_setup)

        if (self.currentPlot.h_dict[index]["name"] != "empty"):
            if (DEBUG):
                print("histoname", self.currentPlot.h_dict[index]["name"])
            a = self.select_plot(index)
            if self.currentPlot.isLoaded:
                if (DEBUG):
                    print("loaded")
                time.sleep(0.01)
                self.plotPlot(a, index)
            else:
                if (DEBUG):                
                    print("not loaded")
                self.setupPlot(a, index)
                self.add(index)
                self.plotPlot(a, index)

            if (self.currentPlot.h_setup[index]):
                self.currentPlot.h_setup[index] = False

        self.currentPlot.figure.tight_layout()
        self.currentPlot.canvas.draw_idle()
        self.setAutoscaleAxis()
            
    # geometrically add plots to the right place
    def addPlot(self):
        if (DEBUG):
            print("Inside addPlot")
        try:
            # if we load the geometry from file
            if self.currentPlot.isLoaded:
                if (DEBUG):
                    print("Inside addPlot - loaded")
                    print(self.currentPlot.h_dict_geo)
                    print(self.currentPlot.h_dict)                
                counter = 0
                for key, value in self.currentPlot.h_dict_geo.items():
                    if (DEBUG):
                        print("counter -->", counter)
                    index = self.wConf.histo_list.findText(value, QtCore.Qt.MatchFixedString)
                    # changing the index to the correct histogram to load
                    self.wConf.histo_list.setCurrentIndex(index)
                    self.updateHistoInfo()
                    if (DEBUG):
                        print(key, value, index)
                    # updating histogram dictionary with the last info needed (dim, xbinx, ybin, parameters, and type)
                    if (index != -1) :
                        self.currentPlot.h_dict[counter] = self.update_spectrum_info()

                    if (DEBUG):
                        print(self.update_spectrum_info())
                    counter += 1
                if (DEBUG):
                    print("updated self.currentPlot.h_dict")
                    print(self.currentPlot.h_dict)

                # updating support list for histogram dimension                
                if len(self.currentPlot.h_dict) != 0:
                    self.currentPlot.h_dim = self.currentPlot.get_histo_key_list(self.currentPlot.h_dict, "dim")
                    if (DEBUG):
                        print("self.currentPlot.h_dim",self.currentPlot.h_dim)

                for key, value in self.currentPlot.h_dict_geo.items():
                    if (DEBUG):
                        print(key, value)
                    self.add(key)
            else:
                if (DEBUG):
                    print("Inside addPlot - not loaded")
                # self adding
                if self.currentPlot.isSelected == False:
                    if (DEBUG):
                        print("Inside plot - self adding")
                    self.currentPlot.index = self.check_index()
                # position selected by user
                else:
                    if (DEBUG):
                        print("Inside plot - user adding")
                    self.currentPlot.index = self.currentPlot.selected_plot_index

                if (DEBUG):
                    print("Adding plot at index ", self.currentPlot.index)
                
                    print("self.currentPlot.h_dict", self.currentPlot.h_dict)
                    print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
                    print("self.currentPlot.h_dim", self.currentPlot.h_dim)                                
                
                # updating histogram dictionary for fast access to information via get_histo_xxx
                self.currentPlot.h_dict[self.currentPlot.index] = self.update_spectrum_info()
                self.currentPlot.h_dict_geo[self.currentPlot.index] = (self.currentPlot.h_dict[self.currentPlot.index])["name"]
                self.currentPlot.h_dim[self.currentPlot.index] = (self.currentPlot.h_dict[self.currentPlot.index])["dim"]
                self.currentPlot.h_limits[self.currentPlot.index] = {}
                
                if (DEBUG):
                    print("self.currentPlot.h_dict", self.currentPlot.h_dict)
                    print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
                    print("self.currentPlot.h_dim", self.currentPlot.h_dim)                                
                    
                '''
                self.h_dict_bak = self.h_dict.copy()
                self.h_dict_geo_bak = deepcopy(self.h_dict_geo)

                if self.logScale:
                    self.h_log[self.idx] = True
                else:
                    self.h_log[self.idx] = False

                self.h_log_bak = deepcopy(self.h_log)
                '''
                self.currentPlot.h_setup[self.currentPlot.index] = True
                self.erasePlot(self.currentPlot.index)
                #self.add(self.currentPlot.index)
                self.updateSinglePlot(self.currentPlot.index)
                #self.currentPlot.canvas.draw()
                self.currentPlot.isSelected = False

        except NameError:
            raise
            #QMessageBox.about(self, "Warning", "addPlot - Please click 'Get Data to access the data...")

    # getting data for plotting
    def get_data(self, index):
        if (DEBUG):
            print("Inside get_data")
        name = self.currentPlot.h_dict[index]["name"]
        dim = self.currentPlot.h_dict[index]["dim"]
        empty = 0
        w = []
        if name == "":
            return
        else:
            select = self.spectrum_list['names'] == name
            df = self.spectrum_list.loc[select]
            w = df.iloc[0]['data']
            
        if (DEBUG):
            print("dim:", dim)
            print("data for ", name)
            print(type(w))
            print(w)
            print("sum ", sum(w), "len", len(w))

        if dim == 1:
            empty = sum(w)
        else:
            if (DEBUG):
                print(len(w[0]))
            empty = len(w[0])

        if (empty == 0):
            self.isEmpty = True
        else:
            self.isEmpty = False
            return w

    def create_range(self, bins, vmin, vmax):
        x = []
        step = (float(vmax)-float(vmin))/float(bins)
        for i in np.arange(float(vmin), float(vmax), step):
            x.append(i)
        return x
        
    # histo plotting
    def plotPlot(self, axis, index, threshold=0.1):
        if (DEBUG):
            print("Inside plotPlot")        

        dim = int(self.currentPlot.h_dim[index])
        minx = float(self.currentPlot.h_dict[index]["xmin"])
        maxx = float(self.currentPlot.h_dict[index]["xmax"])
        binx = int(self.currentPlot.h_dict[index]["xbin"])
        if (DEBUG):
            print(dim, minx, maxx, binx)

        if (DEBUG):            
            print("self.currentPlot.h_dict", self.currentPlot.h_dict)
            print("self.currentPlot.h_lst", self.currentPlot.h_lst)
        
        w = self.get_data(index)
        if (DEBUG):
            print("data",sum(w))
            print(self.currentPlot.h_lst[index])

        if dim == 1:
            if (DEBUG):            
                print("1d case..")
            X = np.array(self.create_range(binx, minx, maxx))
            if (DEBUG):
                print(len(w),len(X))
                print(type(w),type(X))
            self.currentPlot.h_lst[index].set_data(X, w)
        else:
            if (DEBUG):
                print("2d case..")            
            if (self.wConf.button2D_option.currentText() == 'Light'):
                self.palette = copy(plt.cm.plasma)
                w = np.ma.masked_where(w < threshold, w)
                self.palette.set_bad(color='white')
                self.currentPlot.h_lst[index].set_cmap(self.palette)


            self.currentPlot.h_lst[index].set_data(w)

        self.currentPlot.figure.canvas.restore_region(self.currentPlot.axbkg[index])
        axis.draw_artist(self.currentPlot.h_lst[index])
        self.currentPlot.figure.canvas.blit(axis.bbox)

        # setup colorbar only for 2D
        if dim == 2:
            divider = make_axes_locatable(axis)
            cax = divider.append_axes('right', size='5%', pad=0.05)
            # add colorbar
            self.currentPlot.figure.colorbar(self.currentPlot.h_lst[index], cax=cax, orientation='vertical')

    def updatePlot(self):
        if (DEBUG):
            print("Inside updatePlot")
            print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
            print("self.currentPlot.h_setup", self.currentPlot.h_setup)
        try:
            a = None
            if self.currentPlot.isZoomed == True:
                if (DEBUG):
                    print("Inside updatePlot - zoomed")
                self.currentPlot.InitializeCanvas(1,1,False)
                a= plt.gca()
                if (DEBUG):
                    print("self.currentPlot.h_setup", self.currentPlot.h_setup)
                if int(self.currentPlot.h_dim[self.currentPlot.selected_plot_index]) == 1:
                    self.add(self.currentPlot.selected_plot_index)
                else:
                    self.setupPlot(a, self.currentPlot.selected_plot_index)

                self.currentPlot.h_setup[self.currentPlot.selected_plot_index] = False
                self.plotPlot(a, self.currentPlot.selected_plot_index)
                # This needs to be removed for gating - may we can fix it
                try:
                    self.removeCb(a)
                except:
                    pass
                '''
                if self.wConf.button1D.isChecked():
                    self.axisScale(a, self.selected_plot_index)
                else:
                    self.set_log_axis(self.selected_plot_index)
                '''
            else:
                if (DEBUG):
                    print("Inside updatePlot - multipanel mode")
                for index, value in self.currentPlot.h_dict.items():
                    if (DEBUG):
                        print("index", index, "value", value)
                    self.updateSinglePlot(index)
                    '''
                    if (value["name"] != "empty"):
                        a = self.select_plot(index)
                        if self.currentPlot.isLoaded:
                            time.sleep(0.01)
                            self.plotPlot(a, index)                            
                        else:
                            self.setupPlot(a, index)
                            self.add(index)
                            self.plotPlot(a, index)

                        if (self.currentPlot.h_setup[index]):
                            self.currentPlot.h_setup[index] = False
                    '''
                    '''

                        #else:
                        #   
                        self.currentPlot.h_setup[index] = False
                        self.plotPlot(a, index)
                    '''
                        
                    #a = self.select_plot(index)

                    #    self.plotPlot(plt.gca(), index)
                    '''
                    if self.h_log[index]:
                            if (DEBUG):
                                print("Histogram at index", index, "need to be log")
                            if self.h_dict[index]["dim"] == 1:
                                ymin, ymax = a.get_ylim()
                                if ymin == 0:
                                    ymin = 0.001
                                a.set_ylim(ymin,ymax)
                                a.set_yscale("log")
                    '''
            self.currentPlot.figure.tight_layout()
            self.currentPlot.canvas.draw_idle()
            self.setAutoscaleAxis()

            return a
        except:
            pass

    ##############
    # 10) Gates
    ##############

    # helper function that converts index of geometry into index of histo list and updates info
    def clickToIndex(self, idx):
        if (DEBUG):
            print("Inside clickToIndex")
            print("histo index", idx)
            print("self.currentPlot.h_dict[idx]['name']", self.currentPlot.h_dict[idx]['name'])
        try:
            index = self.wConf.histo_list.findText(str(self.currentPlot.h_dict[idx]['name']), QtCore.Qt.MatchFixedString)
            if (DEBUG):
                print("index", index)
            # this is to avoid the histogram combobox to not show the first histogram at first
            if index < 0:
                index = 0
                if (DEBUG):
                    print("Index of combobox", index)
            self.wConf.histo_list.setCurrentIndex(index)
            self.updateHistoInfo()
            self.check_histogram();
        except:
            pass
        
            '''

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
        '''
    
    def createSRegion(self):
        print("Clicked createSRegion in tab", self.wTab.currentIndex())        
        
    ##############################
    # 11) 1D/2D region integration
    ##############################

    def copyPopup(self):
        print("Clicked copyPopup in tab", self.wTab.currentIndex())

    ##############################
    # 17) Misc tools
    ##############################

    def createRectangle(self, plot):
        rec = matplotlib.patches.Rectangle((0, 0), 1, 1, ls="-", lw="2", ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)
        return rec
