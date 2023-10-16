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
import multiprocessing

sys.path.append(os.getcwd())

# use preprocessor macro __file__ to get the installation directory
# caveat : expects a particular format of installation directory (N.NN-NNN)
instPath = ""
fileDir = os.path.dirname(os.path.abspath(__file__))
subDirList = fileDir.split("/")
for subDir in subDirList:
    if subDir != "":
        instPath += "/"+subDir
        if re.search(r'\d+\.\d{2}-\d{3}', subDir ):
            # specVersion = subDir
            break

sys.path.append(instPath + "/lib")

# removes the webproxy from spdaq machines
os.environ['NO_PROXY'] = ""
os.environ['XDG_RUNTIME_DIR'] = os.environ.get("PWD")

from PyQt5 import QtCore, QtNetwork
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtTest import *

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
import matplotlib.colorbar as mcolorbar
from matplotlib.backend_bases import *
from matplotlib.artist import Artist

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
from MenuAndConfigGUI import * # include spectrum/gate info and output popup buttons
from SpecialFunctionsGUI import SpecialFunctions # all the extra functions we defined
from OutputGUI import OutputPopup # popup output window
from PlotGUI import Plot # area defined for the histograms
from PlotGUI import Tabs # area defined for the Tabs
from PyREST import PyREST # class interface for SpecTcl REST plugin
from CopyPropertiesGUI import CopyProperties
from connectConfigGUI import ConnectConfiguration

from logger import log, setup_logging, set_logger
from notebook_process import testnotebook, startnotebook, stopnotebook
from WebWindow import WebWindow

#from collapseMenu import Spoiler

SETTING_BASEDIR = "workdir"
SETTING_EXECUTABLE = "exec"
DEBUG = False
DEBOUNCE_DUR = 0.25
t = None

