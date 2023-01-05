#!/usr/bin/env python3
# import modules and packages
import importlib
import io, pickle, traceback, sys, os, subprocess, ast
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
os.environ['XDG_RUNTIME_DIR'] = os.environ.get("PWD")

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
        #self.factory.initialize(self.extraPopup.imaging.clusterAlgo)
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

        # for peak finding
        self.datax = None        
        self.datay = None
        self.peaks = None
        self.properties = None
        self.peak_pos = {}
        self.peak_vl = {}
        self.peak_hl = {}
        self.peak_txt = {}
        self.isChecked = {}

        # overlay
        self.onFigure = False
        
        #################
        # 2) Signals
        #################

        # top menu signals
        self.wTop.updateButton.clicked.connect(self.update)

        self.wTop.extraButton.clicked.connect(self.spfunPopup)
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
        # zoom callback
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[2].triggered.connect(self.zoomCallback)
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
        self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomIn(self.wTab.wPlot[self.wTab.currentIndex()].canvas))
        # minus button
        self.wTab.wPlot[self.wTab.currentIndex()].minusButton.clicked.connect(lambda: self.zoomOut(self.wTab.wPlot[self.wTab.currentIndex()].canvas))        
        # copy attributes
        self.copyAttr.histoAll.clicked.connect(lambda:self.histAllAttr(self.copyAttr.histoAll))
        self.copyAttr.okAttr.clicked.connect(self.okCopy)
        self.copyAttr.applyAttr.clicked.connect(self.applyCopy)
        self.copyAttr.cancelAttr.clicked.connect(self.closeCopy)
        self.copyAttr.selectAll.clicked.connect(self.selectAll)

        # extra popup
        self.extraPopup.fit_button.clicked.connect(self.fit)

        self.extraPopup.peak.peak_analysis.clicked.connect(self.analyzePeak)
        self.extraPopup.peak.peak_analysis_clear.clicked.connect(self.peakAnalClear)

        self.extraPopup.peak.jup_start.clicked.connect(self.jupyterStart)
        self.extraPopup.peak.jup_stop.clicked.connect(self.jupyterStop)

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
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[2].triggered.connect(self.zoomCallback)        
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.setAutoscaleAxis)
        self.wTab.wPlot[self.wTab.currentIndex()].histo_log.clicked.connect(self.setLogAxis)
        self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomIn(self.wTab.wPlot[self.wTab.currentIndex()].canvas))
        self.wTab.wPlot[self.wTab.currentIndex()].minusButton.clicked.connect(lambda: self.zoomOut(self.wTab.wPlot[self.wTab.currentIndex()].canvas))        
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)
        
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
                        if self.currentPlot.h_log[i]:
                            self.wTab.wPlot[self.wTab.currentIndex()].histo_log.setChecked(True)
                        else:
                            self.wTab.wPlot[self.wTab.currentIndex()].histo_log.setChecked(False)                            
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
            self.updatePlot()

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
            self.updatePlot()            

        t=None

    def get_key(self, val):
        for key, value in self.currentPlot.h_dict.items():
            for key2, value2, in value.items():
                if val == value2:
                    return key

    def zoomCallback(self, event):
        if (DEBUG):
            print("Clicked zoomCallback in tab", self.wTab.currentIndex())
        try:
            # update currentPlot limits with what's on the actual plot
            ax = None
            if self.currentPlot.isZoomed:
                ax = plt.gca()
            else:
                ax = self.select_plot(self.currentPlot.selected_plot_index)
            if (DEBUG):
                print(ax.get_xlim(), ax.get_ylim())
                print(self.currentPlot.h_limits)
            self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["x"] = [ax.get_xlim()[0], ax.get_xlim()[1]]
            self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["y"] = [ax.get_ylim()[0], ax.get_ylim()[1]]
        except:
            pass
            
    def homeCallback(self, event):
        if (DEBUG):
            print("Clicked homeCallback in tab", self.wTab.currentIndex())
        try:
            name = str(self.wConf.histo_list.currentText())
            index = self.get_key(name)
            self.resetAxisLimits(index)
            self.currentPlot.canvas.draw()
        except:
            pass
        
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
        
    def selectAll(self):
        flag = False
        basic = ["Ok", "Cancel", "Apply"]
        discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
        for instance in self.copyAttr.findChildren(QPushButton):
            if instance.text() not in discard:
                instance.setChecked(True)
                instance.setStyleSheet('QPushButton {color: green;}')
            else:
                if instance.text() not in basic:
                    if instance.text() == "Select all":
                        instance.setText("Deselect all")
                    else:
                        instance.setText("Select all")
                        flag = True

        if flag == True:
            for instance in self.copyAttr.findChildren(QPushButton):
                if instance.text() not in discard:
                    instance.setChecked(False)
                    instance.setStyleSheet('QPushButton {color: red;}')
                    flag = False

    def histAllAttr(self, b):
        if b.text() == "Select all properties":
            if b.isChecked() == True:
                self.copyAttr.axisLimitX.setChecked(True)
                self.copyAttr.axisLimitY.setChecked(True)
                self.copyAttr.axisScale.setChecked(True)
                self.copyAttr.histoScaleminZ.setChecked(True)
                self.copyAttr.histoScalemaxZ.setChecked(True)
            else:
                self.copyAttr.axisLimitX.setChecked(False)
                self.copyAttr.axisLimitY.setChecked(False)
                self.copyAttr.axisScale.setChecked(False)
                self.copyAttr.histoScaleminZ.setChecked(False)
                self.copyAttr.histoScalemaxZ.setChecked(False)

        if self.wConf.button1D.isChecked():
            self.copyAttr.histoScaleminZ.setEnabled(False)
            self.copyAttr.histoScaleValueminZ.setEnabled(False)
            self.copyAttr.histoScalemaxZ.setEnabled(False)
            self.copyAttr.histoScaleValuemaxZ.setEnabled(False)
        else:
            self.copyAttr.histoScaleminZ.setEnabled(True)
            self.copyAttr.histoScaleValueminZ.setEnabled(True)
            self.copyAttr.histoScalemaxZ.setEnabled(True)
            self.copyAttr.histoScaleValuemaxZ.setEnabled(True)
        
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
        if self.wTab.wPlot[self.wTab.currentIndex()].histo_log.isChecked():
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
                print(self.currentPlot.h_limits)
            if (self.currentPlot.h_dict[index]["dim"] == 1) :
                if ax.get_yscale() == "log":
                    ax.set_yscale("linear")
                    if (self.currentPlot.h_limits[index]):
                        ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])
                    else:
                        ax.set_ylim(self.minY,self.maxY)
            else:
                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)
    
    # setting log/linear axes
    def setLogAxis(self):
        if (DEBUG):
            print("Clicked setLogAxis in tab", self.wTab.currentIndex())
            print("self.currentPlot.h_log", self.currentPlot.h_log)
            print("self.currentPlot.h_log_bak", self.currentPlot.h_log_bak)

        try:
            if self.currentPlot.selected_plot_index != None:
                if (DEBUG):
                    print("histogram selected with index", self.currentPlot.selected_plot_index)
                if self.wTab.wPlot[self.wTab.currentIndex()].histo_log.isChecked():
                    if (DEBUG):
                        print("histogram needs to become log")
                    self.currentPlot.h_log[self.currentPlot.selected_plot_index] = True                    
                else:
                    if (DEBUG):                    
                        print("histogram needs to become linear")
                    self.currentPlot.h_log[self.currentPlot.selected_plot_index] = False
                if (DEBUG):
                    print("Summary: Inside self.currentPlot.selected_plot_index != None  --- self.currentPlot.h_log", self.currentPlot.h_log)
                ax = None
                if self.currentPlot.isZoomed:
                    ax = plt.gca()
                else:
                    ax = self.select_plot(self.currentPlot.selected_plot_index)
                self.axisScale(ax, self.currentPlot.selected_plot_index)
            else:
                if (DEBUG):                
                    print("----> histogram NOT selected - back from zoom mode")
                    print("Summary ----> self.currentPlot.h_log", self.currentPlot.h_log)

                for index, value in self.currentPlot.h_log.items():
                    if value == True:
                        ax = self.select_plot(index)
                        self.axisScale(ax, index)

            self.currentPlot.canvas.draw()

        except NameError:
            raise

    def zoom(self, ax, index, flag):
        if (DEBUG):
            print("Inside zoom")
        if self.wConf.button1D.isChecked():
            ymax = (ax.get_ylim())[1]
            if (DEBUG):
                print("ymax", ymax)
            if flag == "in":
                ymax /= 2
            elif flag == "out":
                ymax *= 2
            if (DEBUG):                
                print("new ymax", ymax)
            ax.set_ylim((ax.get_ylim())[0],ymax)
        else:
            zmax = self.currentPlot.h_lst[index].get_clim()[1]
            if (DEBUG):
                print("zmax",zmax)
            if flag == "in":
                zmax /= 2
            elif flag == "out":
                zmax *= 2
            if (DEBUG):                
                print("new zmax", zmax)            
            self.currentPlot.h_lst[index].set_clim(vmax=zmax)
        
    def zoomIn(self, canvas):
        if (DEBUG):
            print("Inside zoomIn")
        if self.currentPlot.isZoomed == False:
            for i, ax in enumerate(self.currentPlot.figure.axes):
                if (i == self.currentPlot.selected_plot_index):
                    self.zoom(ax, i, "in")
                    try:
                        self.currentPlot.rec.remove()
                        self.currentPlot.rec = self.createRectangle(ax)
                    except:
                        pass
        else:
            ax = plt.gca()
            self.zoom(ax, self.currentPlot.selected_plot_index, "in")

        canvas.draw()

    def zoomOut(self, canvas):
        if (DEBUG):
            print("Inside zoomOut")
        if self.currentPlot.isZoomed == False:
            for i, ax in enumerate(self.currentPlot.figure.axes):
                if (i == self.currentPlot.selected_plot_index):
                    self.zoom(ax, i, "out")
                    try:
                        self.currentPlot.rec.remove()
                        self.currentPlot.rec = self.createRectangle(ax)
                    except:
                        pass
        else:
            ax = plt.gca()
            self.zoom(ax, self.currentPlot.selected_plot_index, "out")

        canvas.draw()
        
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
                # set limits if they exist
                if (self.currentPlot.h_limits[self.currentPlot.selected_plot_index]):
                    ax.set_xlim(self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["x"][0], self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["x"][1])
                    ax.set_ylim(self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["y"][0], self.currentPlot.h_limits[self.currentPlot.selected_plot_index]["y"][1])
                else:
                    ax.set_xlim(float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmin"][0]), float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmin"][1]))
                    ax.set_ylim(float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["ymin"][0]), float(self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["ymin"][1]))

                if self.currentPlot.autoScale:
                    if (DEBUG):
                        print("Inside self.autoScale for tab with index", self.wTab.currentIndex())
                    if (self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["name"]) != "empty":
                        if (DEBUG):
                            print("histogram exists with index", self.currentPlot.selected_plot_index)
                        data = self.get_data(self.currentPlot.selected_plot_index)
                        if (self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["dim"] == 1) :
                            ymax_new = max(data)*1.1
                            ax.set_ylim((ax.get_ylim())[0], ymax_new)
                        else:
                            maxZ = np.max(data)*1.1
                            self.currentPlot.h_lst[self.currentPlot.selected_plot_index].set_clim(vmin=self.minZ, vmax=maxZ)
                else:
                    if (DEBUG):
                        print("Inside not self.autoScale for tab with index", self.wTab.currentIndex())
                    if (self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["dim"] == 1) :
                        ax.set_ylim((ax.get_ylim())[0], (ax.get_ylim())[1])
                    else:
                        self.currentPlot.h_lst[self.currentPlot.selected_plot_index].set_clim(vmin=self.minZ, vmax=self.maxZ)
            else:
                if (DEBUG):
                    print("Inside multipanel option")
                    print("self.currentPlot.autoScale", self.currentPlot.autoScale)
                if self.currentPlot.autoScale:
                    if (DEBUG):
                        print("Inside self.autoScale for tab with index", self.wTab.currentIndex())
                        
                    for index, values in self.currentPlot.h_dict.items():
                        if (DEBUG):
                            print(index, values)
                            
                        if (self.currentPlot.h_dict[index]["name"]) != "empty":                        
                            data = self.get_data(index)
                            ax = self.select_plot(index)
                            if self.currentPlot.h_dict[index]["dim"] == 1:
                                ymax_new = max(data)*1.1
                                ax.set_ylim((ax.get_ylim())[0], ymax_new)
                                if (self.currentPlot.h_limits[index]):
                                    ax.set_xlim(self.currentPlot.h_limits[index]["x"][0], self.currentPlot.h_limits[index]["x"][1])                                
                            else:
                                maxZ = np.max(data)*1.1
                                if (self.currentPlot.h_limits[index]):
                                    ax.set_xlim(self.currentPlot.h_limits[index]["x"][0], self.currentPlot.h_limits[index]["x"][1])
                                    ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])                                    
                                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=maxZ)

                                
                else:
                    if (DEBUG):
                        print("Inside not self.autoScale for tab with index", self.wTab.currentIndex())
                        print(self.currentPlot.h_limits)
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
        if (DEBUG):
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

        #self.currentPlot.figure.tight_layout()
        #self.currentPlot.canvas.draw_idle()
        #self.setAutoscaleAxis()
        #self.setLogAxis()
        
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
                    
                self.currentPlot.h_setup[self.currentPlot.index] = True
                self.erasePlot(self.currentPlot.index)
                #self.add(self.currentPlot.index)
                self.updateSinglePlot(self.currentPlot.index)
                #self.currentPlot.canvas.draw()
                self.currentPlot.isSelected = False

        except NameError:
            raise

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
            else:
                if (DEBUG):
                    print("Inside updatePlot - multipanel mode")
                for index, value in self.currentPlot.h_dict.items():
                    if (DEBUG):
                        print("index", index, "value", value)
                    self.updateSinglePlot(index)

            self.currentPlot.figure.tight_layout()
            self.currentPlot.canvas.draw_idle()
            self.setAutoscaleAxis()
            self.setLogAxis()
            
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
    
    def createSRegion(self):
        print("Clicked createSRegion in tab", self.wTab.currentIndex())        
        
    ##############################
    # 11) 1D/2D region integration
    ##############################

    def spfunPopup(self):
        if self.extraPopup.isVisible():
            self.extraPopup.close()

        self.extraPopup.show()
    
    def okCopy(self):
        if (DEBUG):
            print("Inside okCopy")
        self.applyCopy()
        self.closeCopy()

    def applyCopy(self):
        if (DEBUG):
            print("Inside applyCopy")            
        try:
            flags = []
            for instance in self.copyAttr.findChildren(QCheckBox):
                if instance.isChecked():
                    if (DEBUG):
                        print(instance.text(), instance.isChecked())
                    flags.append(True)
                else:
                    flags.append(False)

            if (DEBUG):
                print(flags)
        
            dim = self.currentPlot.h_dim[self.currentPlot.selected_plot_index]
            keys = []
            values = []
            xlim_src = []
            ylim_src= []
            zlim_src= []            
            scale_src = None
            discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
            # creating list of target histograms
            for instance in self.copyAttr.findChildren(QPushButton):
                if (instance.text() not in discard) and instance.isChecked():
                    if (DEBUG):
                        print("histo destination",instance.text())
                    keys=list(self.currentPlot.h_dict_geo.keys())
                    values=list(self.currentPlot.h_dict_geo.values())

            index_og = keys[values.index(self.wConf.histo_list.currentText())]
            if (DEBUG):
                print(self.currentPlot.selected_plot_index, index_og)
                print(keys)
                print(values)
            # remove source element
            keys.pop(self.currentPlot.selected_plot_index)
            values.pop(self.currentPlot.selected_plot_index)
            # src values to copy to destination
            xlim_src = ast.literal_eval(self.copyAttr.axisLimLabelX.text())
            ylim_src = ast.literal_eval(self.copyAttr.axisLimLabelY.text())
            scale_src = self.copyAttr.axisSLabel.text()
            zlim_src = [float(self.copyAttr.histoScaleValueminZ.text()), float(self.copyAttr.histoScaleValuemaxZ.text())]
            if (DEBUG):
                print(xlim_src, ylim_src, scale_src, zlim_src)
                print(flags)
            
            # copy to destination
            for index in keys:
                # set the limits for x,y
                if flags[0] == True:
                    self.currentPlot.h_limits[index]["x"] = xlim_src
                    self.currentPlot.h_setup[index] = True
                if flags[1] == True:
                    self.currentPlot.h_limits[index]["y"] = ylim_src
                    self.currentPlot.h_setup[index] = True
                # set log/lin scale
                if flags[2] == True:
                    self.currentPlot.h_log[index] = scale_src
                    self.currentPlot.h_setup[index] = True
                # set minZ/maxZ
                if (flags[3] == True or flags[4] == True) and self.wConf.button2D.isChecked():
                    self.currentPlot.h_setup[index] = True

            if (DEBUG):                    
                print("before applying to destination...")
                    
            ax = None
            if not self.currentPlot.isZoomed:
                for index in range(len(self.currentPlot.h_setup)):
                    # match dimension of the selected histogram (1d/2d)
                    if self.currentPlot.h_dim[index] == dim:
                        # select axes
                        ax = self.select_plot(index)
                        # modifying axis limits
                        ax.set_xlim(xlim_src[0], xlim_src[1])
                        ax.set_ylim(ylim_src[0], ylim_src[1])
                        # modifying log/linear
                        if self.currentPlot.h_log[index]:
                            self.axisScale(ax, index)
                        # for 2D plot sets limits
                        if dim == 2:
                            self.currentPlot.h_lst[index].set_clim(vmin=zlim_src[0], vmax=zlim_src[1])

            self.currentPlot.canvas.draw()

        except:
            pass
            
    def closeCopy(self):
        discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
        for instance in self.copyAttr.findChildren(QPushButton):
            if instance.text() not in discard:
                instance.deleteLater()

        self.copyAttr.close()
            
    def copyPopup(self):
        try:
            if self.copyAttr.isVisible():
                self.copyAttr.close()
            
            if (DEBUG):            
                print("Clicked copyPopup in tab", self.wTab.currentIndex())
            self.copyAttr.histoLabel.setText(self.wConf.histo_list.currentText())
            hdim = 1
            if self.wConf.button2D.isChecked():
                hdim = 2
            # setting up info for source histogram
            for index, values in self.currentPlot.h_dict.items():
                if (DEBUG):
                    print(index)
                for idx, value in values.items():
                    if (DEBUG):
                        print(idx, value)
                    if idx == "name":
                        if value == self.wConf.histo_list.currentText():
                            if (DEBUG):
                                print("histo chosen", value)
                            # log scale check
                            if self.currentPlot.h_log[index] == True:
                                self.copyAttr.axisSLabel.setText("Log")
                            else:
                                self.copyAttr.axisSLabel.setText("Linear")
                            self.copyAttr.axisLimLabelX.setText("["+'{:.1f}'.format((self.currentPlot.h_limits[index]["x"])[0])+","+'{:.1f}'.format((self.currentPlot.h_limits[index]["x"])[1])+"]")
                            self.copyAttr.axisLimLabelY.setText("["+'{:.1f}'.format((self.currentPlot.h_limits[index]["y"])[0])+","+'{:.1f}'.format((self.currentPlot.h_limits[index]["y"])[1])+"]")
                        else:
                            if hdim == values["dim"]:
                                instance = QPushButton(value, self)
                                instance.setCheckable(True)
                                instance.setStyleSheet('QPushButton {color: red;}')
                                self.copyAttr.copy_log.addRow(instance)
                                instance.clicked.connect(
                                    lambda state, instance=instance: self.connectCopy(instance))
                            
        except:
            pass
        self.copyAttr.show()

    def connectCopy(self, instance):
        if (instance.palette().color(QPalette.Text).name() == "#008000"):
            instance.setStyleSheet('QPushButton {color: red;}')
        else:
            instance.setStyleSheet('QPushButton {color: green;}')
        if (DEBUG):
            print(instance.isChecked())

    ############################
    # 12)  Fitting
    ############################

    def axislimits(self, ax):
        left, right = ax.get_xlim()
        if self.extraPopup.fit_range_min.text():
            left = int(self.extraPopup.fit_range_min.text())
        else:
            left = ax.get_xlim()[0] 
        if self.extraPopup.fit_range_max.text():
            right = int(self.extraPopup.fit_range_max.text())
        else:
            right = ax.get_xlim()[1] 
        return left, right

    def fit(self):
        ax = None
        histo_name = str(self.wConf.histo_list.currentText())
        fit_funct = self.extraPopup.fit_list.currentText()
        if self.currentPlot.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(self.currentPlot.selected_plot_index)

        config = self.fit_factory._configs.get(fit_funct)
        if (DEBUG):
            print("Fit function", config)
        fit = self.fit_factory.create(fit_funct, **config)

        try:
            if histo_name != "":
                if self.wConf.button1D.isChecked():
                    if (DEBUG):
                        print("Ready to 1D fit...")
                    x = []
                    y = []
                    # input points for fitting function
                    minx = self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmin"]
                    maxx = self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmax"]                    
                    binx = self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xbin"]
                    print(minx, maxx, binx)
                    xtmp = self.create_range(binx, minx, maxx)
                    fitpar = [float(self.extraPopup.fit_p0.text()), float(self.extraPopup.fit_p1.text()),
                              float(self.extraPopup.fit_p2.text()), float(self.extraPopup.fit_p3.text()),
                              float(self.extraPopup.fit_p4.text()), float(self.extraPopup.fit_p5.text()),
                              float(self.extraPopup.fit_p6.text()), float(self.extraPopup.fit_p7.text())]

                    if (DEBUG):
                        print(fitpar)
                    ytmp = (self.get_data(self.currentPlot.selected_plot_index)).tolist()
                    if (DEBUG):
                        print("xtmp", type(xtmp), "with len", len(xtmp), "ytmp", type(ytmp), "with len", len(ytmp))
                    xmin, xmax = self.axislimits(ax)
                    if (DEBUG):
                        print("fitting axis limits", xmin, xmax)
                        print(type(xtmp), type(x), type(xtmp[0]), type(xmin))
                    # create new tmp list with subrange for fitting
                    for i in range(len(xtmp)):
                        if (xtmp[i]>=xmin and xtmp[i]<xmax):
                            x.append(xtmp[i])
                            y.append(ytmp[i])
                    x = np.array(x)
                    y = np.array(y)
                            
                    fitln = fit.start(x, y, xmin, xmax, fitpar, ax, self.extraPopup.fit_results)
                else: 
                    QMessageBox.about(self, "Warning", "Sorry 2D fitting is not implemented yet")
            else:
                QMessageBox.about(self, "Warning", "Histogram not existing. Please load an histogram...")

            self.currentPlot.canvas.draw()

        except NameError:
            raise

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
                    self.drawSinglePeaks(self.peaks, self.properties, self.datay, i)
                    self.isChecked[i] = True

        self.currentPlot.canvas.draw()

    def create_peak_signals(self, peaks):
        try:
            for i in range(len(peaks)):
                self.isChecked[i] = False
                self.extraPopup.peak.peak_cbox[i].stateChanged.connect(self.peakState)
                self.extraPopup.peak.peak_cbox[i].setChecked(True)
        except:
            pass
                
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

        self.currentPlot.canvas.draw()

    def drawSinglePeaks(self, peaks, properties, data, index):
        if (DEBUG):
            print("inside drawSinglePeaks")
        ax = None
        if self.currentPlot.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(self.currentPlot.selected_plot_index)

        x = self.datax.tolist()
        if (DEBUG):
            print("self.peak_pos[index]", peaks[index], int(x[peaks[index]]))
        self.peak_pos[index] = ax.plot(x[peaks[index]], int(data[peaks[index]]), "v", color="red")
        self.peak_vl[index] = ax.vlines(x=x[peaks[index]], ymin=data[peaks[index]] - properties["prominences"][index], ymax = data[peaks[index]], color = "red")
        self.peak_hl[index] = ax.hlines(y=properties["width_heights"][index], xmin=properties["left_ips"][index], xmax=properties["right_ips"][index], color = "red")
        self.peak_txt[index] = ax.text(x[peaks[index]], int(data[peaks[index]]*1.1), str(int(x[peaks[index]])))        

    def update_peak_output(self, peaks, properties):
        if (DEBUG):
            print("Inside update_peak_output")
            print(type(peaks), peaks)
        x = self.datax.tolist()
        if (DEBUG):
            print(type(x), len(x), x)        
        for i in range(len(peaks)):
            if (DEBUG):
                print("peak at index", peaks[i], "corresponds to x value of", x[peaks[i]])
            s = "Peak"+str(i+1)+"\n\tpeak @ " + str(int(x[peaks[i]]))+", FWHM="+str(int(properties['widths'][i]))
            self.extraPopup.peak.peak_results.append(s)
        
    def analyzePeak(self):
        try:
            ax = None
            if self.currentPlot.isZoomed:
                ax = plt.gca()
            else:
                ax = self.select_plot(self.currentPlot.selected_plot_index)

            x = []
            y = []
            # input points for peak finding
            width = int(self.extraPopup.peak.peak_width.text())
            minx = self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmin"]
            maxx = self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xmax"]
            binx = self.currentPlot.h_dict[self.currentPlot.selected_plot_index]["xbin"]
            if (DEBUG):
                print(minx, maxx, binx)
            xtmp = self.create_range(binx, minx, maxx)
            ytmp = (self.get_data(self.currentPlot.selected_plot_index)).tolist()
            if (DEBUG):
                print("xtmp", type(xtmp), "with len", len(xtmp), "ytmp", type(ytmp), "with len", len(ytmp))
            xmin, xmax = ax.get_xlim()
            if (DEBUG):
                print("fitting axis limits", xmin, xmax)
            # create new tmp list with subrange for fitting
            for i in range(len(xtmp)):
                if (xtmp[i]>=xmin and xtmp[i]<xmax):
                    x.append(xtmp[i])
                    y.append(ytmp[i])
            self.datax = np.array(x)
            self.datay = np.array(y)
            if (DEBUG):
                print(self.datax)
                print(self.datay)
                print("xtmp", type(self.datax), "with len", len(self.datax.tolist()), "ytmp", type(self.datay), "with len", len(self.datay.tolist()))            
            self.peaks, self.properties = find_peaks(self.datay, prominence=1, width=width)

            if (DEBUG):            
                print("peak list with indices", self.peaks)
                print("peak properties list", self.properties)
            self.update_peak_output(self.peaks, self.properties)
            self.create_peak_signals(self.peaks)

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
        if (DEBUG):
            print(fileName)
        try:
            if os.path.isfile(fileName):
                self.LISEpic = cv2.imread(fileName, 0)
                cv2.resize(self.LISEpic, (200, 100))
        except:
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
        if (DEBUG):
            print("inside indexToStartPosition")
        row = int(self.wConf.histo_geo_row.currentText())
        col = int(self.wConf.histo_geo_col.currentText())
        if (DEBUG):
            print("row, col",row, col)
        xoffs = float(1/(2*col))
        yoffs = float(1/(2*row))
        i, j = self.plot_position(index)
        if (DEBUG):
            print("plot position in geometry", i, j)
        xstart = xoffs*(2*j+1)-0.1
        ystart = yoffs*(2*i+1)+0.1

        self.xstart = xstart
        self.ystart = 1-ystart
        if (DEBUG):
            print("self.xstart", self.xstart, "self.ystart", self.ystart)

    def drawFigure(self):
        self.alpha = self.extraPopup.imaging.alpha_slider.value()/10
        self.zoomX = self.extraPopup.imaging.zoomX_slider.value()/10
        self.zoomY = self.extraPopup.imaging.zoomY_slider.value()/10

        ax = plt.axes([self.xstart, self.ystart, self.zoomX, self.zoomY], frameon=True)
        ax.axis('off')
        self.imgplot = ax.imshow(self.LISEpic,
                                 aspect='auto',
                                 alpha=self.alpha)

        self.currentPlot.canvas.draw()

    def deleteFigure(self):
        self.imgplot.remove()
        self.onFigure = False
        self.currentPlot.canvas.draw()

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
            self.indexToStartPosition(self.currentPlot.selected_plot_index)
            if self.onFigure == False:
                self.drawFigure()
                self.onFigure = True
        except NameError:
            raise
            #QMessageBox.about(self, "Warning", "Please select one histogram...")
        
    ############################
    # 16) Jupyter Notebook
    ############################

    def createDf(self):
        try:
            if (DEBUG):
                print("Create dataframe for Jupyter and web")
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

            self.spectrum_list.to_csv(self.extraPopup.peak.jup_df_filename.text(), index=False, compression='gzip')
        except:
            pass
            
    def jupyterStop(self):
        # stop the notebook process
        log("Sending interrupt signal to jupyter-notebook")
        self.extraPopup.peak.jup_start.setEnabled(True)
        self.extraPopup.peak.jup_stop.setEnabled(False)
        self.extraPopup.peak.jup_start.setStyleSheet("background-color:#3CB371;")
        self.extraPopup.peak.jup_stop.setStyleSheet("")
        stopnotebook()

    def jupyterStart(self):
        # dump df to gzip
        self.createDf()
        #starting jupyter server
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
        
    ##############################
    # 17) Misc tools
    ##############################

    def createRectangle(self, plot):
        rec = matplotlib.patches.Rectangle((0, 0), 1, 1, ls="-", lw="2", ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)
        return rec

# redirect logging
class QtLogger(QObject):
    newlog = pyqtSignal(str)

    def __init__(self, parent):
        super(QtLogger, self).__init__(parent)