# 0) Class definition
class MainWindow(QMainWindow):

    stop_signal = pyqtSignal()

    #Simon - flag that gives the status of the ReST thred
    restThreadFlag = False

    #Simon - initialize a status bar with ReST thread status
    # def initStatusBar(self):
    #     self.statBar = self.statusBar()
    #     self.statBar.showMessage('Thread for trace : OFF')
    #     self.lineEdit = QLineEdit()
    #     self.setCentralWidget(self.lineEdit)
    #     self.lineEdit.textEdited.connect(self.updateStatusBar)

    #Simon - update status bar with ReST thread status
    # def updateStatusBar(self):
        # status = "Thread for trace : ON" if self.restThreadFlag else "Thread for trace : OFF"
        # self.topMenu.setConnectSatus(self.restThreadFlag)
        # self.statBar.showMessage(status)

    def __init__(self, factory, fit_factory, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        #Simon - status bar for ReST thread
        # self.initStatusBar()

        # status = "Thread for trace : ON" if self.restThreadFlag else "Thread for trace : OFF"
        # self.statusBarLabel = QLabel("Thread for trace : OFF")
        # self.statusbar.addPermanentWidget(self.statusBarLabel)
        # self.statBar.showMessage('Thread for trace : OFF')
        # self.lineEdit = QLineEdit()
        # self.setCentralWidget(self.lineEdit)
        # self.lineEdit.textEdited.connect(self.updateStatusBar)
        # self.updateStatusBar()

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

        mainLayout = QGridLayout()
        fullLayout = QGridLayout()
        mainLayout.setContentsMargins(10, 10, 10, 0)
        fullLayout.setContentsMargins(0, 0, 0, 0)

        # top menu
        #self.wTop = Menu()
        #comment the following if use collapsable
        # self.wTop.setFixedHeight(40)

        # config menu
        self.wConf = Configuration()
        # self.wConf.setFixedHeight(40)

        # plot widget
        self.wTab = Tabs()
        self.wTab.setMovable(True)
        self.currentPlot = None

        # gui composition
        self.wConf.setContentsMargins(10, 10, 10, 0)

        fullLayout.addLayout(self.wConf, 1, 0, 1, 0)
        fullLayout.addWidget(self.wTab, 2, 0, 1, 0)


        widget = QWidget()
        widget.setLayout(fullLayout)
        # widget.setLayout(mainLayout)
        self.setCentralWidget(widget)

        # output popup window
        self.resPopup = OutputPopup()
        self.table_row = []
        self.ctr = 0

        # extra popup window
        self.extraPopup = SpecialFunctions()

        # Tab editing popup
        self.tabp = TabPopup()

        # connection configuration windows
        self.connectConfig = ConnectConfiguration()

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
        #spectrum_dict_rest {key:name_spectrum, value:{info_spectrum}}, created when connect and updated continuously with traces (if any), hidden to user.
        self.spectrum_dict_rest = {}


        # default min/max for x,y
        self.minX = 0
        self.maxX = 1024
        self.minY = 0.001
        self.maxY = 1024
        # default gradient for 2d plots
        self.minZ = 0.001
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

        # editing gates
        self.showverts = True
        self.epsilon = 5  # max pixel distance to count as a vertex hit
        self._ind = None # the active vert

        self.bPressed = False

        #################
        # 2) Signals
        #################

        # top menu signals
        self.wConf.connectButton.clicked.connect(self.connectPopup)
        self.connectConfig.ok.clicked.connect(self.okConnect)
        self.connectConfig.cancel.clicked.connect(self.closeConnect)

        self.wConf.saveButton.clicked.connect(self.saveGeo)
        self.wConf.loadButton.clicked.connect(self.loadGeo)
        self.wConf.exitButton.clicked.connect(self.closeAll)

        # new tab creation
        self.wTab.tabBarClicked.connect(self.clickedTab)
        self.wTab.tabBarDoubleClicked.connect(self.doubleclickedTab)

        # config menu signals
        self.wConf.histo_geo_add.clicked.connect(self.addPlot)
        self.wConf.histo_geo_update.clicked.connect(self.updatePlot)
        self.wConf.extraButton.clicked.connect(self.spfunPopup)

        self.wConf.histo_geo_row.activated.connect( self.setCanvasLayout )
        self.wConf.histo_geo_col.activated.connect( self.setCanvasLayout )

        self.wConf.createGate.clicked.connect(self.createGate)
        self.wConf.createGate.setEnabled(False)
        self.wConf.editGate.setEnabled(False)
        self.wConf.menu.triggered.connect(self.editGate)
        self.wConf.editGate.setToolTip("Key bindings for Modify->Edit:\n"
                                      "'i' insert vertex\n"
                                      "'d' delete vertex\n")

        self.wConf.integrateGate.clicked.connect(self.integrate)

        self.wConf.button2D_option.activated.connect(self.changeBkg)

        self.tabp.okButton.clicked.connect(self.okTab)
        self.tabp.cancelButton.clicked.connect(self.cancelTab)

        # zoom callback
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[1].triggered.connect(self.zoomCallback)
        # copy properties
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)
        # summing region
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.clicked.connect(self.createSRegion)
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.setEnabled(False)
        # autoscale
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.autoScaleAxisBox)
        # plus button
        self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomInOut("in"))
        # minus button
        self.wTab.wPlot[self.wTab.currentIndex()].minusButton.clicked.connect(lambda: self.zoomInOut("out"))
        # copy attributes
        self.copyAttr.histoAll.clicked.connect(lambda: self.histAllAttr(self.copyAttr.histoAll))
        self.copyAttr.okAttr.clicked.connect(self.okCopy)
        self.copyAttr.applyAttr.clicked.connect(self.applyCopy)
        self.copyAttr.cancelAttr.clicked.connect(self.closeCopy)
        self.copyAttr.selectAll.clicked.connect(self.selectAll)
        # Custom Home button
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.clicked.connect(lambda: self.customHomeButtonCallback(self.currentPlot.selected_plot_index))
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.customContextMenuRequested.connect(self.handle_right_click)
        #log button
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.clicked.connect(lambda: self.logButtonCallback(self.currentPlot.selected_plot_index))
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.customContextMenuRequested.connect(self.log_handle_right_click)

        self.wTab.countClickTab[self.wTab.currentIndex()] = True

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
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_release_event", self.on_release)

        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.histoHover)

        # create helpers
        self.wConf.histo_list.installEventFilter(self)
        self.wConf.listGate.installEventFilter(self)
        self.wConf.listGate.installEventFilter(self)

        self.currentPlot = self.wTab.wPlot[self.wTab.currentIndex()] # definition of current plot

    ################################
    # 3) Implementation of Signals
    ################################

    #So that signals work for each tab, called in clickedTab()
    def bindDynamicSignal(self):
        for index, val in self.wTab.countClickTab.items():
            if val:
                self.wTab.wPlot[index].logButton.disconnect()
                self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.disconnect()
                self.wTab.wPlot[self.wTab.currentIndex()].plusButton.disconnect()
                self.wTab.wPlot[self.wTab.currentIndex()].minusButton.disconnect()
                self.wTab.wPlot[self.wTab.currentIndex()].copyButton.disconnect()
                self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.disconnect()
                self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.disconnect()
                self.wTab.countClickTab[index] = False

        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[1].triggered.connect(self.zoomCallback)
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.autoScaleAxisBox)
        self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomInOut("in"))
        self.wTab.wPlot[self.wTab.currentIndex()].minusButton.clicked.connect(lambda: self.zoomInOut("out"))
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.clicked.connect(lambda: self.customHomeButtonCallback(self.currentPlot.selected_plot_index))
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.customContextMenuRequested.connect(self.handle_right_click)
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.clicked.connect(lambda: self.logButtonCallback(self.currentPlot.selected_plot_index))
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.customContextMenuRequested.connect(self.log_handle_right_click)

        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.clicked.connect(self.createSRegion)
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.setEnabled(False)

        self.resizeID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("resize_event", self.on_resize)
        self.pressID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.on_press)
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_release_event", self.on_release)

        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.histoHover)

        self.wTab.countClickTab[self.wTab.currentIndex()] = True


    def connect(self):
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.on_press)

    def disconnect(self):
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.pressID)

    #Event filter for search in histo_list widget
    def eventFilter(self, obj, event):
        #Simon - parameter
        if (obj == self.wConf.histo_list or self.wConf.listGate) and event.type() == QtCore.QEvent.HoverEnter:
            self.onHovered(obj)
        return super(MainWindow, self).eventFilter(obj, event)

    def onHovered(self, obj):
        if (obj == self.wConf.histo_list):
            self.wConf.histo_list.setToolTip(self.wConf.histo_list.currentText())
        elif (obj == self.wConf.listGate):
            self.wConf.listGate.setToolTip(self.wConf.listGate.currentText())

    def histoHover(self, event):
        try:
            index = 0
            if not event.inaxes: return

            index = list(self.currentPlot.figure.axes).index(event.inaxes)
            #introduced for custom zoom button/function
            xTitle = self.getSpectrumInfoREST("parameters", index=index)[0]
            coordinates = self.getPointerInfo(event, "coordinates", index)
            if self.getSpectrumInfoREST("dim", index=index) == 1:
                if self.getSpectrumInfoREST("type", index=index) == "g1" :
                    xTitle = self.getSpectrumInfoREST("parameters", index=index)[0] + ", ..."
                self.currentPlot.histoLabel.setText("Histogram: "+self.nameFromIndex(index)+"\nX: "+xTitle)
                self.currentPlot.pointerLabel.setText(f"Pointer:\nX: {coordinates[0]:.2f} Y: {coordinates[1]:.0f}")
            elif self.getSpectrumInfoREST("dim", index=index) == 2:
                yTitle = self.getSpectrumInfoREST("parameters", index=index)[1]
                if self.getSpectrumInfoREST("type", index=index) == "g2" or self.getSpectrumInfoREST("type", index=index) == "m2" or self.getSpectrumInfoREST("type", index=index) == "gd":
                    xTitle = self.getSpectrumInfoREST("parameters", index=index)[0] + ", ..."
                    yTitle = self.getSpectrumInfoREST("parameters", index=index)[1] + ", ..."
                self.currentPlot.histoLabel.setText("Histogram: "+self.nameFromIndex(index)+"\nX: "+xTitle+" Y: "+yTitle) 
                self.currentPlot.pointerLabel.setText(f"Pointer:\nX: {coordinates[0]:.2f} Y: {coordinates[1]:.2f}  Z: {coordinates[2]:.0f}")        
                if self.getSpectrumInfoREST("type", index=index) == "s" :
                    xTitle = self.getSpectrumInfoREST("parameters", index=index)[0] + ", ..."
                    self.currentPlot.histoLabel.setText("Histogram: "+self.nameFromIndex(index)+"\nX: "+xTitle) 
                    self.currentPlot.pointerLabel.setText(f"Pointer:\nX: {coordinates[0]:.2f} Y: {coordinates[1]:.2f}  Z: {coordinates[2]:.0f}") 
        except:
            pass

    def getPointerInfo(self, event, info, index):
        result = ['','','']
        if self.getEnlargedSpectrum():
            index = self.getEnlargedSpectrum()[0]
        try:
            ax = self.getSpectrumInfo("axis", index=index)
            dim = self.getSpectrumInfoREST("dim", index=index)
            minx = self.getSpectrumInfoREST("minx", index=index)
            maxx = self.getSpectrumInfoREST("maxx", index=index)
            binx = self.getSpectrumInfoREST("binx", index=index)
            data = self.getSpectrumInfoREST("data", index=index)
            if ax is None or len(data) <= 0:
                return result
            x, y = ax.transData.inverted().transform([event.x, event.y])
            stepx = (float(maxx)-float(minx))/float(binx)
            binminx = int((x-minx)/stepx)
            if dim == 1:
                if "coordinates" == info:
                    #shift of 1 to investigate...
                    y = data[binminx+1:binminx+2]
                    # y = data[binminx:binminx+1]
                    result = [x,y[0],'']
                elif "bins" == info:
                    result = [binminx,'','']
            elif dim == 2:
                if "coordinates" == info:
                    miny = self.getSpectrumInfoREST("miny", index=index)
                    maxy = self.getSpectrumInfoREST("maxy", index=index)
                    biny = self.getSpectrumInfoREST("biny", index=index)
                    stepy = (float(maxy)-float(miny))/float(biny)
                    binminy = int((y-miny)/stepy)
                    #ndarray [row][column]
                    z = data[binminy:binminy+1, binminx:binminx+1]
                    result = [x,y,z[0][0]]
                elif "bins" == info:
                    result = [binminx, binminy,'']
        except NameError:
            raise

        return result
     

    def on_resize(self, event):
        self.currentPlot.figure.tight_layout()
        self.currentPlot.canvas.draw()


    # Introduced for endding zoom action (toolbar) on release
    # For this purpose dont need to check if release outside of axis (it can happen)
    # small delay introduced such that updatePlotLimits is executed after on_release.
    def on_release(self, event):
        # print("Simon - on_release - self.currentPlot.zoomPress",self.currentPlot.zoomPress,self.currentPlot.canvas.toolbar.actions()[1].isChecked())
        if self.currentPlot.zoomPress:
            self.currentPlot.canvas.toolbar.actions()[1].triggered.emit()
            self.currentPlot.canvas.toolbar.actions()[1].setChecked(False)
            threadLimits = threading.Thread(target=self.updatePlotLimits, args=(0.05,))
            threadLimits.start()
            self.currentPlot.zoomPress = False


    # when mouse pressed in main window
    # Introduced for endding zoom action (toolbar) see on_release and on_press too
    def mousePressEvent(self, event: QMouseEvent) -> None:
        if not self.currentPlot.zoomPress : return 
        #height and width determined empirically... better if overestimated because event handled by on_press in that case
        #these values doesnt change with window resizing but could change if decide to change the layout.
        footH = 15 
        headerH = 130 
        sidesW = 10 

        leftLimit = sidesW
        rightLimit = self.geometry().width() - sidesW
        topLimit = headerH
        bottomLimit = self.geometry().height() - footH

        withinLimits = True if (event.x() in range(leftLimit,rightLimit)) and (event.y() in range(topLimit,bottomLimit)) else False

        if not withinLimits:
            self.currentPlot.canvas.toolbar.actions()[1].triggered.emit()
            self.currentPlot.canvas.toolbar.actions()[1].setChecked(False)
            self.currentPlot.zoomPress = False


    def on_press(self, event):
        #if initate zoom (magnifying glass) but dont press in axes, reset the action
        if self.currentPlot.zoomPress and not event.inaxes: 
            self.currentPlot.canvas.toolbar.actions()[1].triggered.emit()
            self.currentPlot.canvas.toolbar.actions()[1].setChecked(False)
            self.currentPlot.zoomPress = False

        if not event.inaxes: return

        index = list(self.currentPlot.figure.axes).index(event.inaxes)
        if self.currentPlot.isEnlarged:
            index = self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]
        self.currentPlot.selected_plot_index = index
        global t
        if t is None:
            t = threading.Timer(DEBOUNCE_DUR, self.on_singleclick, [event,index])
            t.start()
        if event.dblclick:
            t.cancel()
            try:
                self.on_dblclick(event, index)
            except:
                pass


    def on_singleclick(self, event, index):
        global t
        if (DEBUG):
            print("Inside on_singleclick in tab",self.wTab.currentIndex())
        # change the log button status manually only here according to spectrum info
        # so that when one clicks on a spectrum the button shows if log or not
        axisIsLog = self.getSpectrumInfo("log", index=index)
        wPlot = self.currentPlot
        logBut = wPlot.logButton
        if axisIsLog :
            logBut.setDown(True)
        else :
            logBut.setDown(False) 
        # If we are not zooming on one histogram we can select one histogram
        # and a red rectangle will contour the plot
        if self.currentPlot.isEnlarged == False:
            if (DEBUG):
                print("Inside on_singleclick - ZOOM false")
            self.removeRectangle()
            self.currentPlot.isSelected = True
            self.currentPlot.next_plot_index = self.currentPlot.selected_plot_index
            self.currentPlot.rec = self.createRectangle(self.currentPlot.figure.axes[index])
            #tried to blit here but not successful (?) important delay for canvas with many plots
            self.currentPlot.canvas.draw()
        else:
            if (DEBUG):
                print("Inside on_singleclick - ZOOM true")
                print("self.currentPlot.toCreateGate",self.currentPlot.toCreateGate)
                print("self.currentPlot.toCreateSRegion", self.currentPlot.toCreateSRegion)
            # we can create the gate now..
            if self.currentPlot.toCreateGate == True or self.currentPlot.toCreateSRegion == True: # inside gate creation mode
                if (DEBUG):
                    print("create the gate...")
                click = [int(float(event.xdata)), int(float(event.ydata))]
                # create interval (1D)
                if self.wConf.button1D.isChecked():
                    if (DEBUG):
                        print("inside create gate 1d")
                    l = self.addLine(click[0])
                    self.currentPlot.listLine.append(l)
                    # removes the lines from the plot
                    if len(self.currentPlot.listLine) > 2:
                        self.removeLine()
                    if (DEBUG):
                        print(self.currentPlot.listLine)
                else:
                    if (DEBUG):
                        print("inside create gate 2d")
                    self.addPolygon(click[0], click[1])
            self.currentPlot.canvas.draw()
        t = None


    def on_dblclick(self, event, idx):
        global t

        if (DEBUG):
            print("Inside on_dblclick in tab", self.wTab.currentIndex())

        name = self.nameFromIndex(idx)
        index = self.wConf.histo_list.findText(name)
        self.wConf.histo_list.setCurrentIndex(index)
        # self.updateHistoInfo(name)
        if (DEBUG):
            print("UPDATE plot the histogram at index", idx, "with name", self.wConf.histo_list.currentText())

        if self.currentPlot.isEnlarged == False: # entering zooming mode
            if (DEBUG):
                print("###### Entering zooming mode...")
            self.removeRectangle()

            #important that zoomPlotInfo is set only while in zoom mode (not None only here)
            self.setEnlargedSpectrum(idx, name)

            self.currentPlot.next_plot_index = self.currentPlot.selected_plot_index
            self.currentPlot.isEnlarged = True
            # disabling adding histograms
            self.wConf.histo_geo_add.setEnabled(False)
            # disabling changing canvas layout
            self.wConf.histo_geo_row.setEnabled(False)
            self.wConf.histo_geo_col.setEnabled(False)
            # enabling gate creation
            self.currentPlot.createSRegion.setEnabled(True)
            self.wConf.createGate.setEnabled(True)
            self.wConf.editGate.setEnabled(True)
            if (DEBUG):
                print("inside dblclick: self.selected_plot_index",idx )
            # backing up list of histogram
            self.currentPlot.h_dict_bak = self.currentPlot.h_dict.copy()
            # plot corresponding histogram
            self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]= deepcopy(idx)

            #setup single pad canvas
            self.currentPlot.InitializeCanvas(1,1,False)
            self.add(idx)
            self.currentPlot.h_setup[idx] = False
            self.updatePlot()

        else:
            # enabling adding histograms
            self.wConf.histo_geo_add.setEnabled(True)
            # enabling changing canvas layout
            self.wConf.histo_geo_row.setEnabled(True)
            self.wConf.histo_geo_col.setEnabled(True)

            if self.currentPlot.toCreateGate == True or self.currentPlot.toCreateSRegion == True:
                if (DEBUG):
                    print("Fixing index before closing the gate")
                    print(idx,"has to be",self.wTab.selected_plot_index_bak[self.wTab.currentIndex()])
                    print("Before histo name", self.wConf.histo_list.currentText())
                name = self.nameFromIndex[0]
                index = self.wConf.histo_list.findText(name)
                if (DEBUG):
                    print("histogram",name,"has index", index)
                self.wConf.histo_list.setCurrentIndex(index)
                # self.updateHistoInfo(name)
                if (DEBUG):
                    print("Now histo name", self.wConf.histo_list.currentText())
                # ax = plt.gca()
                # ax = self.getSpectrumAxis(0)
                ax = self.getSpectrumInfo("axis", index=0)
                histo_name = self.wConf.histo_list.currentText()
                gate_name = ""
                if self.currentPlot.toCreateGate == True:
                    gate_name = self.wConf.listGate.currentText()
                else:
                    gate_name = self.currentPlot.region_name
                #Simon - added try to be able to exit the if statment if dble click right after created a gate

                try :
                    if self.wConf.button1D.isChecked():
                        if (DEBUG):
                            print("Closing 1D gating")

                        #Simon - added following because xs size can be 1 if double click before drawing a second line (single click)
                        if len(self.currentPlot.xs)==1:
                            click = [int(float(event.xdata)), int(float(event.ydata))]
                            l = self.addLine(click[0])
                            self.currentPlot.listLine.append(l)
                            self.currentPlot.canvas.draw()
                        elif len(self.currentPlot.xs)==0:
                            QMessageBox.about(self, "Warning", "Please do only one click to start creating the gate.")
                            self.deleteGate()
                            raise

                        self.currentPlot.xs.sort()
                        artist1D = {}
                        #adding to list of gates for redrawing
                        artist1D[gate_name] = [deepcopy(self.currentPlot.xs[0]), deepcopy(self.currentPlot.xs[1])]
                        if (DEBUG):
                            print("artist1D for ", gate_name, "in", histo_name)
                        if self.currentPlot.toCreateGate == True:
                            if histo_name in self.currentPlot.artist_dict:
                                self.currentPlot.artist_dict[histo_name][gate_name] = artist1D[gate_name]
                            else:
                                self.currentPlot.artist_dict[histo_name] = {gate_name : artist1D[gate_name]}
                            # push gate to spectcl via PyREST
                            self.formatLinetoREST(deepcopy(self.currentPlot.xs))
                        else:
                            if (DEBUG):
                                print("Time to save the 1D summing region into a dictionary...")

                            if histo_name in self.currentPlot.region_dict:
                                self.currentPlot.region_dict[histo_name][gate_name] = artist1D[gate_name]
                            else:
                                self.currentPlot.region_dict[histo_name] = {gate_name : artist1D[gate_name]}
                            if (DEBUG):
                                print(self.currentPlot.region_dict)
                            self.formatLinetoREST(deepcopy(self.currentPlot.xs))
                            results = self.rest.integrateGate(histo_name, gate_name)
                            self.addRegion(histo_name, gate_name, results)
                            self.resPopup.show()

                    else:
                        if (DEBUG):
                            print("Closing 2D gating")
                            print(self.currentPlot.artist_dict)
                            # print(self.currentPlot.histo_name)
                            # print(self.currentPlot.artist_dict[histo_name])
                        artist2D = {}
                        self.currentPlot.xs.append(self.currentPlot.xs[-1])
                        self.currentPlot.xs.append(self.currentPlot.xs[0])
                        self.currentPlot.ys.append(self.currentPlot.ys[-1])
                        self.currentPlot.ys.append(self.currentPlot.ys[0])
                        # remove second-last point due to double click
                        self.currentPlot.xs.pop(len(self.currentPlot.xs)-2)
                        self.currentPlot.ys.pop(len(self.currentPlot.ys)-2)
                        self.currentPlot.polygon.set_data(self.currentPlot.xs, self.currentPlot.ys)
                        self.currentPlot.polygon.figure.canvas.draw()
                        #adding to list of gates for redrawing
                        artist2D[gate_name] = [deepcopy(self.currentPlot.xs), deepcopy(self.currentPlot.ys)]
                        # Simon - comment this debug print because hnameErroristo does not exist
                        if self.currentPlot.toCreateGate == True:
                            if histo_name in self.currentPlot.artist_dict:
                                self.currentPlot.artist_dict[histo_name][gate_name] = artist2D[gate_name]
                            else:
                                self.currentPlot.artist_dict[histo_name] = {gate_name : artist2D[gate_name]}
                            if (DEBUG):
                                print(self.currentPlot.artist_dict)
                                # push gate to spectcl via PyREST
                            self.formatLinetoREST(deepcopy(self.currentPlot.xs), deepcopy(self.currentPlot.ys))
                        else:
                            if (DEBUG):
                                print("Time to save the 2D summing region into a dictionary...")
                            if histo_name in self.currentPlot.region_dict:
                                self.currentPlot.region_dict[histo_name][gate_name] = artist2D[gate_name]
                            else:
                                self.currentPlot.region_dict[histo_name] = {gate_name : artist2D[gate_name]}
                            if (DEBUG):
                                print(self.currentPlot.region_dict)
                            # push gate to spectcl via PyREST
                            self.formatLinetoREST(deepcopy(self.currentPlot.xs), deepcopy(self.currentPlot.ys))
                            results = self.rest.integrateGate(histo_name, gate_name)
                            self.addRegion(histo_name, gate_name, results)
                            self.resPopup.show()
                except:
                    pass
                if (DEBUG):
                    print("Exiting gating mode...")
                    print(self.currentPlot.artist_dict)
                    print(self.currentPlot.region_dict)
                # exiting gating mode
                self.currentPlot.toCreateSRegion = False
                self.currentPlot.toCreateGate = False
            else:
                if (DEBUG):
                    print("##### Exiting zooming mode...")
                # disabling gate creation
                self.currentPlot.createSRegion.setEnabled(False)
                self.wConf.createGate.setEnabled(False)
                self.wConf.editGate.setEnabled(False)

                #important that zoomPlotInfo is set only while in zoom mode (None only here)
                #tempIdxEnlargedSpectrum is used to draw back the dashed red rectangle, which pad was enlarged
                tempIdxEnlargedSpectrum = self.getEnlargedSpectrum()[0]
                self.setEnlargedSpectrum(None, None)
                self.currentPlot.isEnlarged = False

                canvasLayout = self.wTab.layout[self.wTab.currentIndex()]
                if (DEBUG):
                    #following self.h_setup deos not work, prevent retruning to unzoomed view
                    #print("Reinitialization self.h_setup", self.h_setup)
                    print("original geometry", canvasLayout[0], canvasLayout[1])

                #draw the back the original canvas
                self.currentPlot.InitializeCanvas(canvasLayout[0], canvasLayout[1], False)
                n = canvasLayout[0]*canvasLayout[1]
                self.currentPlot.h_setup = {k: True for k in range(n)}
                self.currentPlot.selected_plot_index = None # this will allow to call drawGate and loop over all the gates
                for index in range(n):
                    self.add(index)
                    self.currentPlot.h_setup[index] = False
                    ax = self.getSpectrumInfo("axis", index=index)
                    self.plotPlot(ax, index)
                    #reset the axis limits as it was before enlarge
                    #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                    dim = self.getSpectrumInfoREST("dim", index=index)
                    if dim == 1:
                        self.setAxisScale(ax, index, "x", "y")
                    elif dim == 2:
                        self.setAxisScale(ax, index, "x", "y", "z")
                
                #drawing back the dashed red rectangle on the unenlarged spectrum
                self.removeRectangle()
                self.currentPlot.recDashed = self.createDashedRectangle(self.currentPlot.figure.axes[tempIdxEnlargedSpectrum])
                #self.updatePlot() #replaced by the content of updatePlot in the above for loop (avoid looping twice)
                self.currentPlot.figure.tight_layout()
                self.drawAllGates()
                self.currentPlot.canvas.draw()
        t=None


    #Return key if val matches a value in h_dict
    def get_key(self, val):
        return next((key for key, value in self.currentPlot.h_dict.items() if any(val == value2 for value2 in value.values())), None)

    #Simon - to show gates on all clones of a plot
    def get_keylst(self, val):
        keys = list(self.currentPlot.h_dict.keys())
        keylst = []
        for key in keys:
            if val in self.currentPlot.h_dict[key].values():
                keylst.append(key)
        return keylst


    def zoomCallback(self, event):
        self.currentPlot.zoomPress = True


    def handle_right_click(self):
        menu = QMenu()
        item1 = menu.addAction("Reset all") 
        #Empty agrument for customHomeButtonCallback means it will reset all spectra
        item1.triggered.connect(lambda: self.customHomeButtonCallback())
        # mouse_pos = QCursor.pos()
        plotgui = self.currentPlot
        menuPosX = plotgui.mapToGlobal(QtCore.QPoint(0,0)).x() + plotgui.customHomeButton.geometry().topLeft().x()
        menuPosY = plotgui.mapToGlobal(QtCore.QPoint(0,0)).y() + plotgui.customHomeButton.geometry().topLeft().y()
        menuPos = QtCore.QPoint(menuPosX, menuPosY)
        # Shows menu at button position, need to calibrate with 0,0 position
        menu.exec_(menuPos)     


    def log_handle_right_click(self):
        menu = QMenu()
        item1 = menu.addAction("Log all")
        item2 = menu.addAction("unLog all")
        #Empty agrument for logButtonCallback means it will set log for all spectra
        item1.triggered.connect(lambda: self.logButtonCallback("logAll"))
        item2.triggered.connect(lambda: self.logButtonCallback("unlogAll"))
        # mouse_pos = QCursor.pos()
        plotgui = self.currentPlot
        menuPosX = plotgui.mapToGlobal(QtCore.QPoint(0,0)).x() + plotgui.logButton.geometry().topLeft().x()
        menuPosY = plotgui.mapToGlobal(QtCore.QPoint(0,0)).y() + plotgui.logButton.geometry().topLeft().y()
        menuPos = QtCore.QPoint(menuPosX, menuPosY)
        # Shows menu at button position, need to calibrate with 0,0 position
        menu.exec_(menuPos)   


    def closeAll(self):
        self.restThreadFlag = False
        self.close()

    def doubleclickedTab(self, index):
        self.tabp.setWindowTitle("Rename tab...")
        self.tabp.setGeometry(200,350,100,50)
        if self.tabp.isVisible():
            self.tabp.close()

        self.tabp.show()

    def okTab(self):
        txt = self.wTab.tabText(self.wTab.currentIndex())
        if self.tabp.lineedit.text() != "":
            txt = self.tabp.lineedit.text()

        self.wTab.setTabText(self.wTab.currentIndex(), txt)
        self.tabp.lineedit.setText("")
        self.tabp.close()

    def cancelTab(self):
        self.tabp.close()

    
    def clickedTab(self, index):
        if (DEBUG):
            print("Clicked tab", index, "with name", self.wTab.tabText(index))

        self.wTab.setCurrentIndex(index)

        if (DEBUG):
            print("verification tab index", index)
        try:
            self.currentPlot = self.wTab.wPlot[index]
            nRow = self.wTab.layout[index][0]
            nCol = self.wTab.layout[index][1]
            if (DEBUG):
                print("self.currentPlot.h_dict_geo", self.getGeo())
                print("self.currentPlot.h_limits",self.currentPlot.h_limits)
                print("self.currentPlot.h_log",self.currentPlot.h_log)
                print("self.currentPlot.h_setup",self.currentPlot.h_setup)
                print("self.currentPlot.isLoaded", self.currentPlot.isLoaded)
                print("Canvas layout: nRow, nCol",nRow, nCol)
            #nRow-1 because nRow (nCol) is the number of row and the following sets an index starting at 0
            self.wConf.histo_geo_row.setCurrentIndex(nRow-1)
            self.wConf.histo_geo_col.setCurrentIndex(nCol-1)

            self.removeRectangle()
            self.bindDynamicSignal()
            
            self.create_gate_list()
        except:
            pass

    def setCanvasLayout(self):
        indexTab = self.wTab.currentIndex()
        nRow = int(self.wConf.histo_geo_row.currentText())
        nCol = int(self.wConf.histo_geo_col.currentText())
        self.wTab.layout[indexTab] = [nRow, nCol]
        self.wTab.wPlot[indexTab].InitializeCanvas(nRow, nCol)
        self.wTab.selected_plot_index_bak[indexTab] = None
        self.currentPlot.selected_plot_index = None
        self.currentPlot.next_plot_index = -1

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

        dim = self.getSpectrumInfoREST("dim", index=self.currentPlot.selected_plot_index)

        if dim == 1:
            self.copyAttr.histoScaleminZ.setEnabled(False)
            self.copyAttr.histoScaleValueminZ.setEnabled(False)
            self.copyAttr.histoScalemaxZ.setEnabled(False)
            self.copyAttr.histoScaleValuemaxZ.setEnabled(False)
        else:
            self.copyAttr.histoScaleminZ.setEnabled(True)
            self.copyAttr.histoScaleValueminZ.setEnabled(True)
            self.copyAttr.histoScalemaxZ.setEnabled(True)
            self.copyAttr.histoScaleValuemaxZ.setEnabled(True)

    ###############################################
    # 5) Connection to REST for gates
    ##
    #############################################

    def incrementNumbers(self, parameter, new_number):
        number = (re.findall(r'\d+',parameter))[0]
        out = parameter.replace(number,str(new_number).zfill(int(math.log10(self.nparams))+1))
        return out

    def formatLinetoREST(self, x = [], y = []):
        if (DEBUG):
            print("inside formatLinetoREST")
            print("self.currentPlot.toEditGate", self.currentPlot.toEditGate)
        name = ""
        Type = ""
        if self.currentPlot.toCreateGate or self.currentPlot.toEditGate:
            name = self.wConf.listGate.currentText()
            Type =  self.currentPlot.gateTypeDict[name]
        else:
            name = self.currentPlot.region_name
            Type =  self.currentPlot.regionTypeDict[name]

        boundaries = []
        parameters = []
        if self.wConf.button1D.isChecked():
            # gate 1Dgate_xamine s {aris.db1.ppac0.uc {1392.232056 1665.277466}}
            low = min(x)
            high = max(x)
            boundaries = [low,high]
            if (DEBUG):
                print(low, high, boundaries)
            # access list of parameters
            for index, value in self.currentPlot.h_dict.items():
                if (value["name"] ==  self.wConf.histo_list.currentText()):
                    if (Type == "s"):
                        parameters = value["parameters"][0]
                    else:
                        for i in value["parameters"]:
                            parameters.append(i)
        else:
            #{'2Dgate_xamine': {'name': '2Dgate_xamine', 'type': 'c',
            # 'parameters': ['aris.tof.tdc.db3scin_to_db5scin', 'aris.db5.pin.dE'],
            # 'points': [{'x': 126.876877, 'y': 29.429428}, {'x': 125.625626, 'y': 25.825825},
            #            {'x': 126.626625, 'y': 22.522522}, {'x': 129.879883, 'y': 22.522522}, {'x': 130.63063, 'y': 26.126125}, {'x': 129.629623, 'y': 29.129128}]
            #}}
            if (DEBUG):
                print(x,y)
            for index, value in self.currentPlot.h_dict.items():
                if (value["name"] ==  self.wConf.histo_list.currentText()):
                    parameters = value["parameters"]
            points = {}
            boundaries = []
            for xx, yy in zip(x,y):
                points["x"] = xx
                points["y"] = yy
                boundaries.append(deepcopy(points))
            boundaries.pop()

        if (DEBUG):
            print(self.wConf.listGate.currentText())
            print(Type)
            print(boundaries)
            print(parameters)

        self.rest.createGate(name, Type, parameters, boundaries)

    # this has to be modified to allow to load gate previously created and being able to apply them correctly
    def formatRESTToLine(self, name):
        if (DEBUG):
            print("inside formatRESTToLine for", name)
            print(self.currentPlot.gate_dict[name])

        # {'name': '2Dgate_xamine', 'type': 'c',
        #  'parameters': ['aris.tof.tdc.db3scin_to_db5scin', 'aris.db5.pin.dE'],
        #'points': [{'x': 126.876877, 'y': 29.429428}, {'x': 125.625626, 'y': 25.825825}, {'x': 126.626625, 'y': 22.522522}, {'x': 129.879883, 'y': 22.522522},
        #{'x': 130.63063, 'y': 26.126125}, {'x': 129.629623, 'y': 29.129128}],
        # 'spectrum': 'pid::db5.pin.dE_vs_tof.db3ntodb5n'}
        gdict = self.currentPlot.gate_dict[name]
        gate_name = gdict["name"]
        gate_type = gdict["type"]
        histo_name = gdict["spectrum"]
        x = []
        y = []
        # 1D {'name': '1Dgate_xamine', 'type': 's', 'parameters': ['aris.db1.ppac0.uc'], 'low': 1392.232056, 'high': 1665.277466}
        if gate_type == "s" or gate_type == "gs":
            gate_parameters = gdict["parameters"]
            xmin = gdict["low"]
            xmax = gdict["high"]
            self.currentPlot.artist1D[name] = [xmin, xmax]
            if (DEBUG):
                print("min/max", xmin, xmax)
                print("self.artist1D", self.currentPlot.artist1D)
            if histo_name in self.currentPlot.artist_dict:
                self.currentPlot.artist_dict[histo_name][name] = self.currentPlot.artist1D[name]
            else:
                self.currentPlot.artist_dict[histo_name] = {name: self.currentPlot.artist1D[name]}
        elif gate_type == "*":
            pass
        else:
            #{'2Dgate_xamine': {'name': '2Dgate_xamine', 'type': 'c',
            # 'parameters': ['aris.tof.tdc.db3scin_to_db5scin', 'aris.db5.pin.dE'],
            # 'points': [{'x': 126.876877, 'y': 29.429428}, {'x': 125.625626, 'y': 25.825825},
            #            {'x': 126.626625, 'y': 22.522522}, {'x': 129.879883, 'y': 22.522522}, {'x': 130.63063, 'y': 26.126125}, {'x': 129.629623, 'y': 29.129128}]
            #}}

            gate_parameters = gdict["parameters"]
            points = gdict["points"]
            if (DEBUG):
                print("points", points)
            for dic in points:
                x.append(dic["x"])
                y.append(dic["y"])
            x.append(x[0])
            y.append(y[0])
            if (DEBUG):
                print(x,y)
                for key, value in zip(x, y):
                    print(key, value)

            self.currentPlot.artist2D[name] = [x, y]
            if (DEBUG):
                print(self.currentPlot.artist2D)
            if histo_name in self.currentPlot.artist_dict:
                self.currentPlot.artist_dict[histo_name][name] = self.currentPlot.artist2D[name]
            else:
                self.currentPlot.artist_dict[histo_name] = {name: self.currentPlot.artist2D[name]}
        # adding gate to dictionary of regions
        self.currentPlot.gateTypeDict[name] = gate_type


    #Start ReST traces on a separated thread
    #Check for traces updates (poll) periodically, with period time < retention time.
    def restThread(self, retention):
        # print("Simon - trace token in startRestThread ")
        self.token = self.rest.startTraces(retention)
        self.restThreadFlag = True
        # self.updateStatusBar()
        # self.topMenu.setConnectSatus(self.restThreadFlag)
        self.wConf.connectButton.setStyleSheet("background-color:#bcee68;")
        self.wConf.connectButton.setText("Connected")

        while self.restThreadFlag:
            #Sleep time < retention to avoid loosing information
            # time.sleep(3)
            time.sleep(retention/2)
            tracesDetails = self.rest.pollTraces(self.token)
            self.updateFromTraces(tracesDetails)

        self.wConf.connectButton.setStyleSheet("background-color:rgb(252, 48, 3);")
        self.wConf.connectButton.setText("Disconnected")


    #Excecuted periodially (period defined in restThread)
    #Update various list and dict according to the trace.
    def updateFromTraces(self, tracesDetails):
        t_para = tracesDetails.get("parameter")
        t_spec = tracesDetails.get("spectrum")
        t_gate = tracesDetails.get("gate")
        t_bind = tracesDetails.get("binding")
        if len(t_bind) > 0:
            for str in t_bind:
                action, name, bindingIdx = str.split(" ") #list with 3 items: add/remove histoName bindingIndex
                if action == "remove" and name in self.getSpectrumInfoRESTDict():
                    self.removeSpectrum(name=name)
                    self.update_spectrum_list()
                elif action == "add" and name not in self.getSpectrumInfoRESTDict():
                    #get spectrum info from ReST but not the data, need to query shmem for that (connect button).
                    #dont need to check the binding, given the definition of this trace...
                    info = self.rest.listSpectrum(name)
                    # print("Simon spectrum info in updateFromTrace ",info)
                    #info[0] because expect only one element in the list with the name filter

                    #get the data in shmem ... maybe not the best to do it here
                    #server info are already checked at this point
                    hostname = self.connectConfig.server.text()
                    port = self.connectConfig.rest.text()
                    user = self.connectConfig.user.text()
                    mirror = self.connectConfig.mirror.text()
                    s = cpy.CPyConverter().Update(bytes(hostname, encoding='utf-8'), bytes(port, encoding='utf-8'), bytes(mirror, encoding='utf-8'), bytes(user, encoding='utf-8'))
                    data = []
                    binx = info[0]["axes"][0]["bins"]
                    minx = info[0]["axes"][0]["low"]
                    maxx = info[0]["axes"][0]["high"]
                    if "1" in info[0]["type"] or "b" in info[0]["type"] or "g1" in info[0]["type"] or "s" in info[0]["type"] :
                        dim = 1
                        biny = miny = maxy = None
                        nameIndex = s[1].index(name)
                        try :
                            data = s[9][nameIndex][0:-1]
                            data[0] = 0
                        except:
                            print("updateFromTraces - nameIndex not in shmem np array for : ", name)
                        self.setSpectrumInfoREST(name, dim=dim, binx=binx, minx=minx, maxx=maxx, biny=biny, miny=miny, maxy=maxy, parameters=info[0]["parameters"], type=info[0]["type"], data=data)
                    else :
                        dim = 2
                        biny = info[0]["axes"][1]["bins"]
                        miny = info[0]["axes"][1]["low"]
                        maxy = info[0]["axes"][1]["high"]
                        nameIndex = s[1].index(name)
                        try :
                            data = s[9][nameIndex][1:-1, 1:-1]
                        except:
                            print("updateFromTraces - nameIndex not in shmem np array for : ", name)
                        self.setSpectrumInfoREST(name, dim=dim, binx=binx, minx=minx, maxx=maxx, biny=biny, miny=miny, maxy=maxy, parameters=info[0]["parameters"], type=info[0]["type"], data=data)
                    self.update_spectrum_list()
                else:
                    return 
        # print("Simon - timer1 updateFromTraces",timer1.elapsed())


    #Set spectrum info from ReST in spectrum_dict_rest (identified by histo name and can update multiple info at once)
    #self.spectrum_dict_rest is used to keep track of the treegui definition (fixed) 
    def setSpectrumInfoREST(self, name, **info):
        for key, value in info.items():
            if key in ("dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type"):
                if name not in self.spectrum_dict_rest:
                    self.spectrum_dict_rest[name] = {"dim":[],"binx":[],"minx":[],"maxx":[],"biny":[],"miny":[],"maxy":[],"data":[],"parameters":[],"type":[]}
                # print("Simon - setSpectrumInfoREST - name key value",name,key,value)
                self.spectrum_dict_rest[name][key] = value


    #Get spectrum info from spectrum_dict_rest (identified by histo name or index and info name)
    #template of expected arguments e.g.: ("dim", index=5) takes only the first info parameter (here "dim") (one per call)
    #Important that it gets only the info from self.spectrum_dict_rest here.
    def getSpectrumInfoREST(self, *info, **identifier):
        name = None
        if not identifier and self.getEnlargedSpectrum():
            name = self.getEnlargedSpectrum()[1]
        elif "index" in identifier:
            name = self.nameFromIndex(identifier["index"])
        elif "name" in identifier:
            name = identifier["name"]
        else:
            print("getSpectrumInfo - wrong identifier - expects name=histo_name or index=histo_index or shoud be in zoomed mode")
            return
        if name is not None and name in self.spectrum_dict_rest and info[0] in ("dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type"):
            # print("Simon - getSpectrumInfoREST - ",name,info[0],self.spectrum_dict_rest[name][info[0]])
            return self.spectrum_dict_rest[name][info[0]]
        else:
            return


    #Update spectrum info in spectrum_dict (identified by index and can update multiple info at once)
    #Important that only self.wTab.spectrum_dict is changed here
    #work in normal and enlarged mode
    def setSpectrumInfo(self, **info):
        name = None
        index = None
        if self.getEnlargedSpectrum():
            index = self.getEnlargedSpectrum()[0]
            name = self.getEnlargedSpectrum()[1]
        elif "index" in info:
            index = info["index"]
            name = self.nameFromIndex(info["index"])
        # commented the following option because can have several versions of the same plot in a window (name not a unique id)
        # elif "name" in info:
        #     name = info["name"]
        else:
            print("setSpectrumInfo - wrong identifier - expects index=histo_index or shoud be in zoomed mode")
            return
        # print("Simon - setSpectrumInfo - ", index,name,info["index"])
        for key, value in info.items():
            if key in ("name", "dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type", "log", "minz", "maxz", "spectrum", "axis") and index is not None:
                if index not in self.wTab.spectrum_dict[self.wTab.currentIndex()]:
                    print("setSpectrumInfo -",name,"not in spectrum_dict")
                    return
                    # self.wTab.spectrum_dict[self.wTab.currentIndex()][name] = {"dim":[],"binx":[],"minx":[],"maxx":[],"biny":[],"miny":[],"maxy":[],"data":[],"parameters":[],"type":[],"log":[],"minz":[],"maxz":[]}
                self.wTab.spectrum_dict[self.wTab.currentIndex()][index][key] = value
                #set axes info at the same time than spectrum
                if key == "spectrum":
                    self.wTab.spectrum_dict[self.wTab.currentIndex()][index]["axis"] = value.axes


    #Get spectrum info from spectrum_dict (identified by index and info name)
    #template of expected arguments e.g.: ("dim", index=5) takes only the first info parameter (here "dim") (one per call)
    #Important that it gets only the info from self.wTab.spectrum_dict[self.wTab.currentIndex()] here.
    #work in normal and enlarged mode
    def getSpectrumInfo(self, *info, **identifier):
        name = None
        index = None
        if self.getEnlargedSpectrum():
            index = self.getEnlargedSpectrum()[0]
            # name = self.getEnlargedSpectrum()[1]
        elif "index" in identifier:
            index = identifier["index"]
            # name = self.nameFromIndex(identifier["index"])
        # commented the following option because can have several versions of the same plot in a window (name not a unique id)
        # elif "name" in identifier:
        #     name = identifier["name"]
        else:
            print("getSpectrumInfo - wrong identifier - expects index=histo_index or shoud be in zoomed mode")
            return
        if index is not None and index in self.wTab.spectrum_dict[self.wTab.currentIndex()] and info[0] in ("name", "dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type", "log", "minz", "maxz", "spectrum", "axis"):
        # if index is not None and info[0] in ("name", "dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type", "log", "minz", "maxz"):
            # print("Simon - in getSpectrumInfo - ",self.wTab.currentIndex(), index, info[0])
            return self.wTab.spectrum_dict[self.wTab.currentIndex()][index][info[0]]


    #Remove spectrum from self.wTab.spectrum_dict:
    #important: only functions that delete item in spectrum_dict and spectrum_dict_rest
    #important: should not be triggered by user, for now used only in updateFromTraces, because of the way it deletes local spectrumInfo entries.
    def removeSpectrum(self, **identifier):
        name = None
        if "index" in identifier:
            name = self.nameFromIndex(identifier["index"])
        elif "name" in identifier:
            name = identifier["name"]
        else:
            print("removeSpectrum - wrong identifier - expects name=histo_name or index=histo_index")
            return
        # in spectrumInfoReST dict can only have unique spectrum
        del self.spectrum_dict_rest[name]
        # to_delete = [key for key, value in self.spectrum_dict.items() if name in value]
        # in local spectrumInfo dict can have multiple spectra with the same name
        to_delete = self.indexFromName(name)
        for key in to_delete:
            del self.wTab.spectrum_dict[self.wTab.currentIndex()][key]


    #get full spectrum dict self.wTab.spectrum_dict:
    def getSpectrumInfoDict(self):
        return self.wTab.spectrum_dict[self.wTab.currentIndex()]


    #get full spectrum dict self.spectrum_dict_rest:
    def getSpectrumInfoRESTDict(self):
        return self.spectrum_dict_rest
 

    #Find name with geo index:
    def nameFromIndex(self, index):
        #Can call getSpectrumInfo and setSpectrumInfo with an identifier but still check if in zoom mode,
        #which is important for autoScaleAxis/setAxisScale
        if self.getEnlargedSpectrum():
            return self.getEnlargedSpectrum()[1]
        elif index in self.currentPlot.h_dict_geo:
            return self.currentPlot.h_dict_geo[index]


    #Find index(es) in geo for spectrum name (returns a list)
    def indexFromName(self, name):
        result = []
        #Have to be careful that zoomPlotInfo is well sets all the time.
        #Should have a value _only_while_ in enlarged mode
        if self.getEnlargedSpectrum():
            result = [0]
        else:
            result = [key for key, value in self.currentPlot.h_dict_geo.items() if name in value]
        return result


    #sets h_dict_geo {key=index, value=histoName}
    #Use only this function to set the geometry dict when add plot (against using it elsewhere because it initializes spectrum_dict)
    def setGeo(self, index, name):
        self.currentPlot.h_dict_geo[index] = name
        #Set also here the spectrum_dict with only the spectra defined in the geo
        if index not in self.wTab.spectrum_dict[self.wTab.currentIndex()]:
            self.wTab.spectrum_dict[self.wTab.currentIndex()][index] = {"name":[], "dim":[],"binx":[],"minx":[],"maxx":[],"biny":[],"miny":[],"maxy":[],"data":[],"parameters":[],"type":[],"log":[],"minz":[],"maxz":[], "spectrum":[], "axis":[]}
        self.wTab.spectrum_dict[self.wTab.currentIndex()][index]["name"] = name
        #Initialize with the same info than in spectrum_dict_rest.
        for key, value in self.spectrum_dict_rest[name].items():
            self.wTab.spectrum_dict[self.wTab.currentIndex()][index][key] = value


    #returns h_dict_geo {key=index, value=histoName}
    #Use only this function to get the geometry dict, to know the name at index there is nameFromIndex
    def getGeo(self):
        return self.currentPlot.h_dict_geo


    #Check if there is a plot at a given index
    def emptySpectrumFrame(self, index):
        if index in self.currentPlot.h_dict_geo and self.currentPlot.h_dict_geo[index]:
            return False
        else:
            return True


    def saveCanvasBkg(self, axis, spectrum, index):
        self.currentPlot.axbkg[index] = self.currentPlot.figure.canvas.copy_from_bbox(axis.bbox)
        # axis.draw_artist(spectrum)
        # self.currentPlot.figure.canvas.blit(axis.bbox)


    def restoreCanvasBkg(self, axis, spectrum, index):
        axis.clear()
        self.currentPlot.figure.canvas.restore_region(self.currentPlot.axbkg[index])
        axis.draw_artist(spectrum)
        self.currentPlot.figure.canvas.blit(axis.bbox)
        self.currentPlot.figure.canvas.flush_events()

    def setEnlargedSpectrum(self, index, name):
        self.wTab.zoomPlotInfo[self.wTab.currentIndex()] = None 
        if index is not None and name is not None: 
            self.wTab.zoomPlotInfo[self.wTab.currentIndex()] = [index, name]


    def getEnlargedSpectrum(self):
        result = None
        if self.wTab.zoomPlotInfo[self.wTab.currentIndex()] :
            result = self.wTab.zoomPlotInfo[self.wTab.currentIndex()]
        return result

    ##########################################
    # 6) Accessing the ShMem
    ##########################################

    def connectShMem(self):
        # trying to access the shared memory through SpecTcl Mirror Client
        try:
            # update host name and port, mirror port, and user name from GUI
            hostname = str(self.connectConfig.server.text())
            port = str(self.connectConfig.rest.text())
            user = str(self.connectConfig.user.text())
            mirror = str(self.connectConfig.mirror.text())

            # configuration of the REST plugin
            self.rest = PyREST(hostname,port)
            # set traces
            if (DEBUG):
                print("trace token", self.token)
                print(self.rest.pollTraces(self.token))
            threadRest = threading.Thread(target=self.restThread, args=(6,))
            threadRest.start()

            if (hostname == "hostname" or port == "port" or mirror == "mirror"):
                raise ValueError("hostname/port/mirror are not configured!")
            timer1 = QElapsedTimer()
            timer1.start()
            if (DEBUG):
                print(hostname.encode('utf-8'), port.encode('utf-8'), user.encode('utf-8'), mirror.encode('utf-8'))
                print("before cpy.CPyConverter().Update")
            s = cpy.CPyConverter().Update(bytes(hostname, encoding='utf-8'), bytes(port, encoding='utf-8'), bytes(mirror, encoding='utf-8'), bytes(user, encoding='utf-8'))

            # creates a dataframe for spectrum info
            # use the spectrum name to merge both sources (REST and shared memory) of spectrum info
            # info = {"id":[],"names":[],"dim":[],"binx":[],"minx":[],"maxx":[],"biny":[],"miny":[],"maxy":[],"data":[],"parameters":[],"type":[]}

            otherInfo = self.getSpectrumInfoFromReST()
            for i, name in enumerate(s[1]):
                if name in otherInfo:
                    if s[2][i] == 2:
                        data = s[9][i][ 1:-1, 1:-1]
                    else:
                        data = s[9][i][0:-1]
                        data[0] = 0
                    # print("Simon connectShMem ", s[3][i], s[4][i], s[5][i], s[6][i], s[7][i], s[8][i],otherInfo[name]["type"])
                    self.setSpectrumInfoREST(name, dim=s[2][i], binx=s[3][i]-2, minx=s[4][i], maxx=s[5][i], biny=s[6][i]-2, miny=s[7][i], maxy=s[8][i], data=data, parameters=otherInfo[name]["parameters"], type=otherInfo[name]["type"])

            # update and create parameter, spectrum, and gate lists
            self.update_spectrum_list(True)
            self.create_gate_list()
            self.updateGateType()

            '''
            # update Modify menu
            gate_list = [self.wConf.listGate.itemText(i) for i in range(self.wConf.listGate.count())]
            print("gate_list", gate_list)
            for gate in gate_list:
                self.wConf.submenuD.addAction(gate, lambda:self.wConf.drag(gate))
                self.wConf.submenuE.addAction(gate, lambda:self.wConf.edit(gate))
            '''
        except NameError:
            raise
        # except:
        #     QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started or hostname/port/mirror are not configured!")


    #get histo name, type and parameters from REST
    def getSpectrumInfoFromReST(self):
        if (DEBUG):
            print("Inside getSpectrumInfoFromReST")
        outDict = {}
        inpDict = self.rest.listSpectrum()
        bindList = self.rest.listsbind("*")
        # Dictionary of bindings, keyed by spectrum name
        bindings = {}
        for d in bindList:
            bindings[d["name"]] = d["binding"]
        for el in inpDict:
            if el["name"] in bindings:
                outDict[el["name"]] = {
                    "parameters": el["parameters"],
                    "type": el["type"],
                    "binding": bindings[el["name"]],
                }
            else:
                pass
        return outDict


    # update spectrum list for GUI, only the widget histo_list is set here
    def update_spectrum_list(self, init=False):
        if (DEBUG):
            print("Inside create_spectrum_list")
        self.wConf.histo_list.clear()
        #Sort because otherwise when ReST update the modified spectrum is append at the end of the list
        for name in sorted(self.getSpectrumInfoRESTDict()):
            if self.wConf.histo_list.findText(name) == -1:
                self.wConf.histo_list.addItem(name)

        if init:
            self.wConf.histo_list.setEditable(True)
            self.wConf.histo_list.setInsertPolicy(QComboBox.NoInsert)
            self.wConf.histo_list.completer().setCompletionMode(QCompleter.PopupCompletion)
            self.wConf.histo_list.completer().setFilterMode(QtCore.Qt.MatchContains)


    # update spectrum information
    # fill currentPlot.h_dict, if h_dict obselete this function is also
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
                # Simon - parameter
                # index = self.wConf.listParams[i].findText(df.iloc[0]['parameters'][i], QtCore.Qt.MatchFixedString)
                # if index >= 0:
                #     self.wConf.listParams[i].setCurrentIndex(index)
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
        print("Simon - create_disable2D empty")
        # Simon - parameter
        # if value==True:
        #     self.wConf.listParams[1].setEnabled(False)
        # else:
        #     self.wConf.listParams[1].setEnabled(True)

    # check histogram dimension from GUI
    def check_histogram(self):
        print("Simon - check_histogram empty")
        # Simon - parameter
        # if self.wConf.button1D.isChecked():
        #     self.create_disable2D(True)
        # else:
        #     self.create_disable2D(False)

    # update spectrum information
    # given that we will likely remove button1D and button2D this function is obselete
    def updateHistoInfo(self, hist_name):
        print("Simon - updateHistoInfo empty")
        # if (DEBUG):
        #     print("Inside updateHistoInfo")
        #     print("hist_name",hist_name)
        # try:
        #     select = self.spectrum_list['names'] == hist_name
        #     df = self.spectrum_list.loc[select]
        #     hist_dim = df.iloc[0]['dim']
        #     if hist_dim == 1:
        #         self.wConf.button1D.setChecked(True)
        #     else:
        #         self.wConf.button2D.setChecked(True)
        #     self.check_histogram();
            # Simon - parameter
            # for i in range(hist_dim):
            #     #Simon - moved the print in the for loop because df.iloc[0]['parameters'] size depends on hist_dim
            #     if (DEBUG):
            #         print(hist_name, hist_dim, df.iloc[0]['parameters'][i])
            #     index = self.wConf.listParams[i].findText(df.iloc[0]['parameters'][i], QtCore.Qt.MatchFixedString)
            #     if index >= 0:
            #         self.wConf.listParams[i].setCurrentIndex(index)
        # except:
        #     pass

    # update and create gate list
    def create_gate_list(self):
        if (DEBUG):
            print("Inside create_gate_list")
        try:
            gate_list = self.rest.listGate()
            # resetting ComboBox
            self.wConf.listGate.clear()
            if (DEBUG):
                print("gate list", gate_list)
            self.currentPlot.gate_dict = {d["name"]: d for d in gate_list}
            if (DEBUG):
                print("self.currentPlot.gate_dict", self.currentPlot.gate_dict)
            # extract keys from gate_dict
            gates = list(self.currentPlot.gate_dict.keys())
            if (DEBUG):
                print("gates",gates)
            apply_gatelist = self.rest.applylistgate()
            if (DEBUG):
                print("apply_gatelist",apply_gatelist)
            for i in apply_gatelist:
                if i["gate"] in gates:
                    (self.currentPlot.gate_dict[i["gate"]])["spectrum"] = i["spectrum"]
                    if self.wConf.listGate.findText(i["gate"]) == -1:
                        self.wConf.listGate.addItem(i["gate"])
                    self.formatRESTToLine(i["gate"])
        except NameError:
            raise


    def updateGateType(self):
        try:
            gate_name = self.wConf.listGate.currentText()
            gate_type = self.currentPlot.gateTypeDict[gate_name]
            self.wConf.listGate_type_label.setText(gate_type)
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
            geo = self.getGeo()
            for index in range(len(geo)):
                h_name = geo[index]
                x_range, y_range = self.getAxisProperties(index)
                scale = True if self.getSpectrumInfo("log", index=index) else False
                properties[index] = {"name": h_name, "x": x_range, "y": y_range, "scale": scale}

            tmp = {"row": int(self.wConf.histo_geo_row.currentText()), "col": int(self.wConf.histo_geo_col.currentText()), "geo": properties}
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
            row = infoGeo["row"]
            col = infoGeo["col"]
            # change index in combobox to the actual loaded values
            index_row = self.wConf.histo_geo_row.findText(str(row), QtCore.Qt.MatchFixedString)
            index_col = self.wConf.histo_geo_col.findText(str(col), QtCore.Qt.MatchFixedString)
            notFound = []
            if index_row >= 0 and index_col >= 0:
                self.wConf.histo_geo_row.setCurrentIndex(index_row)
                self.wConf.histo_geo_col.setCurrentIndex(index_col)
                self.setCanvasLayout()
                # self.currentPlot.InitializeCanvas(infoGeo["row"],infoGeo["col"])
                for index, val_dict in infoGeo["geo"].items():
                    if (DEBUG):
                        print("---->",index, val_dict)

                    if self.getSpectrumInfoREST("dim", name=val_dict["name"]) is None:
                        notFound.append(val_dict["name"])
                        continue 

                    self.setGeo(index, val_dict["name"])
                    self.setSpectrumInfo(log=val_dict["scale"], index=index)
                    self.setSpectrumInfo(minx=val_dict["x"][0], index=index)
                    self.setSpectrumInfo(maxx=val_dict["x"][1], index=index)
                    self.setSpectrumInfo(miny=val_dict["y"][0], index=index)
                    self.setSpectrumInfo(maxy=val_dict["y"][1], index=index)

                if len(notFound) > 0:
                    print("LoadGeo - definition not found for: ",notFound)

                self.currentPlot.isLoaded = True
                self.wTab.selected_plot_index_bak[self.wTab.currentIndex()] = None
                self.currentPlot.selected_plot_index = None
                self.currentPlot.next_plot_index = -1
                

            if (DEBUG):
                print("After loading geo win")
                print("self.currentPlot.h_dict_geo", self.getGeo())
                print("self.currentPlot.h_limits",self.currentPlot.h_limits)
                print("self.currentPlot.h_log",self.currentPlot.h_log)
                print("self.currentPlot.h_setup",self.currentPlot.h_setup)
                print("self.currentPlot.isLoaded", self.currentPlot.isLoaded)

            self.addPlot()
            self.updatePlot()
            self.currentPlot.isLoaded = False
            #Simon - commented following line because drawAllGates is called in updatePlot
            #self.drawAllGates()
            # self.updateGateType()

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

    # sets y scales for 1d and y,z scales for 2d depending on the scale identifier and if axisIsLog
    #Do all the scaling operations
    def setAxisScale(self, ax, index, *scale):
        if not scale:
            print("setAxisScale - scale name not specified - should call setAxisScale with scale='x' or/and 'y' or/and 'z'")

        wPlot = self.currentPlot
        axisIsLog = self.getSpectrumInfo("log", index=index)
        axisIsAutoScale = wPlot.histo_autoscale.isChecked()

        # priority to autoscale value, then if not to user defined value (ex: by zoom), and finally to default value
        # log is set last
        # update spectrumInfo if autoscale and/or log if value <=0

        if (self.getSpectrumInfoREST("dim", index=index) == 1) :
            #x limits need to be known for y autoscale in x range
            xmin = self.getSpectrumInfo("minx", index=index)
            xmax = self.getSpectrumInfo("maxx", index=index)
            if "x" in scale and xmin and xmax:
                ax.set_xlim(xmin,xmax) 
            if "y" or "log" in scale:
                ymin = self.getSpectrumInfo("miny", index=index)
                ymax = self.getSpectrumInfo("maxy", index=index)
                if (not ymin or ymin is None or ymin == 0) and (not ymax or ymax is None or ymax == 0):
                    ymin = self.minY
                    ymax = self.maxY
                if axisIsAutoScale:
                    #search in the current view
                    xmin, xmax = ax.get_xlim()
                    ymax = self.getMaxInRange(index, xmin=xmin, xmax=xmax)
                if axisIsLog:
                    if ymin <= 0:
                        ymin = 0.001
                    ax.set_ylim(ymin,ymax)
                    ax.set_yscale("log")
                else:
                    ax.set_ylim(ymin,ymax)
                    ax.set_yscale("linear")
                self.setSpectrumInfo(miny=ymin, index=index)
                self.setSpectrumInfo(maxy=ymax, index=index)
                ax_chk = self.getSpectrumInfo("axis", index=index)

        else:
            #x and y limits need to be known for z autoscale in x,y ranges
            xmin = self.getSpectrumInfo("minx", index=index)
            xmax = self.getSpectrumInfo("maxx", index=index)
            ymin = self.getSpectrumInfo("miny", index=index)
            ymax = self.getSpectrumInfo("maxy", index=index)

            if "x" in scale and xmin and xmax:
                ax.set_xlim(xmin,xmax) 
            if "y" in scale and ymin and ymax:
                ax.set_ylim(ymin,ymax)
            if "z" or "log" in scale:
                zmin = self.getSpectrumInfo("minz", index=index)
                zmax = self.getSpectrumInfo("maxz", index=index)
                spectrum = self.getSpectrumInfo("spectrum", index=index)
                if (not zmin or zmin is None or zmin == 0) and (not zmax or zmax is None or zmax == 0):
                    zmin = self.minZ
                    zmax = self.maxZ
                if axisIsAutoScale:
                    #search in the current view
                    xmin, xmax = ax.get_xlim()
                    ymin, ymax = ax.get_ylim()
                    zmax = self.getMaxInRange(index, xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax)
                    self.setSpectrumInfo(maxz=zmax, index=index)
                    self.setSpectrumInfo(minz=zmin, index=index)
                if axisIsLog:
                    if zmin and zmin <= 0 :
                        zmin = 0.001
                    zmin = math.log10(zmin)
                    zmax = math.log10(zmax)
                spectrum.set_clim(vmin=zmin, vmax=zmax)
                self.setSpectrumInfo(spectrum=spectrum, index=index)
                #Dont want to save z limits in spectrum info because in log: z_new = f(z_old)


    def zoom(self, ax, index, flag):
        if (DEBUG):
            print("Inside zoom")
        spectrum = self.getSpectrumInfo("spectrum", index=index)
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
        else :
            zmax = ax.get_clim()[1]
            if (DEBUG):
                print("zmax",zmax)
            if flag == "in":
                zmax /= 2
            elif flag == "out":
                zmax *= 2
            if (DEBUG):
                print("new zmax", zmax)
            spectrum.set_clim(vmax=zmax)
            self.setSpectrumInfo(spectrum=spectrum, index=index)

    def zoomInOut(self, arg):
        if (DEBUG):
            print("Inside zoomInOut",arg)
        # Simon - added following lines to avoid None plot index
        index = self.autoIndex()
        ax = None
        spectrum = self.getSpectrumInfo("spectrum", index=index)
        if spectrum is None : return
        ax = spectrum.axes
        dim = self.getSpectrumInfoREST("dim", index=index)
        if dim == 1 :
            #step if 0.5
            ymin, ymax = ax.get_ylim()
            if arg is "in" :
                ymax = ymax*0.5
            elif arg is "out" :
                ymax = ymax*2
            ax.set_ylim(ymin, ymax)
            self.setSpectrumInfo(miny=ymin, index=index)
            self.setSpectrumInfo(maxy=ymax, index=index)
            self.setSpectrumInfo(spectrum=spectrum, index=index)
        elif dim == 2 :
            zmin, zmax = spectrum.get_clim()
            if arg is "in" :
                zmax = zmax*0.5
            elif arg is "out" :
                zmax = zmax*2
            spectrum.set_clim(zmin, zmax)
            self.setSpectrumInfo(minz=zmin, index=index)
            self.setSpectrumInfo(maxz=zmax, index=index)
            self.setSpectrumInfo(spectrum=spectrum, index=index)
        self.currentPlot.canvas.draw()


    # set axis limits, log and autoscale
    def autoScaleAxisBox(self):
        if (DEBUG):
            print("autoScaleAxisBox in tab:", self.wTab.currentIndex(), "with name", self.wTab.tabText(self.wTab.currentIndex()))
        wPlot = self.currentPlot
        try:
            ax = None
            if self.currentPlot.isEnlarged:
                if (DEBUG):
                    print("inside isEnlarged")
                ax = self.getSpectrumInfo("axis", index=0)
                dim = self.getSpectrumInfoREST("dim", index=0)

                #Set y for 1D and z for 2D.
                #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                if dim == 1:
                    self.setAxisScale(ax, 0, "y")
                elif dim == 2:
                    self.setAxisScale(ax, 0, "z")
            else:
                for index, name in self.getGeo().items():
                    if name:
                        ax = self.select_plot(index)
                        dim = self.getSpectrumInfoREST("dim", index=index)

                        #Set y for 1D and z for 2D.
                        #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                        if dim == 1:
                            self.setAxisScale(ax, index, "y")
                        elif dim == 2:
                            self.setAxisScale(ax, index, "z")
            self.currentPlot.canvas.draw()
        except:
            pass


    # get data max within user defined range
    # For 2D have to give two ranges (x,y), for 1D range x.
    def getMaxInRange(self, index, **limits):
        result = None
        if not limits :
            print("getMaxInRange - limits identifier not valid - expect xmin=val, xmax=val etc. for y with 2D")
            return
        if "xmin" and "xmax" in limits:
            xmin = limits["xmin"]
            xmax = limits["xmax"]
        if "ymin" and "ymax" in limits:
            ymin = limits["ymin"]
            ymax = limits["ymax"]

        dim = self.getSpectrumInfoREST("dim", index=index)
        minx = self.getSpectrumInfoREST("minx", index=index)
        maxx = self.getSpectrumInfoREST("maxx", index=index)
        binx = self.getSpectrumInfoREST("binx", index=index)
        data = self.getSpectrumInfoREST("data", index=index)
        stepx = (float(maxx)-float(minx))/float(binx)
        binminx = int((xmin-minx)/stepx)
        binmaxx = int((xmax-minx)/stepx)
        if dim == 1:
            # get max in x range
            #increase by 10% to get axis view a little bigger than max
            result = data[binminx+1:binmaxx+2].max()*1.1
        elif dim == 2:
            #get max in x,y ranges
            miny = self.getSpectrumInfoREST("miny", index=index)
            maxy = self.getSpectrumInfoREST("maxy", index=index)
            biny = self.getSpectrumInfoREST("biny", index=index)
            stepy = (float(maxy)-float(miny))/float(biny)
            binminy = int((ymin-miny)/stepy)
            binmaxy = int((ymax-miny)/stepy)
            #Dont increase by 10% here...
            result = data[binminy:binmaxy+1, binminx:binmaxx+1].max()
        return result


    def getAxisProperties(self, index):
        if (DEBUG):
            print("Inside getAxisProperties")
        try:
            ax = None
            ax = self.getSpectrumInfo("axis", index=index)
            return list(ax.get_xlim()), list(ax.get_ylim())
        except:
            pass
            

    #Used by customHome button, reset the axis limits to ReST definitions, for the specified plot at index or for all plots if index not provided
    def customHomeButtonCallback(self, index=None):
        if (DEBUG):
            print("Inside customHomeButtonCallback", index)

        index_list = [idx for idx, name in self.getGeo().items() if index is None]
        if index is not None:
            index_list = [index]
        for idx in index_list:
            ax = None
            # spectrum = self.getSpectrum(idx)
            spectrum = self.getSpectrumInfo("spectrum", index=idx)
            if spectrum is None : return
            ax = spectrum.axes
            dim = self.getSpectrumInfoREST("dim", index=idx)
            xmin = self.getSpectrumInfoREST("minx", index=idx)
            xmax = self.getSpectrumInfoREST("maxx", index=idx)
            ymin = self.getSpectrumInfoREST("miny", index=idx)
            ymax = self.getSpectrumInfoREST("maxy", index=idx)

            ax.set_xlim(xmin, xmax)
            if dim == 1:
                #Similar to autoscale, in principle ymin and ymax are not defined in ReST for 1D so set to ymin=0 and autoscale for ymax
                ymax = self.getMaxInRange(idx, xmin=xmin, xmax=xmax)
                ax.set_ylim(ymin, ymax)
                if self.getSpectrumInfo("log", index=idx) :
                    ax.set_yscale("linear")
            # y limits should be known at this point for both cases 1D/2D
            if dim == 2:
                ax.set_ylim(ymin, ymax)  
                zmax = self.getMaxInRange(idx, xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax)
                spectrum.set_clim(vmin=self.minZ, vmax=zmax)
                self.setSpectrumInfo(maxz=zmax, index=idx)
                self.setSpectrumInfo(minz=self.minZ, index=idx)
                
            self.setSpectrumInfo(log=None, index=idx)
            self.setSpectrumInfo(minx=xmin, index=idx)
            self.setSpectrumInfo(maxx=xmax, index=idx)
            self.setSpectrumInfo(miny=ymin, index=idx)
            self.setSpectrumInfo(maxy=ymax, index=idx)
            self.setSpectrumInfo(spectrum=spectrum, index=idx)
        self.currentPlot.canvas.draw()
        #These flags may be obselete?
        self.currentPlot.isZoomCallback = False
        # self.currentPlot.isZoomInOut = False

    #Used by logButton, define the log scale, for the specified plot at index or for all plots if logAll/unlogAll
    def logButtonCallback(self, *arg):
        if (DEBUG):
            print("Inside logButtonCallback - arg -", arg)

        index = None
        logAllPlot = False
        unlogAllPlot = False
        if "logAll" in arg:
            logAllPlot = True
        elif "unlogAll" in arg:
            unlogAllPlot = True
        else :
            index = arg[0]

        wPlot = self.currentPlot
        logBut = wPlot.logButton

        index_list = [idx for idx, name in self.getGeo().items() if logAllPlot or unlogAllPlot]

        if index is not None:
            index_list = [index]
        for idx in index_list:
            ax = None
            # spectrum = self.getSpectrum(idx)
            spectrum = self.getSpectrumInfo("spectrum", index=idx)
            if spectrum is None : continue 
            ax = spectrum.axes
            # only place where the log spectrum info is set 
            # so if log now it needs to switch to linear, and vice et versa
            if logAllPlot :
                self.setSpectrumInfo(index=idx, log=True)
            elif unlogAllPlot :
                self.setSpectrumInfo(index=idx, log=False)
            elif self.getSpectrumInfo("log", index=idx) and not logAllPlot and not unlogAllPlot:
                self.setSpectrumInfo(index=idx, log=False)
            elif not self.getSpectrumInfo("log", index=idx) and not logAllPlot and not unlogAllPlot:
                self.setSpectrumInfo(index=idx, log=True)

            self.setAxisScale(ax, idx, "log")
        wPlot.canvas.draw()


    #Simon - used to keep modified axis ranges after zoomCallback unless homeCallback is pressed
    # now only used in applyCopy
    def setAxisLimits(self, index):
        if (DEBUG):
            print("Inside setAxisLimits")
        ax = None
        ax = self.getSpectrumInfo("axis", index=index)
        if "x" in self.currentPlot.h_limits[index] and "y" in self.currentPlot.h_limits[index]:
            x_limits = self.currentPlot.h_limits[index]["x"]
            y_limits = self.currentPlot.h_limits[index]["y"]
            if x_limits and y_limits:
                ax.set_xlim(float(x_limits[0]), float(x_limits[1]))
                ax.set_ylim(float(y_limits[0]), float(y_limits[1]))
        if self.currentPlot.h_log[index] and self.currentPlot.h_dim[index] == 1:
            self.setAxisScale(ax, index)
        if self.currentPlot.h_dim[index] == 2:
            self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)
            if self.currentPlot.h_log[index]:
                self.setAxisScale(ax, index)


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
        keys=list(self.currentPlot.h_dict.keys())
        values = []
        try:
            values = [value["name"] for value in self.currentPlot.h_dict.values()]
        except TypeError as err:
            print(err)
            return
        if "empty" in values:
            self.currentPlot.index = keys[values.index("empty")]
        else:
            if (DEBUG):
                print("list is full, set index to full")
            self.currentPlot.index = keys[-1]
            self.currentPlot.isFull = True
        return self.currentPlot.index


    # select axes based on indexing
    def select_plot(self, index):
        for i, axis in enumerate(self.currentPlot.figure.axes):
            # retrieve the subplot from the click
            if (i == index):
                return axis


    # returns position in grid based on indexing
    def plot_position(self, index):
        cntr = 0
        # convert index to position in geometry
        canvasLayout = self.wTab.layout[self.wTab.currentIndex()]
        for i in range(canvasLayout[0]):
            for j in range(canvasLayout[1]):
                if index == cntr:
                    return i, j
                else:
                    cntr += 1


    # erase plot
    def erasePlot(self, index):
        if (DEBUG):
            print("Inside erasePlot")
        a = None
        a = self.getSpectrumInfo("axis", index=index)
        # if 2d histo I need a bit more efforts for the colorbar
        try:
            print("Inside try removeCb ")
            self.removeCb(a)
        except:
            pass
        a.clear()
        return a
    

    # setup histogram limits according to the ReST info
    # called in "add", when the plot is first added
    def setupPlot(self, axis, index):
        if (DEBUG):
            print("Inside setupPlot")
            print("Geo",self.getGeo())

        if self.nameFromIndex(index):
            if len(self.currentPlot.hist_list) > index :
                self.currentPlot.hist_list.pop(index)

            dim = self.getSpectrumInfoREST("dim", index=index)
            minx = self.getSpectrumInfoREST("minx", index=index)
            maxx = self.getSpectrumInfoREST("maxx", index=index)
            binx = self.getSpectrumInfoREST("binx", index=index)

            w = self.getSpectrumInfoREST("data", index=index)
            # update axis
            if dim == 1:
                if (DEBUG):
                    print("1d case...")
                axis.set_xlim(minx,maxx)
                axis.set_ylim(self.minY,self.maxY)
                # create histogram
                line, = axis.plot([], [], drawstyle='steps')
                self.setSpectrumInfo(spectrum=line, index=index)
                if len(w) > 0:
                    X = np.array(self.create_range(binx, minx, maxx))
                    line.set_data(X, w)
                    self.setSpectrumInfo(spectrum=line, index=index)
            else:
                if (DEBUG):
                    print("2d case...")
                miny = self.getSpectrumInfoREST("miny", index=index)
                maxy = self.getSpectrumInfoREST("maxy", index=index)
                biny = self.getSpectrumInfoREST("biny", index=index)
                # empty data for initialization
                if w is None:
                    w = np.zeros((int(binx), int(biny))) 
                # setup up palette
                if (self.wConf.button2D_option.currentText() == 'Dark'):
                    #self.palette = 'plasma'
                    self.palette = 'plasma_r'
                else:
                    self.palette = copy(plt.cm.plasma)
                    w = np.ma.masked_where(w < 0.1, w)
                    self.palette.set_bad(color='white')
                
                #check if enlarged mode, dont want to modify spectrum dict in enlarged mode
                self.setSpectrumInfo(spectrum=axis.imshow(w,
                                                            interpolation='none',
                                                            extent=[float(minx),float(maxx),float(miny),float(maxy)],
                                                            aspect='auto',
                                                            origin='lower',
                                                            vmin=float(self.minZ), vmax=float(self.maxZ),
                                                            cmap=self.palette), index=index)

                if w is not None :
                    spectrum = self.getSpectrumInfo("spectrum", index=index)
                    spectrum.set_data(w)
                    self.setSpectrumInfo(spectrum=spectrum, index=index)

                # setup colorbar only for 2D
                # if not self.currentPlot.cbar[index]:
                if self.getEnlargedSpectrum() is None:
                    divider = make_axes_locatable(axis)
                    cax = divider.append_axes('right', size='5%', pad=0.05)
                    self.currentPlot.figure.colorbar(spectrum, cax=cax, orientation='vertical')
            #Simon - here fill tab dict with gates
            self.updateHistList(index)


    # geometrically add plots to the right place and calls plotting
    # should be called only by addPlot and on_dblclick when entering enlarged mode
    def add(self, index):
        a = None
        #cannot use getSpectrumAxes here because the underlying list is built in setupPlot
        if self.currentPlot.isEnlarged:
            #following line to work with multiple tabs, otherwise the default current axes are in the latest tab
            plt.sca(self.currentPlot.figure.axes[0])
            a = plt.gca()
        else:
            a = self.select_plot(index)
        #clear plot and if 2D remove color bar
        try:
            self.removeCb(a)
        except:
            pass
        a.clear()
        #set lines 1D or 2D properties and plot limits with spectrum limits
        self.setupPlot(a, index)

        if (self.currentPlot.h_setup[index]):
            self.currentPlot.h_setup[index] = False


    #hist_list will be obselete, kept here for now because of gates
    def updateHistList(self, index):
        if (DEBUG):
            print("Inside updateHistList")
        hist_name = self.currentPlot.h_dict[index]['name']

        if hist_name not in self.currentPlot.hist_list:
            self.currentPlot.hist_list.insert(index, hist_name)


    # geometrically add plots to the right place
    # plot axis as defined in the ReST interface.
    def addPlot(self):
        if (DEBUG):
            print("Inside addPlot")
            print("check tab ",self.wTab.currentIndex(),len(self.wTab)-1,len(self.wTab.selected_plot_index_bak))

        if self.wConf.histo_list.count() == 0 : 
            QMessageBox.about(self, "Warning", 'Please click on "Connection" and fill in the information')

        try:
            # currentPlot = self.currentPlot
            # if we load the geometry from file
            if self.currentPlot.isLoaded:
                if (DEBUG):
                    print("Inside addPlot - loaded")
                    print(self.getGeo())
                counter = 0
                for key, value in self.getGeo().items():
                    if (DEBUG):
                        print("counter -->", counter)
                    index = self.wConf.histo_list.findText(value, QtCore.Qt.MatchFixedString)
                    if self.getSpectrumInfoREST("dim", name=value) is None: return
                    # changing the index to the correct histogram to load
                    self.wConf.histo_list.setCurrentIndex(index)
                    counter += 1
                for key, value in self.getGeo().items():
                    self.add(key)
            else:
                if (DEBUG):
                    print("Inside addPlot - not loaded")
                # self adding
                index = self.nextIndex()
                #Set the plot according to the selected name in the spectrum list widget
                name = str(self.wConf.histo_list.currentText())

                if self.getSpectrumInfoREST("dim", name=name) is None: return

                self.setGeo(index, name)
                self.currentPlot.h_limits[index] = {}
                self.currentPlot.h_setup[index] = True
                self.add(index)
                # if gate in gateList:
                self.drawAllGates()
                #draw dashed red rectangle to indicate where the next plot would be added, based on next_plot_index, selected_plot_index is unchanged.
                #recDashed added only here
                self.removeRectangle()
                self.currentPlot.recDashed = self.createDashedRectangle(self.currentPlot.figure.axes[self.currentPlot.next_plot_index])
                # Following lines to try to plot only the relevant pad not the entire canvas
                # spectrum = self.getSpectrum(index)
                # self.select_plot(index).draw_artist(spectrum)
                self.currentPlot.figure.tight_layout()
                self.currentPlot.canvas.draw()
                self.currentPlot.isSelected = False
        except NameError:
            raise


    #why not using np.linspace(vmin, vmax, bins)
    def create_range(self, bins, vmin, vmax):
        x = []
        step = (float(vmax)-float(vmin))/float(bins)
        for i in np.arange(float(vmin), float(vmax), step):
            x.append(i + step)
        x.insert(0, float(vmin))
        return x

    # fill spectrum with new data
    # called in addPlot and updatePlot
    # dont actually draw the plot in this function
    def plotPlot(self, axis, index, threshold=0.1):
        currentPlot = self.currentPlot
        if (DEBUG):
            print("Inside plotPlot")
            print("verification tab index", self.wTab.currentIndex())

        # name = self.nameFromIndex(index)
        # Use spectrumInfoREST dont want to change the resolution etc.
        dim = self.getSpectrumInfoREST("dim", index=index)
        minx = self.getSpectrumInfoREST("minx", index=index)
        maxx = self.getSpectrumInfoREST("maxx", index=index)
        binx = self.getSpectrumInfoREST("binx", index=index)
        spectrum = self.getSpectrumInfo("spectrum", index=index)
        w = self.getSpectrumInfoREST("data", index=index)

        if len(w) <= 0:
            return

        if dim == 1:
            if (DEBUG):
                print("1d case..")
            X = np.array(self.create_range(binx, minx, maxx))
            spectrum.set_data(X, w)
        else:
            if (DEBUG):
                print("2d case..")
            if (self.wConf.button2D_option.currentText() == 'Light'):
                w = np.ma.masked_where(w < threshold, w)
            spectrum.set_data(w)
        self.setSpectrumInfo(spectrum=spectrum, index=index)
        self.currentPlot = currentPlot


    def clearPlot(self):
        if (DEBUG):
            print("Inside clearPlot")
            print("self.currentPlot.h_dict", self.currentPlot.h_dict)
        # Simon - added following lines to avoid None plot index
        index = self.autoIndex()
        if index is None:
            return

        if self.currentPlot.h_dict[index]["name"] != "empty" :
            a = self.getSpectrumInfo("axis", index=index)
            a.clear()
        self.currentPlot.canvas.draw()


    def updatePlot(self):
        if (DEBUG):
            print("Inside updatePlot")
            print("self.currentPlot.h_dict", self.currentPlot.h_dict)
            print("self.currentPlot.h_dict_geo", self.getGeo())
            print("self.currentPlot.h_setup", self.currentPlot.h_setup)
            print("verification tab index", self.wTab.currentIndex())

        name = str(self.wConf.histo_list.currentText())
        index = self.autoIndex()
        if index is None or self.getSpectrumInfoREST("dim", name=name) is None: return
        try:
            #x_range, y_range = self.getAxisProperties(index)
            if self.currentPlot.isEnlarged:
                ax = self.getSpectrumInfo("axis", index=0)
                self.plotPlot(ax, index)
                #reset the axis limits as it was before enlarge
                #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                dim = self.getSpectrumInfoREST("dim", index=0)
                if dim == 1:
                    self.setAxisScale(ax, 0, "x", "y")
                elif dim == 2:
                    self.setAxisScale(ax, 0, "x", "y", "z")
                try:
                    self.removeCb(ax)
                except:
                    pass
            else:
                if (DEBUG):
                    print("Inside updatePlot - multipanel mode")
                    debug_info = [(index, value) for index, value in self.currentPlot.h_dict.items()]
                    print("index, value ", debug_info)
                for index, value in self.getGeo().items():
                    ax = self.getSpectrumInfo("axis", index=index)
                    self.plotPlot(ax, index)
                    #reset the axis limits as it was before enlarge
                    #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                    dim = self.getSpectrumInfoREST("dim", index=index)
                    if dim == 1:
                        self.setAxisScale(ax, index, "x", "y")
                    elif dim == 2:
                        self.setAxisScale(ax, index, "x", "y", "z")
                    if (self.currentPlot.h_setup[index]):
                        self.currentPlot.h_setup[index] = False

            self.currentPlot.figure.tight_layout()
            #set all axis limits, deal also with log and autoscale
            self.drawAllGates()
            self.currentPlot.canvas.draw()
        except NameError:
            raise


    #Used in zoomCallBack to save the new axis limits
    #sleepTime is a small delay to ensure this function is executed after on_release
    #seems necessary to get the updated axis limits (zoom toolbar action ends on_release)
    def updatePlotLimits(self, sleepTime=0):
        # update currentPlot limits with what's on the actual plot
        ax = None
        index = self.currentPlot.selected_plot_index
        ax = self.getSpectrumInfo("axis", index=index)
        im = ax.images

        time.sleep(sleepTime)

        if (DEBUG):
            print(ax.get_xlim(), ax.get_ylim())
        try:
            #Simon - modified the following lines
            x_range, y_range = self.getAxisProperties(index)
            self.setSpectrumInfo(minx=x_range[0], index=index)
            self.setSpectrumInfo(maxx=x_range[1], index=index)
            self.setSpectrumInfo(miny=y_range[0], index=index)
            self.setSpectrumInfo(maxy=y_range[1], index=index)
            #Set axis limits try with spectrum 
            ax.set_xlim(x_range[0], x_range[1])
            ax.set_ylim(y_range[0], y_range[1])
            self.currentPlot.isZoomCallback = True
            spectrum = self.getSpectrumInfo("spectrum", index=index)
            spectrum.axes.set_xlim(x_range[0], x_range[1])
            spectrum.axes.set_ylim(y_range[0], y_range[1])
            self.setSpectrumInfo(spectrum=spectrum, index=index)
        except NameError as err:
            print(err)
            pass


    #Simon - added following def to avoid None plot index
    def autoIndex(self):
        if (DEBUG):
            print("Inside autoIndex")

        if self.currentPlot.isSelected == False or self.currentPlot.selected_plot_index is None:
            if self.wTab.selected_plot_index_bak[self.wTab.currentIndex()] is not None:
                self.currentPlot.index = self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]
            else :
                self.currentPlot.index = self.check_index()
        else:
            self.currentPlot.index = self.currentPlot.selected_plot_index
            self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]= self.currentPlot.selected_plot_index

        return self.currentPlot.index


    #go to next index
    def nextIndex(self):
        if (DEBUG):
            print("Inside nextIndex")
        tabIndex = self.wTab.currentIndex()

        #Try to deal with all cases... not elegant
        #first case when ex: coming back from zoom mode or if no plot selected
        if self.currentPlot.selected_plot_index is None:
            if self.wTab.selected_plot_index_bak[tabIndex] is not None:
                self.forNextIndex(tabIndex,self.currentPlot.next_plot_index)
            else :
                self.currentPlot.index = self.check_index()
                self.wTab.selected_plot_index_bak[tabIndex]= self.currentPlot.index
                self.currentPlot.next_plot_index = self.setIndex(self.currentPlot.next_plot_index)
        #second case when select a plot before clicking "Add"
        elif self.currentPlot.selected_plot_index == self.currentPlot.next_plot_index:
            self.currentPlot.index = self.currentPlot.selected_plot_index
            self.wTab.selected_plot_index_bak[tabIndex]= self.currentPlot.selected_plot_index
            self.currentPlot.next_plot_index = self.setIndex(self.currentPlot.next_plot_index)
        #third case when click "Add" without selecting a plot, will draw in the next frame
        elif self.currentPlot.selected_plot_index != self.currentPlot.next_plot_index and self.currentPlot.next_plot_index>=0:
            self.forNextIndex(tabIndex,self.currentPlot.next_plot_index)

        return self.currentPlot.index


    def forNextIndex(self, tabIndex, index):
        self.currentPlot.index = index
        self.currentPlot.selected_plot_index = index
        self.wTab.selected_plot_index_bak[tabIndex]= self.currentPlot.selected_plot_index
        self.currentPlot.next_plot_index = self.setIndex(index)


    def setIndex(self, indexToChange):
        row = int(self.wConf.histo_geo_row.currentText())
        col = int(self.wConf.histo_geo_col.currentText())
        try:
            #Once in the nextIndex first case change to next_plot_index to 0 (then +1)
            if indexToChange == -1:
                indexToChange = 0
            if indexToChange <= row*col-1:
                if indexToChange == row*col-1:
                    indexToChange = 0
                else:
                    indexToChange += 1
        except IndexError as e:
            print(f"An IndexError occured: {e}")
        return indexToChange


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
            # self.updateHistoInfo()
            self.check_histogram();
        except:
            pass

    def plot1DGate(self, axis, histo_name, gate_name, gate_line):
        if (DEBUG):
            print("inside plot1dgate for", histo_name, "with gate", gate_name)
        new_line = [mlines.Line2D([],[], picker=5), mlines.Line2D([],[], picker=5)]
        ymin, ymax = axis.get_ybound()
        new_line[0].set_data([gate_line[0],gate_line[0]], [ymin, ymax])
        if "summing_region" in gate_name:
            new_line[0].set_color('blue')
        else:
            new_line[0].set_color('red')
        axis.add_artist(new_line[0])
        new_line[1].set_data([gate_line[1],gate_line[1]], [ymin, ymax])
        if "summing_region" in gate_name:
            new_line[1].set_color('blue')
        else:
            new_line[1].set_color('red')
        axis.add_artist(new_line[1])

        axis.draw_artist(new_line[0])
        axis.draw_artist(new_line[1])


    def plot2DGate(self, axis, histo_name, gate_name, gate_line):
        if (DEBUG):
            print("inside plot2dgate for", histo_name, "with gate", gate_name)
        new_line = mlines.Line2D([],[], picker=5)
        if (DEBUG):
            print(gate_line)
        new_line.set_data(gate_line[0], gate_line[1])
        if "summing_region" in gate_name:
            new_line.set_color('blue')
        else:
            new_line.set_color('red')
        axis.add_artist(new_line)
        # axis.canvas.blit(new_line.get_bbox())
        # new_line.figure.canvas.draw()
        axis.draw_artist(new_line)

    def drawRegion(self, histo_name, region_name, region_line):
        if (DEBUG):
            print("Inside drawRegion")
            print(histo_name, region_name, region_line)
        ax = None
        rtype = self.currentPlot.regionTypeDict[region_name]
        if (DEBUG):
            print(self.currentPlot.regionTypeDict)
            print("region type to be drawn", rtype)
        # index = self.get_key(histo_name)
        if self.currentPlot.isEnlarged:
            ax = plt.gca()
            if (self.wConf.histo_list.currentText() == histo_name):
                if (DEBUG):
                    print("I need to draw ", region_name,"to", self.wConf.histo_list.currentText())
                if (self.wConf.button1D.isChecked() and (rtype == "s" or rtype == "gs")):
                    self.plot1DGate(ax, histo_name, region_name, region_line)
                else:
                    self.plot2DGate(ax, histo_name, region_name, region_line)
        else:
            index = self.get_key(histo_name)
            ax = self.select_plot(index)
            if (rtype == "s" or rtype == "gs"):
                self.plot1DGate(ax, histo_name, region_name, region_line)
            else:
                self.plot2DGate(ax, histo_name, region_name, region_line)

        # ax.canvas.blit(region_line.get_bbox())
        # self.currentPlot.canvas.draw()

    def drawGate(self, histo_name, gate_name, gate_line):
        if (DEBUG):
            print("Inside drawGate")
        gtype = self.currentPlot.gateTypeDict[gate_name]
        if (DEBUG):
            print("gate type to be drawn", gtype)
        ax = None
        if self.currentPlot.isEnlarged:
            ax = plt.gca()
            # if (self.currentPlot.h_dict_geo[self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]] == histo_name):
            if (self.nameFromIndex[self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]] == histo_name):
                if (DEBUG):
                    print("I need to add the gate name", gate_name,"to", histo_name)
                if (gtype == "s" or gtype == "gs"):
                    self.plot1DGate(ax, histo_name, gate_name, gate_line)
                else:
                    self.plot2DGate(ax, histo_name, gate_name, gate_line)
        else:
            indexlst = self.get_keylst(histo_name)
            for index in indexlst:
                ax = self.select_plot(index)
                if (gtype == "s" or gtype == "gs"):
                    self.plot1DGate(ax, histo_name, gate_name, gate_line)
                else:
                    self.plot2DGate(ax, histo_name, gate_name, gate_line)

        # self.currentPlot.canvas.draw()
        # ax.canvas.blit(gate_line.get_bbox())

    def drawAllGates(self):
        artist_dict = self.currentPlot.artist_dict
        region_dict = self.currentPlot.region_dict
        hist_list = self.currentPlot.hist_list

        if (DEBUG):
            print("Inside drawAllGate")
            print(artist_dict)
            print(region_dict)


        artist_dict_keys = set(artist_dict.keys())
        gates_to_draw = []
        for hist in hist_list:
            if hist in artist_dict_keys:
                for gate_name, gate_line in artist_dict[hist].items():
                    self.drawGate(hist, gate_name, gate_line)

        region_dict_keys = set(region_dict.keys())
        regions_to_draw = []
        for hist in hist_list:
            if hist in region_dict_keys:
                for region_name, region_line in region_dict[hist].items():
                    self.drawRegion(hist, region_name, region_line)


    def createSRegion(self):
        if (DEBUG):
            print("Clicked createSRegion in tab", self.wTab.currentIndex())
        self.currentPlot.region_name = "summing_region_tab_"+str(self.wTab.currentIndex())+"_"+str(self.currentPlot.counter_sr)
        self.currentPlot.region_type = ""
        if (DEBUG):
            print("creating", self.currentPlot.region_name,"in",self.wConf.histo_list.currentText())
        self.currentPlot.counter_sr+=1

        if self.wConf.button1D.isChecked():
            self.currentPlot.region_type = "s"
        else:
            self.currentPlot.region_type = "c"
        # adding gate
        self.currentPlot.regionTypeDict[self.currentPlot.region_name] = self.currentPlot.region_type

        self.currentPlot.toCreateSRegion = True;
        self.createRegion()

    def addGate(self):
        if (DEBUG):
            print("inside addGate")
        try:
            hname = self.wConf.histo_list.currentText()
            gname = self.wConf.listGate.currentText()
            gate_dict = self.currentPlot.artist_dict[hname]
            if gname in gate_dict:
                self.drawGate(hname, gname, value)
        except:
            pass


    def deleteGate(self):
        try:
            hname = self.wConf.histo_list.currentText()
            gname = self.wConf.listGate.currentText()
            # delete from plot
            self.clearGate()
            # delete from combobox and change index
            index = self.wConf.listGate.findText(gname, QtCore.Qt.MatchFixedString)
            if index >= 0:
                self.wConf.listGate.removeItem(index)
            # delete from artist dict
            gatedict_in_histo = self.currentPlot.artist_dict[hname]
            if gname in gatedict_in_histo:
                del self.currentPlot.artist_dict[hname][gname]
            # delete from spectcl
            self.rest.deleteGate(gname)
            self.currentPlot.canvas.draw()

        except:
            pass

    def clearGate(self):
        try:
            ax = None
            hname = self.wConf.histo_list.currentText()
            gname = self.wConf.listGate.currentText()

            if (DEBUG):
                print("gate",gname,"in",hname)

            if self.currentPlot.isEnlarged:
                ax = plt.gca()
            else:
                ax = self.select_plot(self.currentPlot.selected_plot_index)

            if (DEBUG):
                print("List of the child Artists of this Artist \n",
                      *list(ax.get_children()), sep ="\n")
            lst = list(ax.get_children())
	    #Simon - changed following lines
            gatedict_in_histo = self.currentPlot.artist_dict[hname]
            if gname in gatelist_in_histo:
                for line in lst:
                    if isinstance(line, matplotlib.lines.Line2D):
                        if self.wConf.button2D.isChecked():
                            if round(line.get_xdata()[0],2) == round(gate[gname][0][0],2) and len(line.get_xdata()) == len(gate[gname][0]):
                                #ax.lines.pop(i)
                                line.remove()
                        if self.wConf.button1D.isChecked():
                            if line.get_xdata()[0] == gate[gname][0] or line.get_xdata()[0] == gate[gname][1]:
                                line.remove()

            self.currentPlot.canvas.draw()

        except:
            pass

    def createGate(self):
        if (DEBUG):
            print("Inside createGate")
        # check for histogram existance
        name = self.wConf.histo_list.currentText()
        gate_name = self.wConf.listGate.currentText()
        # gate_parameter = self.wConf.listParams[0].currentText()
        gateType = None

        if (name==""):
            return QMessageBox.about(self,"Warning!", "Please add at least one spectrum")
        else:
            # creating entry in combobox if it doesn't exist
            allItems = [self.wConf.listGate.itemText(i) for i in range(self.wConf.listGate.count())]
            result = gate_name in allItems
            while result:
                self.currentPlot.counter += 1
                gate_name = "default_gate_"+str(self.currentPlot.counter)
                result = gate_name in allItems

            # gate name
            text, okPressed = QInputDialog.getText(self, "Gate name", "Please choose a name for the gate:", QLineEdit.Normal, gate_name)
            if okPressed:
                if text:
                    gate_name = text
                else:
                    gate_name = "default_gate_"+str(self.currentPlot.counter)
            else:
                gate_name = "default_gate_"+str(self.currentPlot.counter)
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
                gateType = item
            else:
                return QMessageBox.about(self,"Warning!", "Please select a gate type")

        # adding gate
        self.currentPlot.gateTypeDict[gate_name] = gateType
        if (DEBUG):
            print(self.currentPlot.gateTypeDict)
        self.updateGateType()
        # gindex = self.findGateType(gate_name)
        # if (DEBUG):
        #     print("index of the gate in combobox", gindex)
        # self.wConf.listGate_type.setCurrentIndex(gindex)
        self.currentPlot.toCreateGate = True;
        self.createRegion()

    def createRegion(self):
        ax = plt.gca()
        self.cleanRegion()

    def cleanRegion(self):
        self.currentPlot.xs.clear()
        self.currentPlot.ys.clear()
        self.currentPlot.listLine = []

    def addLine(self, posx):
        if (DEBUG):
            print("Inside addLine", posx)
        ax = plt.gca()
        ymin, ymax = ax.get_ybound()
        l = mlines.Line2D([posx,posx], [ymin,ymax], picker=5)
        ax.add_line(l)
        if self.currentPlot.toCreateSRegion == True:
            l.set_color('b')
        else:
            l.set_color('r')
        self.currentPlot.xs.append(posx)
        if (DEBUG):
            print(posx,ymin,ymax)
        return l

    def addPolygon(self, posx, posy):
        if (DEBUG):
            print("Inside addPolygon", posx, posy)
        ax = plt.gca()
        self.currentPlot.polygon = mlines.Line2D([],[], picker=5)
        self.currentPlot.xs.append(posx)
        self.currentPlot.ys.append(posy)
        self.currentPlot.polygon.set_data(self.currentPlot.xs, self.currentPlot.ys)
        ax.add_line(self.currentPlot.polygon)
        if self.currentPlot.toCreateSRegion == True:
            self.currentPlot.polygon.set_color('b')
        else:
            self.currentPlot.polygon.set_color('r')

    def removeLine(self):
        l = self.currentPlot.listLine[0]
        self.currentPlot.listLine.pop(0)
        self.currentPlot.xs.pop(0)
        if (DEBUG):
            print("after removing lines...", self.currentPlot.xs)
        l.remove()

    def releaseonclick(self, event):
        self.currentPlot.canvas.mpl_disconnect(self.releaser)
        self.currentPlot.canvas.mpl_disconnect(self.follower)
        self.currentPlot.canvas.mpl_disconnect(self.sid)

        xs = []
        lst = list(self.edit_ax.get_children())
        for obj in lst[1:]:
            if isinstance(obj, matplotlib.lines.Line2D):
                xs.append(obj.get_xdata()[0])
                obj.set_color("red")

        xs.sort()
        # update gate
        gname = self.wConf.listGate.currentText()
        hname = self.wConf.histo_list.currentText()
        self.currentPlot.artist1D[gname] = []
        self.currentPlot.artist1D[gname] = xs
        if hname in self.currentPlot.artist_dict:
            self.currentPlot.artist_dict[hname][gname] = self.currentPlot.artist1D[gname]
        else:
            self.currentPlot.artist_dict[hname] = {gname : self.currentPlot.artist1D[gname]}

        self.formatLinetoREST(deepcopy(xs))

        self.connect()
        self.currentPlot.toEditGate = False

    def followmouse(self, event):
        self.thisline.set_xdata([event.xdata, event.xdata])
        self.currentPlot.canvas.draw_idle()

    def clickonline(self, event):
        self.currentPlot.toEditGate = True
        if (DEBUG):
            print("Inside clickonline, self.currentPlot.toEditGate", self.currentPlot.toEditGate)
        self.thisline = None
        lst = list(self.edit_ax.get_children())
        for obj in lst:
            if isinstance(obj, matplotlib.lines.Line2D):
                if obj == event.artist:
                    if (DEBUG):
                        print("id", id(obj))
                        print("obj", obj)
                        print("obj x data", obj.get_xdata())
                        print("obj y data", obj.get_ydata())
                    self.thisline = obj

        if self.wConf.button1D.isChecked():
            self.thisline.set_color("green")
            self.follower = self.currentPlot.canvas.mpl_connect("motion_notify_event", self.followmouse)
            self.releaser = self.currentPlot.canvas.mpl_connect("button_press_event", self.releaseonclick)
        else:
            self.thisline.set_visible(False)
            self.polyXY = self.convertToArray(self.thisline)
            if self.wConf.isDrag:
                self.polygon = Polygon(self.polyXY, facecolor = 'green', alpha=0.5)
                self.edit_ax.add_patch(self.polygon)

                Artist.remove(self.thisline)

                self.e_press = self.currentPlot.canvas.mpl_connect('button_press_event', self.button_press_callback)
                self.e_motion = self.currentPlot.canvas.mpl_connect('motion_notify_event', self.motion_notify_callback)
                self.e_release = self.currentPlot.canvas.mpl_connect('button_release_event', self.button_release_callback)

            elif self.wConf.isEdit:

                self.polygon = Polygon(self.polyXY, facecolor = 'green', alpha=0.5, animated=True)
                self.edit_ax.add_patch(self.polygon)

                Artist.remove(self.thisline)

                # new stuff
                if self.polygon is None:
                    raise RuntimeError("You must first add the polygon to a figure or canvas before defining the interactor")

                canvas = self.polygon.figure.canvas
                x, y = zip(*self.polygon.xy)
                self.line = mlines.Line2D(x, y, marker='o', color="green", markerfacecolor="green", animated=True)
                self.edit_ax.add_line(self.line)

                self.cid = self.polygon.add_callback(self.poly_changed)

                canvas.mpl_connect('draw_event', self.on_draw)
                canvas.mpl_connect('button_press_event', self.on_button_press)
                canvas.mpl_connect('key_press_event', self.on_key_press)
                canvas.mpl_connect('button_release_event', self.on_button_release)
                canvas.mpl_connect('motion_notify_event', self.on_mouse_move)
                canvas.mpl_connect('button_press_event', self.gate_release)

                self.canvas = canvas

            self.currentPlot.canvas.draw()

    def gate_release(self, event):
        if event.button == 3 and self.wConf.isEdit:
            if (DEBUG):
                print("Inside gate_release, self.currentPlot.toEditGate", self.currentPlot.toEditGate)
            self.canvas.mpl_disconnect(self.on_button_press)

            self.showverts = not self.showverts
            self.line.set_visible(self.showverts)
            if not self.showverts:
                self._ind = None

                        # update line position
            pol2line = self.convertToList(self.polygon)
            pol2line.append(pol2line[0])
            x, y = map(list, zip(*pol2line))
            # push gate to shared memory 2D
            self.formatLinetoREST(x,y)
            # update gate
            gname = self.wConf.listGate.currentText()
            hname = self.wConf.histo_list.currentText()
            self.currentPlot.artist2D[gname] = []
            self.currentPlot.artist2D[gname] = [deepcopy(x), deepcopy(y)]
            if hname in self.currentPlot.artist_dict:
                self.currentPlot.artist_dict[hname][gname] = self.currentPlot.artist2D[gname]
            else:
                self.currentPlot.artist_dict[hname] = {gname : self.currentPlot.artist2D[gname]}


            self.thisline = mlines.Line2D([],[], color= "red", picker=5)
            self.thisline.set_data(x, y)
            self.edit_ax.add_line(self.thisline)

            self.polygon.set_visible(False)
            self.line.set_visible(False)

            self.canvas.mpl_disconnect(self.on_mouse_move)
            self.canvas.mpl_disconnect(self.on_button_release)
            self.canvas.mpl_disconnect(self.on_key_press)
            self.canvas.mpl_disconnect(self.on_draw)
            self.canvas.mpl_disconnect(self.gate_release)

            self.currentPlot.canvas.mpl_disconnect(self.sid)
            self.connect()

            self.currentPlot.toEditGate = False
            self.canvas.draw()
            self.currentPlot.canvas.draw()

    def get_ind_under_point(self, event):
        # Return the index of the point closest to the event position or *None*
        # if no point is within ``self.epsilon`` to the event position.
        # display coords
        xy = np.asarray(self.polygon.xy)
        xyt = self.polygon.get_transform().transform(xy)
        xt, yt = xyt[:, 0], xyt[:, 1]
        d = np.hypot(xt - event.x, yt - event.y)
        indseq, = np.nonzero(d == d.min())
        ind = indseq[0]

        if d[ind] >= self.epsilon:
            ind = None

        return ind

    def on_button_press(self, event):
        # Callback for mouse button presses
        if not self.showverts:
            return
        if event.inaxes is None:
            return
        if event.button != 1:
            return
        self._ind = self.get_ind_under_point(event)

    def on_button_release(self, event):
        # Callback for mouse button releases
        if not self.showverts:
            return
        if event.button != 1:
            return
        self._ind = None

    def on_key_press(self, event):
        # Callback for key presses
        if not event.inaxes:
            return
        '''
        if event.key == 't':
            self.showverts = not self.showverts
            self.line.set_visible(self.showverts)
            if not self.showverts:
                self._ind = None
        '''
        if event.key == 'd':
            ind = self.get_ind_under_point(event)
            if ind is not None:
                self.polygon.xy = np.delete(self.polygon.xy,
                                         ind, axis=0)
                self.line.set_data(zip(*self.polygon.xy))
        elif event.key == 'i':
            xys = self.polygon.get_transform().transform(self.polygon.xy)
            p = event.x, event.y  # display coords
            for i in range(len(xys) - 1):
                s0 = xys[i]
                s1 = xys[i + 1]
                d = self.dist_point_to_segment(p, s0, s1)
                if d <= self.epsilon:
                    self.polygon.xy = np.insert(
                        self.polygon.xy, i+1,
                        [event.xdata, event.ydata],
                        axis=0)
                    self.line.set_data(zip(*self.polygon.xy))
                    break
        if self.line.stale:
            self.canvas.draw_idle()

    def on_mouse_move(self, event):
        # Callback for mouse movements
        if not self.showverts:
            return
        if self._ind is None:
            return
        if event.inaxes is None:
            return
        if event.button != 1:
            return
        x, y = event.xdata, event.ydata

        self.polygon.xy[self._ind] = x, y
        if self._ind == 0:
            self.polygon.xy[-1] = x, y
        elif self._ind == len(self.polygon.xy) - 1:
            self.polygon.xy[0] = x, y
        self.line.set_data(zip(*self.polygon.xy))

        self.canvas.restore_region(self.background)
        self.edit_ax.draw_artist(self.polygon)
        self.edit_ax.draw_artist(self.line)
        self.canvas.blit(self.edit_ax.bbox)
        self.canvas.draw_idle()

    def dist(self, x, y):
        # Return the distance between two points.
        d = x - y
        return np.sqrt(np.dot(d, d))

    def dist_point_to_segment(self, p, s0, s1):
        #Get the distance of a point to a segment.
        #  *p*, *s0*, *s1* are *xy* sequences
        #This algorithm from
        #http://www.geomalgorithms.com/algorithms.html
        v = s1 - s0
        w = p - s0
        c1 = np.dot(w, v)
        if c1 <= 0:
            return self.dist(p, s0)
        c2 = np.dot(v, v)
        if c2 <= c1:
            return self.dist(p, s1)
        b = c1 / c2
        pb = s0 + b * v
        return self.dist(p, pb)

    def on_draw(self, event):
        self.background = self.canvas.copy_from_bbox(self.edit_ax.bbox)
        self.edit_ax.draw_artist(self.polygon)
        self.edit_ax.draw_artist(self.line)
        # do not need to blit here, this will fire before the screen is
        # updated

    def poly_changed(self, poly):
        # This method is called whenever the pathpatch object is called.
        # only copy the artist props to the line (except visibility)
        vis = self.line.get_visible()
        Artist.update_from(self.line, poly)
        self.line.set_visible(vis)  # don't use the poly visibility state

    def button_press_callback(self, event):
        if (DEBUG):
            print("Inside button_press_callback, self.currentPlot.toEditGate", self.currentPlot.toEditGate)
        if (event.inaxes != self.polygon.axes):
            return
        contains, attrd = self.polygon.contains(event)
        if not contains:
            return
        x0, y0 = self.polygon.xy[0]
        if self.bPressed == False:
            self.press = x0, y0, event.xdata, event.ydata
            self.bPressed = True

    def button_release_callback(self, event):
        # right click
        if event.button == 3 and self.wConf.isDrag:
            if (DEBUG):
                print("Inside button_release_callback, self.currentPlot.toEditGate", self.currentPlot.toEditGate)
            self.currentPlot.canvas.mpl_disconnect(self.e_press)
            self.currentPlot.canvas.mpl_disconnect(self.e_motion)
            self.currentPlot.canvas.mpl_disconnect(self.e_release)
            self.polygon.set_visible(False)
            # update line position
            pol2line = self.convertToList(self.polygon)
            pol2line.append(pol2line[0])
            x, y = map(list, zip(*pol2line))
            # push gate to shared memory 2D
            self.formatLinetoREST(x,y)
            # update gate
            gname = self.wConf.listGate.currentText()
            hname = self.wConf.histo_list.currentText()
            self.currentPlot.artist2D[gname] = []
            self.currentPlot.artist2D[gname] = [deepcopy(x), deepcopy(y)]
            if hname in self.currentPlot.artist_dict:
                self.currentPlot.artist_dict[hname][gname] = self.currentPlot.artist2D[gname]
            else:
                self.currentPlot.artist_dict[hname] = {gname : self.currentPlot.artist2D[gname]}

            self.thisline = mlines.Line2D([],[], color= "red", picker=5)
            self.thisline.set_data(x, y)
            self.edit_ax.add_line(self.thisline)

            self.currentPlot.canvas.mpl_disconnect(self.sid)
            self.connect()

            self.bPressed = False
            self.currentPlot.toEditGate = False
            self.currentPlot.canvas.draw()

    def motion_notify_callback(self, event):
        try:
            if (DEBUG):
                print("Inside motion_notify_callback, self.currentPlot.toEditGate", self.currentPlot.toEditGate)
            if (event.inaxes != self.polygon.axes):
                return

            if self.bPressed == True:
                x0, y0, xpress, ypress = self.press
            dx = event.xdata - xpress
            dy = event.ydata - ypress
            # this shift from the original position not from the modified one
            xdx = [i+dx for i,_ in self.polyXY]
            ydy = [i+dy for _,i in self.polyXY]
            poly_xy = [[a, b] for a, b in zip(xdx, ydy)]
            self.polygon.set_xy(poly_xy)
            self.polygon.figure.canvas.draw_idle()

        except:
            pass

    def editGate(self):
        try:
            self.edit_ax = plt.gca()
            self.disconnect()

            '''
            print("======= list of lines =======")
            lst = list(self.edit_ax.get_children())
            for obj in lst:
                if isinstance(obj, matplotlib.lines.Line2D):
                    print("id", id(obj), self.wConf.listGate.currentText())
                    print("obj", obj)
                    print("obj x data", obj.get_xdata())
                    print("obj y data", obj.get_ydata())
            '''

            self.sid = self.currentPlot.canvas.mpl_connect('pick_event', self.clickonline)

        except NameError:
            raise


    ##############################
    # 11) 1D/2D region integration
    ##############################

    def integrate(self):
        if (DEBUG):
            print("Inside integrate")
            print(self.currentPlot.artist_dict)
        try:
            for histo_name, gates in self.currentPlot.artist_dict.items():
                if (DEBUG):
                    print(histo_name, gates)
                if (self.wConf.histo_list.currentText() == histo_name):
                    # if histo_name and "summing_region" not in histo_name:
                    if "summing_region" not in histo_name:
                        for gate_name, gate_line in gates.items():
                            if (DEBUG):
                                print(gate_name, gate_line)
                            results = self.rest.integrateGate(histo_name, gate_name)
                            self.addRegion(histo_name, gate_name, results)
                self.resultPopup()

            for histo_name, regions in self.currentPlot.region_dict.items():
                if (DEBUG):
                    print(histo_name, regions)
                if (self.wConf.histo_list.currentText() == histo_name):
                    # if histo_name and "summing_region" not in histo_name:
                    if "summing_region" not in histo_name:
                        for region_name, region_line in regions.items():
                            if (DEBUG):
                                print(region_name, region_line)
                            results = self.rest.integrateGate(histo_name, region_name)
                            self.addRegion(histo_name, region_name, results)
                self.resultPopup()
        except NameError:
            pass


    def resultPopup(self):
        self.resPopup.setGeometry(100,100,724,500)
        if self.resPopup.isVisible():
            self.resPopup.close()

        self.resPopup.show()

    def addRegion(self, histo, gate, results):
        if (DEBUG):
            print("Inside addRegion")
        # Schema for output window
        #["ID", "Spectrum", "Name", "centroid X", "centroid Y", "FWHM X", "FWHM Y", "Area"]
        self.resPopup.tableWidget.setRowCount(0);

        if (DEBUG):
            print(results)
        hname = histo
        gname = gate
        cx = 0
        cy = 0
        fwhmx = 0
        fwhmy = 0
        if isinstance(results["centroid"], list):
            cx = str(results["centroid"][0])
            cy = str(results["centroid"][1])
            fwhmx = str(results["fwhm"][0])
            fwhmy = str(results["fwhm"][1])
        else:
            cx = str(results["centroid"])
            fwhmx = str(results["fwhm"])
        counts = str(results["counts"])

        self.table_row.append([self.ctr, hname, gname, cx, cy, fwhmx, fwhmy, counts])
        self.ctr += 1

        for row in self.table_row:
            inx = self.table_row.index(row)
            self.resPopup.tableWidget.insertRow(inx)
            for i in range(len(row)):
                self.resPopup.tableWidget.setItem(inx,i,QTableWidgetItem(str(row[i])))
        header = self.resPopup.tableWidget.horizontalHeader()
        header.setSectionResizeMode(QHeaderView.Stretch)
        self.resPopup.tableWidget.resizeColumnsToContents()


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
            discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]

            for instance in self.copyAttr.findChildren(QCheckBox):
                if instance.isChecked():
                    flags.append(True)
                else:
                    flags.append(False)

            if (DEBUG):
                print(flags)

            dim = self.getSpectrumInfoREST("dim", index=self.currentPlot.selected_plot_index)
            indexes = []
            xlim_src = []
            ylim_src = []
            zlim_src = []
            scale_src = None

            # creating list of target histograms
            for instance in self.copyAttr.findChildren(QPushButton):
                for index, name in self.getGeo().items():
                    if instance.text() not in discard and instance.isChecked() and name == instance.text() and index != self.currentPlot.selected_plot_index :
                        if (DEBUG):
                            print("histo destination",instance.text())
                        indexes.append(index)

            if (DEBUG):
                print(self.currentPlot.selected_plot_index)
                print(indexes)

            # src values to copy to destination
            xlim_src = ast.literal_eval(self.copyAttr.axisLimLabelX.text())
            ylim_src = ast.literal_eval(self.copyAttr.axisLimLabelY.text())
            scale_src = self.copyAttr.axisSLabel.text()
            scale_src_bool = True if scale_src == "Log" else False
            zlim_src = [float(self.copyAttr.histoScaleValueminZ.text()), float(self.copyAttr.histoScaleValuemaxZ.text())]

            if (DEBUG):
                print(xlim_src, ylim_src, scale_src, zlim_src)
                print(flags)

            # copy to destination
            for index in indexes:
                # set the limits for x,y
                if flags[0]:
                    self.setSpectrumInfo(minx=xlim_src[0], index=index)
                    self.setSpectrumInfo(maxx=xlim_src[1], index=index)
                if flags[1]:
                    self.setSpectrumInfo(miny=ylim_src[0], index=index)
                    self.setSpectrumInfo(maxy=ylim_src[1], index=index)
                # set log/lin scale
                if flags[2]:
                    self.setSpectrumInfo(log=scale_src_bool, index=index)
                # set minZ/maxZ
                if dim == 2 and (flags[3] or flags[4]):
                    #unlog the zlim_src because setAxisScale in updatePlot will apply log() to the limits (twice if dont unlog first...)
                    if scale_src_bool:
                        #setAxisScale dont save zlim in the dictionnary...
                        zmin = 10**(zlim_src[0])
                        zmax = 10**(zlim_src[1])
                        self.setSpectrumInfo(minz=zmin, index=index)
                        self.setSpectrumInfo(maxz=zmax, index=index)
                    else:
                        self.setSpectrumInfo(minz=zlim_src[0], index=index)
                        self.setSpectrumInfo(maxz=zlim_src[1], index=index)
            self.updatePlot()
        except:
            pass


    def closeCopy(self):
        discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
        for instance in self.copyAttr.findChildren(QPushButton):
            if instance.text() not in discard:
                instance.deleteLater()

        self.copyAttr.close()

    def copyPopup(self):
        if self.copyAttr.isVisible():
            self.copyAttr.close()
        # try:
        #     self.updatePlotLimits()
        # except:
        #     return
        index = self.currentPlot.selected_plot_index
        name = self.nameFromIndex(index)
        dim = self.getSpectrumInfoREST("dim", index=index)

        if dim is None : return

        if (DEBUG):
            print("Clicked copyPopup in tab", self.wTab.currentIndex())
        # setting up info for source histogram
        self.copyAttr.histoLabel.setText(name)
        # hdim = 2 if self.wConf.button2D.isChecked() else 1
        if dim == 2 :
            spectrum = self.getSpectrumInfo("spectrum", index=index)
            # zmin = self.getSpectrumInfo("minz", index=index)
            # zmax = self.getSpectrumInfo("maxz", index=index)
            zmin, zmax = spectrum.get_clim()
            # self.copyAttr.histoScaleValueminZ.setText(f"[{zmin:.1f}]")
            # self.copyAttr.histoScaleValuemaxZ.setText(f"[{zmax:.1f}]")
            self.copyAttr.histoScaleValueminZ.setText(f"{zmin}")
            self.copyAttr.histoScaleValuemaxZ.setText(f"{zmax}")
        self.copyAttr.axisSLabel.setText("Log" if self.getSpectrumInfo("log", index=index) else "Linear")
        xmin = self.getSpectrumInfo("minx", index=index)
        xmax = self.getSpectrumInfo("maxx", index=index)
        ymin = self.getSpectrumInfo("miny", index=index)
        ymax = self.getSpectrumInfo("maxy", index=index)
        self.copyAttr.axisLimLabelX.setText(f"[{xmin:.1f},{xmax:.1f}]")
        self.copyAttr.axisLimLabelY.setText(f"[{ymin:.1f},{ymax:.1f}]")

        try:
            for idx, name in self.getGeo().items():
                if dim == self.getSpectrumInfoREST("dim", index=idx):
                    instance = QPushButton(name, self)
                    instance.setCheckable(True)
                    instance.setStyleSheet('QPushButton {color: red;}')
                    self.copyAttr.copy_log.addRow(instance)
                    instance.clicked.connect(lambda state, instance=instance: self.connectCopy(instance))
        except KeyError as e:
            print(f"KeyError occured: {e}")
        self.copyAttr.show()


    def connectCopy(self, instance):
        if (instance.palette().color(QPalette.Text).name() == "#008000"):
            instance.setStyleSheet('QPushButton {color: red;}')
        else:
            instance.setStyleSheet('QPushButton {color: green;}')
        if (DEBUG):
            print(instance.isChecked())


    def connectPopup(self):
        # if self.connectConfig.isVisible():
        #     self.connectConfig.close()

        self.connectConfig.show()


    def closeConnect(self):
        # discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
        # for instance in self.copyAttr.findChildren(QPushButton):
        #     if instance.text() not in discard:
        #         instance.deleteLater()

        self.connectConfig.close()


    def okConnect(self):
        # if (DEBUG):
        #     print("Inside okCopy")
        # self.applyCopy()
        self.connectShMem()
        self.closeConnect()



    ############################
    # 12)  Fitting
    ############################

    def axislimits(self, ax):
        left, right = ax.get_xlim()
        if self.extraPopup.fit_range_min.text():
            left = float(self.extraPopup.fit_range_min.text())
        else:
            left = ax.get_xlim()[0]
        if self.extraPopup.fit_range_max.text():
            right = float(self.extraPopup.fit_range_max.text())
        else:
            right = ax.get_xlim()[1]
        # Make sure xmin is always smaller than xmax.
        if left > right:
            left, right = right, left
            QMessageBox.about(self, "Warning", "xmin > xmax, the provided limits will be swapped for the fit")
        return left, right

    def fit(self):
        histo_name = str(self.wConf.histo_list.currentText())
        fit_funct = self.extraPopup.fit_list.currentText()
        index = self.autoIndex()
        ax = self.getSpectrumInfo("axis", index=index)

        dim = self.getSpectrumInfoREST("dim", index=index)
        binx = self.getSpectrumInfoREST("binx", index=index)
        minxREST = self.getSpectrumInfoREST("minx", index=index)
        maxxREST = self.getSpectrumInfoREST("maxx", index=index)

        config = self.fit_factory._configs.get(fit_funct)
        if (DEBUG):
            print("Fit function", config)
        fit = self.fit_factory.create(fit_funct, **config)

        try:
            if histo_name != "":
                if dim == 1:
                    x = []
                    y = []
                    xtmp = self.create_range(binx, minxREST, maxxREST)

                    if not self.extraPopup.fit_p0.text():
                        self.extraPopup.fit_p0.setText("0")
                    if not self.extraPopup.fit_p1.text():
                        self.extraPopup.fit_p1.setText("0")
                    if not self.extraPopup.fit_p2.text():
                        self.extraPopup.fit_p2.setText("0")
                    if not self.extraPopup.fit_p3.text():
                        self.extraPopup.fit_p3.setText("0")
                    if not self.extraPopup.fit_p4.text():
                        self.extraPopup.fit_p4.setText("0")
                    if not self.extraPopup.fit_p5.text():
                        self.extraPopup.fit_p5.setText("0")
                    if not self.extraPopup.fit_p6.text():
                        self.extraPopup.fit_p6.setText("0")
                    if not self.extraPopup.fit_p7.text():
                        self.extraPopup.fit_p7.setText("0")

                    fitpar = [float(self.extraPopup.fit_p0.text()), float(self.extraPopup.fit_p1.text()),
                              float(self.extraPopup.fit_p2.text()), float(self.extraPopup.fit_p3.text()),
                              float(self.extraPopup.fit_p4.text()), float(self.extraPopup.fit_p5.text()),
                              float(self.extraPopup.fit_p6.text()), float(self.extraPopup.fit_p7.text())]
                    
                    ytmp = (self.getSpectrumInfoREST("data", index=index)).tolist()
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

                    if fit_funct == "Skeleton":
                        fitln = fit.start(x, y, xmin, xmax, fitpar, ax)
                    else:
                        fitln = fit.start(x, y, xmin, xmax, fitpar, ax, self.extraPopup.fit_results)

                else:
                    QMessageBox.about(self, "Warning", "Sorry 2D fitting is not implemented yet")
            else:
                QMessageBox.about(self, "Warning", "Histogram not existing. Please load an histogram...")

            self.currentPlot.canvas.draw()

        except NameError as err:
            print(err)
            pass

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
        ax = self.getSpectrumInfo("axis", index=self.currentPlot.selected_plot_index)
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
        print("Simon - peak finding first ", self.currentPlot.selected_plot_index)
        try:
            # ax = self.getSpectrumAxis(self.currentPlot.selected_plot_index)
            ax = self.getSpectrumInfo("axis", index=self.currentPlot.selected_plot_index)

            x = []
            y = []
            # input points for peak finding
            width = int(self.extraPopup.peak.peak_width.text())

            dim = self.getSpectrumInfoREST("dim", index=index)
            binx = self.getSpectrumInfoREST("binx", index=index)
            minxREST = self.getSpectrumInfoREST("minx", index=index)
            maxxREST = self.getSpectrumInfoREST("maxx", index=index)

            if (DEBUG):
                print(minx, maxx, binx)
            xtmp = self.create_range(binx, minxREST, maxxREST)
            ytmp = (self.getSpectrumInfoREST("data", index=self.currentPlot.selected_plot_index)).tolist()
            if (DEBUG):
                print("xtmp", type(xtmp), "with len", len(xtmp), "ytmp", type(ytmp), "with len", len(ytmp))
            xmin, xmax = ax.get_xlim()
            print("Simon - fitting axis limits", dim, xmin, xmax, xtmp, ytmp)

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

    def convertToList(self, poly):
        polyg = []
        for i,tup in enumerate(poly.xy):
            if i<len(poly.xy)-1:
                polyg.append(list(itertools.chain(tup)))
        return polyg

    def convertToArray(self, line):
        poly = []
        for x,y in line.get_xydata():
            poly.append([x,y])
        poly.pop()  # removing the last element because I need just a list of vertices
        return np.array(poly)

    def createRectangle(self, plot):
        rec = matplotlib.patches.Rectangle((0, 0), 1, 1, ls="-", lw="2", ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)
        return rec

    def createDashedRectangle(self, plot):
        rec = matplotlib.patches.Rectangle((0, 0), 1, 1, ls=":", lw="2", ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)
        return rec

    def removeRectangle(self):
        try:                       
            # if self.currentPlot.recDashed is not None: print("Simon - removeRectangle - ",self.currentPlot.recDashed.get_ls())
            for ax in self.currentPlot.figure.axes:
                for child in ax.get_children():
                    if type(child) == matplotlib.patches.Rectangle :
                        if child.get_ls() == ":" and child.get_lw() == 2:
                            if self.currentPlot.recDashed is not None :
                                self.currentPlot.recDashed.remove()
                                self.currentPlot.recDashed = None
                        elif child.get_ls() == "-" and child.get_lw() == 2:
                            if self.currentPlot.rec is not None :
                                self.currentPlot.rec.remove()
                                self.currentPlot.rec = None
        except NameError:
            raise


    def changeBkg(self):
        spectra = self.getSpectrumInfoRESTDict()
        indices = []
        geo = self.getGeo()
        for name, info in spectra.items():
            if info["dim"] == 2: 
                indices.append(i for i, x in enumerate(geo) if x == name)
        for index in indices:
            self.currentPlot.isSelected = False

        # if any(x == 2 for x in self.currentPlot.h_dim) == True:
        #     indices = [i for i, x in enumerate(self.currentPlot.h_dim) if x == 2]
        #     for index in indices:
        #         self.currentPlot.isSelected = False # this line is important for automatic conversion from dark to light and viceversa
        self.updatePlot()
        self.currentPlot.canvas.draw()

# redirect logging
class QtLogger(QObject):
    newlog = pyqtSignal(str)

    def __init__(self, parent):
        super(QtLogger, self).__init__(parent)


class TabPopup(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)

        self.lineedit = QLineEdit(self)
        self.okButton = QPushButton("Ok", self)
        self.cancelButton = QPushButton("Cancel", self)

        layButt = QHBoxLayout()
        layButt.addWidget(self.okButton)
        layButt.addWidget(self.cancelButton)

        layout = QVBoxLayout()
        layout.addWidget(self.lineedit)
        layout.addLayout(layButt)
        self.setLayout(layout)

