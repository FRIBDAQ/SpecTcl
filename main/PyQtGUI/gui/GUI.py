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
import matplotlib.colors as colors
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

import matplotlib.text as mtext

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
from MenuAndConfigGUI import * #All widgets of the top menu section
from SpecialFunctionsGUI import SpecialFunctions # all the extra functions we defined
from OutputGUI import OutputPopup # popup output window
from PlotGUI import Plot # area defined for the histograms
from PlotGUI import Tabs # area defined for the Tabs
from PyREST import PyREST # class interface for SpecTcl REST plugin
from CopyPropertiesGUI import CopyProperties
from connectConfigGUI import ConnectConfiguration #class for the connection configuration popup
from MenuGate import MenuGate #class for the gate creation/edition popup

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

        # cutoff editing popup
        self.cutoffp = cutoffPopup()

        # connection configuration windows
        self.connectConfig = ConnectConfiguration()

        # gate window
        self.gatePopup = MenuGate()

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

        #dictionary used in case user wants to have one color per gate {gateName: color}
        self.gateColor = {}
        #dictionary used to know the annotation positions to prevent overlay {spectrumName: lowestPosition}
        self.gateAnnotation = {}

        # Bool set by extra options -> differentiate gates 
        # self.annotateGate = False

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
        self.gatePopup.ok.clicked.connect(self.okGate)
        self.gatePopup.cancel.clicked.connect(self.cancelGate)
        self.gatePopup.gateActionCreate.clicked.connect(self.createGate)
        self.gatePopup.gateActionEdit.clicked.connect(self.editGate)

        # self.wConf.editGate.setToolTip("Key bindings for Modify->Edit:\n"
        #                               "'i' insert vertex\n"
        #                               "'d' delete vertex\n")

        self.wConf.integrateGate.clicked.connect(self.integrate)


        self.tabp.okButton.clicked.connect(self.okTab)
        self.tabp.cancelButton.clicked.connect(self.cancelTab)

        self.cutoffp.okButton.clicked.connect(self.okCutoff)
        self.cutoffp.cancelButton.clicked.connect(self.cancelCutoff)
        self.cutoffp.resetButton.clicked.connect(lambda: self.resetCutoff(True))

        # zoom callback
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.toolbar.actions()[1].triggered.connect(self.zoomCallback)

        # copy properties
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)
        # summing region
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.clicked.connect(self.createSRegion)
        # self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.setEnabled(False)
        # autoscale
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.autoScaleAxisBox)
        # plus button
        self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomInOut("in"))
        # minus button
        self.wTab.wPlot[self.wTab.currentIndex()].minusButton.clicked.connect(lambda: self.zoomInOut("out"))
        # cutoff button
        self.wTab.wPlot[self.wTab.currentIndex()].cutoffButton.clicked.connect(self.cutoffButtonCallback)
        self.wTab.wPlot[self.wTab.currentIndex()].cutoffButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].cutoffButton.customContextMenuRequested.connect(self.cutoff_handle_right_click)
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

        self.extraPopup.options.gateAnnotation.clicked.connect(self.gateAnnotationCallBack)
        self.extraPopup.options.gateHide.clicked.connect(self.updatePlot)


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
        # self.wConf.listGate.installEventFilter(self)

        # Hotkeys
        # zoom (click-drag)
        self.shortcutZoomDrag = QShortcut(QKeySequence("Alt+Z"), self)
        self.shortcutZoomDrag.activated.connect(self.zoomKeyCallback)


        self.currentPlot = self.wTab.wPlot[self.wTab.currentIndex()] # definition of current plot

    ################################
    # 3) Implementation of Signals
    ################################

    #So that signals work for each tab, called in clickedTab()
    def bindDynamicSignal(self):
        for index, val in self.wTab.countClickTab.items():
            if val:
                self.wTab.wPlot[index].logButton.disconnect()
                self.wTab.wPlot[index].cutoffButton.disconnect()
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
        self.wTab.wPlot[self.wTab.currentIndex()].cutoffButton.clicked.connect(self.cutoffButtonCallback)
        self.wTab.wPlot[self.wTab.currentIndex()].cutoffButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].cutoffButton.customContextMenuRequested.connect(self.cutoff_handle_right_click)
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.clicked.connect(lambda: self.customHomeButtonCallback(self.currentPlot.selected_plot_index))
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].customHomeButton.customContextMenuRequested.connect(self.handle_right_click)
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.clicked.connect(lambda: self.logButtonCallback(self.currentPlot.selected_plot_index))
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.wTab.wPlot[self.wTab.currentIndex()].logButton.customContextMenuRequested.connect(self.log_handle_right_click)

        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.clicked.connect(self.createSRegion)
        # self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.setEnabled(False)

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
        if (obj == self.wConf.histo_list or self.gatePopup.gateNameList) and event.type() == QtCore.QEvent.HoverEnter:
            self.onHovered(obj)
        return super(MainWindow, self).eventFilter(obj, event)


    def onHovered(self, obj):
        if (obj == self.wConf.histo_list):
            self.wConf.histo_list.setToolTip(self.wConf.histo_list.currentText())
        elif (obj == self.gatePopup.gateNameList):
            self.gatePopup.gateNameList.setToolTip(self.gatePopup.gateNameList.currentText())


    #Display information when hover spectrum
    def histoHover(self, event):
        try:
            index = 0
            if not event.inaxes: return

            index = list(self.currentPlot.figure.axes).index(event.inaxes)
            xTitle = self.getSpectrumInfoREST("parameters", index=index)[0]
            coordinates = self.getPointerInfo(event, "coordinates", index)
            type = self.getSpectrumInfoREST("type", index=index)
            if self.getSpectrumInfoREST("dim", index=index) == 1:
                if type == "g1" :
                    xTitle = self.getSpectrumInfoREST("parameters", index=index)[0] + ", ..."
                self.currentPlot.histoLabel.setText("Spectrum: "+self.nameFromIndex(index)+"\nX: "+xTitle)
                self.currentPlot.pointerLabel.setText(f"Pointer:\nX: {coordinates[0]:.2f} Y: {coordinates[1]:.0f} Count: {coordinates[2]:.0f}")
            elif self.getSpectrumInfoREST("dim", index=index) == 2:
                yTitle = self.getSpectrumInfoREST("parameters", index=index)[1]
                if type == "g2" or type == "m2" or type == "gd":
                    xTitle = self.getSpectrumInfoREST("parameters", index=index)[0] + ", ..."
                    yTitle = self.getSpectrumInfoREST("parameters", index=index)[1] + ", ..."
                self.currentPlot.histoLabel.setText("Spectrum: "+self.nameFromIndex(index)+"\nX: "+xTitle+" Y: "+yTitle) 
                self.currentPlot.pointerLabel.setText(f"Pointer:\nX: {coordinates[0]:.2f} Y: {coordinates[1]:.2f}  Count: {coordinates[2]:.0f}")        
                if type == "s" :
                    xTitle = self.getSpectrumInfoREST("parameters", index=index)[0] + ", ..."
                    self.currentPlot.histoLabel.setText("Spectrum: "+self.nameFromIndex(index)+"\nX: "+xTitle) 
                    self.currentPlot.pointerLabel.setText(f"Pointer:\nX: {coordinates[0]:.2f} Y: {coordinates[1]:.2f}  Count: {coordinates[2]:.0f}") 
            gateName = self.getAppliedGateName(index=index)
            if gateName is not None:
                self.currentPlot.gateLabel.setText("Gate applied: "+gateName+"\n") 
            else :
                self.currentPlot.gateLabel.setText("Gate applied: \n") 

        except:
            pass


    #called in histoHover, return the bin position under mouse pointer
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
                    count = data[binminx+1:binminx+2]
                    # y = data[binminx:binminx+1]
                    result = [x,y,count[0]]
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


    #Hotkey triggering toolbar zoom action 
    def zoomKeyCallback(self):
        self.currentPlot.canvas.toolbar.actions()[1].triggered.emit()
        self.currentPlot.canvas.toolbar.actions()[1].setChecked(True)


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
        # print("Simon - mousePressEvent - ",self.currentPlot.zoomPress,self.currentPlot.canvas.toolbar.actions()[1].isChecked())
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


    #callback for button_press_event
    def on_press(self, event):
        # print("Simon - on_press - ",self.currentPlot.zoomPress,self.currentPlot.canvas.toolbar.actions()[1].isChecked())
        #if initate zoom (magnifying glass) but dont press in axes, reset the action
        if self.currentPlot.zoomPress and not event.inaxes: 
            self.currentPlot.canvas.toolbar.actions()[1].triggered.emit()
            self.currentPlot.canvas.toolbar.actions()[1].setChecked(False)
            self.currentPlot.zoomPress = False

        if not event.inaxes: return

        # if gatePopup exit with [X], want to reset everything as if gateEditor hasn't been openned
        if (self.currentPlot.toCreateGate or self.currentPlot.toEditGate) and not self.gatePopup.isVisible():
            self.cancelGate()

        index = list(self.currentPlot.figure.axes).index(event.inaxes)
        if self.currentPlot.isEnlarged:
            index = self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]
        self.currentPlot.selected_plot_index = index
        #removed the timer on thread, cause problem for populating gatePoint in the gate popup
        #Dont know (Simon) why a small delay was needed
        if event.dblclick:
            if self.currentPlot.toCreateGate or self.currentPlot.toCreateSRegion :
                #dont need to do anything special here, the 2d line is closed when click okGate
                # self.on_dblclick_gate(event, index)
                pass
            elif self.currentPlot.toEditGate:
                self.on_dblclick_gate_edit(event, index)
            else :
                self.on_dblclick(index)
        else :
            if self.currentPlot.toCreateGate or self.currentPlot.toCreateSRegion :
                #1: left mouse button, 3: right mouse button
                if event.button == 1:
                    self.on_singleclick_gate(event, index)
                if event.button == 3:
                    self.on_singleclick_gate_right(index)
            elif self.currentPlot.toEditGate:
                self.on_singleclick_gate_edit(event, index)
            else :
                self.on_singleclick(index)


    #not used for now
    # def on_dblclick_gate(self, event, index):
    #     pass


    #called by on_press when not in create/edit Gate mode
    def on_singleclick(self, index):
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
        # similar to log button, cutoff button change status according to spectrum info
        cutoffVal = self.getSpectrumInfo("cutoff", index=index)
        if cutoffVal is not None and len(cutoffVal)>1 and (cutoffVal[0] is not None or cutoffVal[1] is not None):
            wPlot.cutoffButton.setDown(True)
        else :
            wPlot.cutoffButton.setDown(False)
            self.resetCutoff(False)

        # If we are not zooming on one histogram we can select one histogram
        # and a red rectangle will contour the plot
        if self.currentPlot.isEnlarged == False:
            if (DEBUG):
                print("Inside on_singleclick - Enlarge false")
            self.removeRectangle()
            self.currentPlot.isSelected = True
            self.currentPlot.next_plot_index = self.currentPlot.selected_plot_index
            self.currentPlot.rec = self.createRectangle(self.currentPlot.figure.axes[index])
            #tried to blit here but not successful (?) important delay for canvas with many plots
            self.currentPlot.canvas.draw()


    # find the closest bin edge position to the input position
    def closestBinPos(self, index=None, x=None, y=None):
        result = None 
        if index is None :
            return result

        dim = self.getSpectrumInfoREST("dim", index=index)
        minx = self.getSpectrumInfoREST("minx", index=index)
        maxx = self.getSpectrumInfoREST("maxx", index=index)
        binx = self.getSpectrumInfoREST("binx", index=index)
        stepx = (float(maxx)-float(minx))/float(binx)

        if dim == 1 and x is not None:
            # to round int essential, will give the closest bin edge
            nXbin = round((x-minx)/stepx, 0)
            xbinPos = minx + nXbin*stepx
            result = xbinPos
            # print("Simon - closestBinPos - dim1 - ", minx, maxx, stepx, nXbin, xbinPos )
        if dim == 2 and x is not None and y is not None:
            miny = self.getSpectrumInfoREST("miny", index=index)
            maxy = self.getSpectrumInfoREST("maxy", index=index)
            biny = self.getSpectrumInfoREST("biny", index=index)
            stepy = (float(maxy)-float(miny))/float(biny)
            # to round int essential, will give the closest bin edge
            nXbin = round((x-minx)/stepx, 0)
            xbinPos = minx + nXbin*stepx
            nYbin = round((y-miny)/stepy, 0)
            ybinPos = miny + nYbin*stepy
            result = xbinPos, ybinPos
        else:
            pass

        return result 


    #called by on_press when create gate mode
    #add new 2DLines in ax and set text points of gatePopup
    def on_singleclick_gate(self, event, index):
        if self.currentPlot.isEnlarged == True:
            dim = self.getSpectrumInfoREST("dim", index=index)
            gateType = self.gatePopup.listGateType.currentText()
            if dim == 1:
                #second arg = 0 not used for 1d
                #l = self.addLine(self.closestBinPos(index, float(event.xdata)), 0, index)
                l = self.addLine(float(event.xdata), 0, index)
                self.gatePopup.listGateLine.append(l)
                # removes the lines n-1 (n is the latest) from the plot
                if len(self.gatePopup.listGateLine) > 2:
                    self.removePrevLine()
                
                lineText = ""
                for nbLine in range(len(self.gatePopup.listGateLine)):
                    if nbLine == 0:
                        lineText = lineText + (f"{nbLine}: X= {self.gatePopup.listGateLine[nbLine].get_xdata()[0]:.5f}") 
                    else :
                        lineText = lineText + (f"\n{nbLine}: X= {self.gatePopup.listGateLine[nbLine].get_xdata()[0]:.5f}") 
                self.gatePopup.gatePoint.clear()
                self.gatePopup.gatePoint.insertPlainText(lineText)

            elif dim == 2:
                #If exist, remove line between the last and first points of the contour (closing_segment), except for band gate type
                tempLine = [line for line in self.gatePopup.listGateLine if line.get_label() is "closing_segment"]
                if gateType not in ["b", "gb"] and len(tempLine) == 1 :
                    tempLine[0].remove()
                    self.gatePopup.listGateLine.pop()

                # Add a new line based on clicked location
                #xBinned, yBinned= self.closestBinPos(index, float(event.xdata), float(event.ydata))
                #l = self.addLine(xBinned, yBinned, index)
                #xBinned, yBinned= self.closestBinPos(index, float(event.xdata), float(event.ydata))
                l = self.addLine(float(event.xdata), float(event.ydata), index)
                if l is not None :
                    self.gatePopup.listGateLine.append(l)

                # Add text in gatePopup
                lineNb = len(self.gatePopup.listGateLine)
                lineText = ""
                for nbLine in range(lineNb):
                    if nbLine == 0:
                            lineText = lineText + (f"{nbLine}: X= {self.gatePopup.listGateLine[nbLine].get_xdata()[0]:.5f}   Y= {self.gatePopup.listGateLine[nbLine].get_ydata()[0]:.5f}") 
                    else :
                        lineText = lineText + (f"\n{nbLine}: X= {self.gatePopup.listGateLine[nbLine].get_xdata()[0]:.5f}   Y= {self.gatePopup.listGateLine[nbLine].get_ydata()[0]:.5f}") 
                if lineNb == 0 :
                    lineText = lineText + (f"{lineNb}: X= {float(event.xdata):.5f}   Y= {float(event.ydata):.5f}") 
                else :
                    lineText = lineText + (f"\n{lineNb}: X= {float(event.xdata):.5f}   Y= {float(event.ydata):.5f}") 
                self.gatePopup.gatePoint.clear()
                self.gatePopup.gatePoint.insertPlainText(lineText)

                # Close contour, draw a line between the last and first points, except for band gate type
                if gateType not in ["b", "gb"] and lineNb > 1 :
                    # Special label to identify this segment elsewhere in the code
                    label = "closing_segment"
                    #xBinned, yBinned= self.closestBinPos(index, self.gatePopup.listGateLine[0].get_xdata()[0], self.gatePopup.listGateLine[0].get_ydata()[0])
                    #l = self.addLine(xBinned, yBinned, index, label)
                    l = self.addLine(self.gatePopup.listGateLine[0].get_xdata()[0], self.gatePopup.listGateLine[0].get_ydata()[0], index, label)
                    if l is not None :
                        self.gatePopup.listGateLine.append(l)

                # for line in self.gatePopup.listGateLine:
                #     print("Simon on_singleclick_gate - line -", line, line.get_label())

            self.currentPlot.canvas.draw()


    # called by on_press, right click when creating a gate is removing the last point and so change contour lines and update point text in gatePopup
    def on_singleclick_gate_right(self, index):
        if self.currentPlot.isEnlarged == True:
            
            dim = self.getSpectrumInfoREST("dim", index=index)
            gateType = self.gatePopup.listGateType.currentText()
            gateTypeList1 = ["c", "gc"]
            gateTypeList2 = ["b"]
            
            if dim == 2:
                if gateType in gateTypeList1:
                    lineNb = len(self.gatePopup.listGateLine) 
                    # Reset prevPoint for next new line to first point of line n-1 
                    self.gatePopup.prevPoint = [self.gatePopup.listGateLine[-2].get_xdata()[0], self.gatePopup.listGateLine[-2].get_ydata()[0]]
                    # if contour has only 3 lines, remove the two last lines 
                    if lineNb == 3 :
                        for i in range(2):
                            self.gatePopup.listGateLine[-1].remove()
                            self.gatePopup.listGateLine.pop(-1)
                    # save n-2 line, remove line n-1 and change first point of the closing_segment to last point of saved n-2 line
                    elif lineNb > 3:
                        tempLine = self.gatePopup.listGateLine[-3]
                        self.gatePopup.listGateLine[-2].remove()
                        self.gatePopup.listGateLine.pop(-2)
                        self.gatePopup.listGateLine[-1].set_xdata([tempLine.get_xdata()[1], self.gatePopup.listGateLine[0].get_xdata()[0]]) 
                        self.gatePopup.listGateLine[-1].set_ydata([tempLine.get_ydata()[1], self.gatePopup.listGateLine[0].get_ydata()[0]]) 
                
                # For band type we dont want to reconnect the extremities
                if gateType in gateTypeList2:
                    lineNb = len(self.gatePopup.listGateLine) 
                    # remove the last line
                    if lineNb >= 2 :                   
                        # Reset prevPoint for next new line to last point of line n-2
                        self.gatePopup.prevPoint = [self.gatePopup.listGateLine[-2].get_xdata()[1], self.gatePopup.listGateLine[-2].get_ydata()[1]]
                        self.gatePopup.listGateLine[-1].remove()
                        self.gatePopup.listGateLine.pop(-1)

                # Add text in gatePopup
                lineNb = len(self.gatePopup.listGateLine)
                lineText = ""
                for nbLine in range(lineNb):
                    if nbLine == 0:
                            lineText = lineText + (f"{nbLine}: X= {self.gatePopup.listGateLine[nbLine].get_xdata()[0]:.5f}   Y= {self.gatePopup.listGateLine[nbLine].get_ydata()[0]:.5f}") 
                    else :
                        lineText = lineText + (f"\n{nbLine}: X= {self.gatePopup.listGateLine[nbLine].get_xdata()[0]:.5f}   Y= {self.gatePopup.listGateLine[nbLine].get_ydata()[0]:.5f}") 
                if lineNb == 1 :
                    lineText = lineText + (f"\n{lineNb}: X= {self.gatePopup.listGateLine[0].get_xdata()[1]:.5f}   Y= {self.gatePopup.listGateLine[0].get_ydata()[1]:.5f}") 
                self.gatePopup.gatePoint.clear()
                self.gatePopup.gatePoint.insertPlainText(lineText)

            self.currentPlot.canvas.draw()


    #called by on_press when not in ceate/edit gate mode
    def on_dblclick(self, idx):
        if (DEBUG):
            print("Inside on_dblclick in tab", self.wTab.currentIndex())
        name = self.nameFromIndex(idx)
        index = self.wConf.histo_list.findText(name)
        self.wConf.histo_list.setCurrentIndex(index)
        if (DEBUG):
            print("UPDATE plot the histogram at index", idx, "with name", self.wConf.histo_list.currentText())

        if self.currentPlot.isEnlarged == False: # entering enlarged mode
            if (DEBUG):
                print("###### Entering enlarged mode...")
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
            if (DEBUG):
                print("inside dblclick: self.selected_plot_index",idx )
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

            if (DEBUG):
                print("##### Exiting zooming mode...")
            # disabling gate creation
            self.currentPlot.createSRegion.setEnabled(False)
            self.wConf.createGate.setEnabled(False)

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

            #draw back the original canvas
            self.currentPlot.InitializeCanvas(canvasLayout[0], canvasLayout[1], False)
            n = canvasLayout[0]*canvasLayout[1]
            self.currentPlot.h_setup = {k: True for k in range(n)}
            self.currentPlot.selected_plot_index = None # this will allow to call drawGate and loop over all the gates
            for index, name in self.getGeo().items():
                if name is not None and name != "" and name != "empty":
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
                    self.drawGateTest(index)
            #drawing back the dashed red rectangle on the unenlarged spectrum
            self.removeRectangle()
            self.currentPlot.recDashed = self.createDashedRectangle(self.currentPlot.figure.axes[tempIdxEnlargedSpectrum])
            #self.updatePlot() #replaced by the content of updatePlot in the above for loop (avoid looping twice)
            self.currentPlot.figure.tight_layout()
            # self.drawAllGates()
            self.currentPlot.canvas.draw()


    #obselete
    # def get_key(self, val):
    #     return next((key for key, value in self.currentPlot.h_dict.items() if any(val == value2 for value2 in value.values())), None)


    #callback for exitButton
    def closeAll(self):
        self.restThreadFlag = False
        self.close()


    def doubleclickedTab(self, index):
        #For now, if change tab while working on gate, close any ongoing gate action
        if self.currentPlot.toCreateGate or self.currentPlot.toEditGate or self.gatePopup.isVisible():
            self.cancelGate()

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

        #For now, if change tab while working on gate, close any ongoing gate action
        if self.currentPlot.toCreateGate or self.currentPlot.toEditGate or self.gatePopup.isVisible():
            self.cancelGate()
            return

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

            #enable/disable some widgets depending if enlarge mode of not, flags set also in on_dblclick 
            self.wConf.histo_geo_add.setEnabled(not self.currentPlot.isEnlarged)
            self.wConf.histo_geo_row.setEnabled(not self.currentPlot.isEnlarged)
            self.wConf.histo_geo_col.setEnabled(not self.currentPlot.isEnlarged)
            self.currentPlot.createSRegion.setEnabled(self.currentPlot.isEnlarged)
            self.wConf.createGate.setEnabled(self.currentPlot.isEnlarged)
            self.removeRectangle()
            self.bindDynamicSignal()
        except:
            pass


    #set geometry of the canvas
    def setCanvasLayout(self):
        indexTab = self.wTab.currentIndex()
        nRow = int(self.wConf.histo_geo_row.currentText())
        nCol = int(self.wConf.histo_geo_col.currentText())
        self.wTab.layout[indexTab] = [nRow, nCol]
        self.wTab.wPlot[indexTab].InitializeCanvas(nRow, nCol)
        self.wTab.selected_plot_index_bak[indexTab] = None
        self.currentPlot.selected_plot_index = None
        self.currentPlot.next_plot_index = -1



    ###############################################
    # 5) Connection to REST for gates
    ##
    #############################################


    #callback for the configuration popup of REST/MIRROR ports
    def connectPopup(self):
        self.connectConfig.show()


    #callback to close configuration popup
    def closeConnect(self):
        self.connectConfig.close()


    #callback to attempt a connection to REST/MIRROR
    def okConnect(self):
        self.connectShMem()
        self.closeConnect()

    #called in okGate, if was adding a new gate the pushed gate is gatePopup.listGateLine
    #if was editing a gate, the pushed gate is based on the gatePopup.gatePoint text
    def pushGateToREST(self, gateName, gateType):
        if gateName is None or gateName == "None" :
            print("Warning - pushGateToREST - gateName is None, no push to ReST, please choose a gate name")
            return

        dim = self.getSpectrumInfo("dim", index=self.gatePopup.gateSpectrumIndex)
        parameters = self.getSpectrumInfoREST("parameters", index=self.gatePopup.gateSpectrumIndex)
        spectrumType = self.getSpectrumInfoREST("type", index=self.gatePopup.gateSpectrumIndex)

        # special case for gd the spectrum parameters do not match with the gate definition
        if spectrumType == "gd" :
            parametersFormat = []
            for item in parameters:
                pars = item.split(' ')
                if len(pars) == 2:
                    parametersFormat.append(pars[0])
                    parametersFormat.append(pars[1])
            parameters = parametersFormat

        # special case for m2 (passed as a + gate of sub gates), parameters are sub gates name made on pairs of spectrum parameters
        if spectrumType == "m2" :
            parametersFormat = {}
            if len(parameters) % 2 == 0:
                pass
            else:
                #discard last parameter if not even count (possible case?)
                parameters = parameters[:-1]
            countPar = 0
            for ipar in range(0, len(parameters), 2):
                par1 = parameters[ipar]
                par2 = parameters[ipar + 1]
                dumNum1 = str(10 + countPar + 1)
                dumNum2 = str(10 + countPar + 2)
                countPar += 2
                nameSubGate = "_" + gateName + "." + dumNum1 + "." + dumNum2 + ".000"
                parametersFormat[nameSubGate] = [par1, par2]
        if len(parameters) == 0:
            return

        boundaries = []
        # set boundaries from the point text in gatePopup 
        if self.currentPlot.toEditGate:
            #points for 1d: [x0, x1] and for 2d:  [[x0, y0], [x1, y1], ...]
            points = self.formatGatePopupPointText(dim)
            if dim == 1:
                boundaries = points 
            elif dim == 2:
                for point in points:
                    boundaries.append({"x": point[0], "y": point[1]})
        else:
            if len(self.gatePopup.listGateLine) == 0:
                return
            if dim == 1:
                if gateType in ["s", "gs"] :
                    # gate 1Dgate_xamine s {aris.db1.ppac0.uc {1392.232056 1665.277466}}
                    boundaries = [self.gatePopup.listGateLine[0].get_xdata()[0],self.gatePopup.listGateLine[1].get_xdata()[0]]
                    # sort such that lowest first
                    if boundaries[0] > boundaries[1]:
                        boundaries.sort()
                        print("Warning - pushGateToREST 1d - found boundaries[0] > boundaries[1] so sorted boundaries")
            else:
                #{'2Dgate_xamine': {'name': '2Dgate_xamine', 'type': 'c',
                # 'parameters': ['aris.tof.tdc.db3scin_to_db5scin', 'aris.db5.pin.dE'],
                # 'points': [{'x': 126.876877, 'y': 29.429428}, {'x': 125.625626, 'y': 25.825825},
                #            {'x': 126.626625, 'y': 22.522522}, {'x': 129.879883, 'y': 22.522522}, {'x': 130.63063, 'y': 26.126125}, {'x': 129.629623, 'y': 29.129128}]
                #}}
                for iline, line in enumerate(self.gatePopup.listGateLine):
                    if line.get_label() is not "closing_segment":
                        #Add both points of the first line to boundaries
                        if iline == 0 :
                            for ipoint in range(2):
                                boundaries.append({"x": line.get_xdata()[ipoint], "y": line.get_ydata()[ipoint]})
                        #Add only the last point to boundaries
                        else :
                            boundaries.append({"x": line.get_xdata()[1], "y": line.get_ydata()[1]})

         # special case for m2 (passed as a + gate of sub gates), parameters are sub gates name made on pairs of spectrum parameters
        if spectrumType == "m2" :
            #push individual gates
            for name, par in parametersFormat.items():
                self.rest.createGate(name, gateType, par, boundaries)
            #push the combined gate 
            subGateNameList = []
            for subGateName in parametersFormat.keys() :
                subGateNameList.append(subGateName)
            self.rest.createGate(gateName, "+", subGateNameList, None)
        else :
            self.rest.createGate(gateName, gateType, parameters, boundaries)


    # format gatePopup.gatePoint into (1d) [x0, x1, ...] or (2d) [[x0, y0], [x1, y1], ...]
    def formatGatePopupPointText(self, dim):
        points = []
        pointDict = {}
        #first split when new lines
        textBlockLines = self.gatePopup.gatePoint.toPlainText().split("\n")
        #loop on each line
        for line in textBlockLines:
            #pass on blank line
            if not line :
                continue
            #get first number from left in the lines 
            pointId = re.search(r'\d+', line)
            if not pointId:
                print("Warning -formatGatePopupPointText - Expect a point number before X (and Y) at beginning of a line")
                return None
            #pointId must be unique
            if int(pointId.group()) in pointDict.keys():
                print("Warning -formatGatePopupPointText - Expect unique point number : ",int(pointId.group()))
                return None 
            pointId = int(pointId.group())
            #get first number between X and Y (X position) and first number from Y (Y position)
            posX = re.search(r'X(\s*[=]\s*)([-+]?(?:\d*\.*\d+))', line)
            if dim == 1 and not posX:
                print("Warning -formatGatePopupPointText - 1d spectrum - Line format is: i: X=f1 where i is integer and f1, f2 floats (no sci notation)")
                return None 
            posX = float(posX.group(2))
            pointDict[pointId] = posX

            posY = re.search(r'Y(\s*[=]\s*)([-+]?(?:\d*\.*\d+))', line)
            if dim == 2 and not posY:
                print("Warning -formatGatePopupPointText - 2d spectrum - Line format is: i: X=f1 Y=f2 where i is integer and f1, f2 floats (no sci notation)")
                return None 
            elif dim == 2 and posY:
                posY = float(posY.group(2))
                pointDict[pointId] = [posX, posY]

        #Order dict with pointId, so that pointId doesn't have to follow line index
        pointDict = {key:pointDict[key] for key in sorted(pointDict)}
        #Fill points with the format used in pushGateToRest
        if dim == 1:
            #Take only the two last values
            points = [val for key, val in pointDict.items()]
            points = points[-2:]
        elif dim == 2:
            for point in pointDict.values():
                points.append([point[0], point[1]])

        return points


    #Start ReST traces on a separated thread
    #Check for traces updates (poll) periodically, with period time < retention time.
    def restThread(self, retention):
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
                    self.updateSpectrumList()
                elif action == "add" and name not in self.getSpectrumInfoRESTDict():
                    #get spectrum info from ReST but not the data, need to query shmem for that (connect button).
                    #dont need to check the binding, given the definition of this trace...
                    info = self.rest.listSpectrum(name)
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
                    self.updateSpectrumList()
                else:
                    return 


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
            if key in ("name", "dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type", "log", "minz", "maxz", "spectrum", "axis", "cutoff") and index is not None:
                if index not in self.wTab.spectrum_dict[self.wTab.currentIndex()]:
                    print("setSpectrumInfo -",name,"not in spectrum_dict")
                    return
                    # self.wTab.spectrum_dict[self.wTab.currentIndex()][name] = {"dim":[],"binx":[],"minx":[],"maxx":[],"biny":[],"miny":[],"maxy":[],"data":[],"parameters":[],"type":[],"log":[],"minz":[],"maxz":[]}
                self.wTab.spectrum_dict[self.wTab.currentIndex()][index][key] = value
                # print("Simon - setSpectrumInfo -",index, self.wTab.spectrum_dict[self.wTab.currentIndex()], self.wTab.spectrum_dict[self.wTab.currentIndex()][index])
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
        if index is not None and index in self.wTab.spectrum_dict[self.wTab.currentIndex()] and info[0] in ("name", "dim", "binx", "minx", "maxx", "biny", "miny", "maxy", "data", "parameters", "type", "log", "minz", "maxz", "spectrum", "axis", "cutoff"):
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
            if key in self.wTab.spectrum_dict[self.wTab.currentIndex()]:
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
            self.wTab.spectrum_dict[self.wTab.currentIndex()][index] = {"name":[], "dim":[],"binx":[],"minx":[],"maxx":[],"biny":[],"miny":[],"maxy":[],"data":[],"parameters":[],"type":[],"log":[],"minz":[],"maxz":[], "spectrum":[], "axis":[], "cutoff":[]}
        self.wTab.spectrum_dict[self.wTab.currentIndex()][index]["name"] = name
        #Initialize with the same info than in spectrum_dict_rest.
        for key, value in self.spectrum_dict_rest[name].items():
            self.wTab.spectrum_dict[self.wTab.currentIndex()][index][key] = value


    #returns h_dict_geo {key=index, value=histoName}
    #Use only this function to get the geometry dict, to know the name at index there is nameFromIndex
    def getGeo(self):
        return self.currentPlot.h_dict_geo


    #attempt to use blit more generically (maybe for future)
    # def saveCanvasBkg(self, axis, spectrum, index):
    #     self.currentPlot.axbkg[index] = self.currentPlot.figure.canvas.copy_from_bbox(axis.bbox)
    #     # axis.draw_artist(spectrum)
    #     # self.currentPlot.figure.canvas.blit(axis.bbox)

    # def restoreCanvasBkg(self, axis, spectrum, index):
    #     axis.clear()
    #     self.currentPlot.figure.canvas.restore_region(self.currentPlot.axbkg[index])
    #     axis.draw_artist(spectrum)
    #     self.currentPlot.figure.canvas.blit(axis.bbox)
    #     self.currentPlot.figure.canvas.flush_events()

        
    def setEnlargedSpectrum(self, index, name):
        self.wTab.zoomPlotInfo[self.wTab.currentIndex()] = None 
        if index is not None and name is not None: 
            self.wTab.zoomPlotInfo[self.wTab.currentIndex()] = [index, name]


    def getEnlargedSpectrum(self):
        result = None
        if self.wTab.zoomPlotInfo[self.wTab.currentIndex()] :
            result = self.wTab.zoomPlotInfo[self.wTab.currentIndex()]
        return result


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


    # update spectrum list for GUI, the widget histo_list is set only here
    def updateSpectrumList(self, init=False):
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


    #About the gates: unlike spectrum, there is no internal gate dictionnary 
    #which means everytime one gets/sets gate info, one uses the ReST interface, like for the name here:
    #return the gate name applied to a spectrum (identified by index or name)
    def getAppliedGateName(self, **identifier):
        spectrumName = None
        if "index" in identifier:
            spectrumName = self.nameFromIndex(identifier["index"])
        elif "name" in identifier:
            spectrumName = identifier["name"]
        else:
            print("getAppliedGateName - wrong identifier - expects name=histo_name or index=histo_index")
            return
        gate = self.rest.applylistgate(spectrumName)
        if gate is None:
            return 
        #gate is a list with one dictionary [{'spectrum': 'spectrumName', 'gate': 'gateName'}]
        gateName = gate[0]["gate"]
        # print("Simon - getAppliedGateName -",spectrumName,gateName)
        if gateName == "-TRUE-" or gateName == "-Ungated-":
            return None 
        else :
            return gateName


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
            # updateSpectrumList has True flag/arg only here, to define searchable list
            self.updateSpectrumList(True)

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
                for index, val_dict in infoGeo["geo"].items():
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
                    print("Warning - LoadGeo - definition not found for: ",notFound)

                self.currentPlot.isLoaded = True
                self.wTab.selected_plot_index_bak[self.wTab.currentIndex()] = None
                self.currentPlot.selected_plot_index = None
                self.currentPlot.next_plot_index = -1

            self.addPlot()
            self.updatePlot()
            self.currentPlot.isLoaded = False
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


    #############################
    # 8) Zoom/Scaling operations
    #############################

    # can sets x, y scales for 1d and x, y, z scales for 2d depending on the scale identifier and if axisIsLog
    # basically do all the scaling operations
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
                    #getMinMaxInRange returns only max for 1d 
                    ymax = self.getMinMaxInRange(index, xmin=xmin, xmax=xmax)
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
                if spectrum is None :
                    return
                if (not zmin or zmin is None or zmin==0) and (not zmax or zmax is None or zmax==0):
                    zmin = self.minZ
                    zmax = self.maxZ
                if axisIsAutoScale:
                    #search in the current view
                    xmin, xmax = ax.get_xlim()
                    ymin, ymax = ax.get_ylim()
                    #getMinMaxInRange returns min and max for 2d 
                    zmin, zmax = self.getMinMaxInRange(index, xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax)
                    self.setSpectrumInfo(maxz=zmax, index=index)
                    self.setSpectrumInfo(minz=zmin, index=index)
                spectrum.set_clim(vmin=zmin, vmax=zmax)
                if axisIsLog:
                    self.setCmapNorm("log", index)
                else :
                    #linearCentered not used so far but could be user choice 
                    #while testing linearCentered noticed that it is not very compatible with cutoff
                    #self.setCmapNorm("linearCentered", index)
                    self.setCmapNorm("linear", index)
                self.setSpectrumInfo(spectrum=spectrum, index=index)


    # Where is defined the color bar
    def setCmapNorm(self, scale, index):
        validScales = ["linear", "log", "linearCentered"]
        if scale not in validScales or index is None:
            return
        spectrum = self.getSpectrumInfo("spectrum", index=index)
        zmin, zmax = spectrum.get_clim()

        if scale is validScales[0]:
            spectrum.set_norm(colors.Normalize(vmin=zmin, vmax=zmax))
        elif scale is validScales[1]:
            if zmin and zmin <= 0 :
                zmin = 0.001
                print("setCmapNorm - warning - LogNorm with zmin<=0, may want to use CenteredNorm")
            spectrum.set_norm(colors.LogNorm(vmin=zmin, vmax=zmax))
        elif scale is validScales[2]:
            palette = copy(plt.cm.jet)
            palette.set_bad(color='white')
            data = self.getSpectrumInfo("data", index=index)
            spectrum.set_cmap(palette)
            # set vcenter to variable set by user
            spectrum.set_norm(centeredNorm(data,50000))
        if self.getEnlargedSpectrum() is None:
            ax = spectrum.axes
            self.removeCb(ax)
            divider = make_axes_locatable(ax)
            cax = divider.append_axes('right', size='5%', pad=0.05)
            self.currentPlot.figure.colorbar(spectrum, cax=cax, orientation='vertical')


    #Callback for plusButton/minusButton
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
        self.drawGateTest(index)
        self.currentPlot.canvas.draw()


    # Callback for histo_autoscale, calls setAxisScale
    def autoScaleAxisBox(self):
        if (DEBUG):
            print("autoScaleAxisBox in tab:", self.wTab.currentIndex(), "with name", self.wTab.tabText(self.wTab.currentIndex()))
        try:
            ax = None
            if self.currentPlot.isEnlarged:
                if (DEBUG):
                    print("inside isEnlarged")
                ax = self.getSpectrumInfo("axis", index=0)
                dim = self.getSpectrumInfoREST("dim", index=0)
                if ax is None :
                    return
                #Set y for 1D and z for 2D.
                #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                if dim == 1:
                    self.setAxisScale(ax, 0, "y")
                elif dim == 2:
                    self.setAxisScale(ax, 0, "z")
                #draw gate if there is one
                self.drawGateTest(0)
            else:
                for index, name in self.getGeo().items():
                    if name:
                        ax = self.getSpectrumInfo("axis", index=index)
                        dim = self.getSpectrumInfoREST("dim", index=index)
                        if ax is None :
                            return
                        #Set y for 1D and z for 2D.
                        #dont need to specify if log scale, it is checked inside setAxisScale, if 2D histo in log its z axis is set too.
                        if dim == 1:
                            self.setAxisScale(ax, index, "y")
                        elif dim == 2:
                            self.setAxisScale(ax, index, "z")
                        #draw gate if there is one
                        self.drawGateTest(index)
            self.currentPlot.canvas.draw()
        except:
            pass


    # get data max within user defined range
    # For 2D have to give two ranges (x,y), for 1D range x.
    def getMinMaxInRange(self, index, **limits):
        result = None
        if not limits :
            print("getMinMaxInRange - limits identifier not valid - expect xmin=val, xmax=val etc. for y with 2D")
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
        # data = self.getSpectrumInfoREST("data", index=index)
        data = self.getSpectrumInfo("data", index=index)
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
            #Dont increase max by 10% here...
            #truncData = data[binminy:binmaxy+1, binminx:binmaxx+1]
            #Following two lines work for "small" array, replaced by custom function
            #maximum = truncData.max()
            #minimum = np.min(truncData[np.nonzero(truncData)])
            # minimum, maximum = self.customMinMax(data, binminy, binmaxy, binminx, binmaxx)
            minimum, maximum = self.customMinMax(data[binminy:binmaxy+1, binminx:binmaxx+1])
            result = minimum, maximum
        return result


    # Have seen malloc error if data array too large
    # Divide data array in sub-arrays with sub-(min, max) and then find the global-(min, max)
    def customMinMax(self, data):
        minimum = None
        maximum = None
        nbCol = data.shape[1] 
        nbRow = data.shape[0] 

        if len(data) <= 0 :
            return minimum, maximum
        #Arbitrarily choose number max of col and row of 200...
        if nbCol < 200 and nbRow < 200:
            maximum = data.max()
            minimum = np.min(data[np.nonzero(data)])
            return minimum, maximum
        else :
            stepX = nbCol if nbCol < 200 else 200
            stepY = nbRow if nbRow < 200 else 200
            rangeX = list(range(0, data.shape[1], stepX))
            rangeY = list(range(0, data.shape[0], stepY))
            subMax = []
            subMin = []
            yprev = data.shape[0]+1
            xprev = data.shape[1]+1
            for x in rangeX[::-1]:
                for y in rangeY[::-1]:
                    subData = data[y:yprev,x:xprev]
                    nonZeroIndices = np.where(subData > 0)
                    filteredSubData = subData[nonZeroIndices]
                    if filteredSubData is not None and filteredSubData.size > 0:
                        subMax.append(filteredSubData.max())
                        subMin.append(filteredSubData.min())
                    yprev = y
                xprev = x
            if len(subMin) == 0:
                minimum = self.minZ
            if len(subMax) == 0:
                minimum = self.maxZ
            elif len(subMin)>0 and len(subMax)>0 :
                minimum = min(subMin)
                maximum = max(subMax)

            return minimum, maximum


    #return the axis limits in a certain format [[xmin, xmax], [ymin, ymax]]
    def getAxisProperties(self, index):
        if (DEBUG):
            print("Inside getAxisProperties")
        try:
            ax = None
            ax = self.getSpectrumInfo("axis", index=index)
            return list(ax.get_xlim()), list(ax.get_ylim())
        except:
            pass
            

    def zoomCallback(self, event):
        self.currentPlot.zoomPress = True


    #Used by customHome button, reset the axis limits to ReST definitions, for the specified plot at index or for all plots if index not provided
    def customHomeButtonCallback(self, index=None):
        if (DEBUG):
            print("Inside customHomeButtonCallback", index)

        index_list = [idx for idx, name in self.getGeo().items() if index is None]
        if index is not None:
            index_list = [index]
        for idx in index_list:
            ax = None
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
                #getMinMaxInRange gives only min for 1d
                ymax = self.getMinMaxInRange(idx, xmin=xmin, xmax=xmax)
                ax.set_ylim(ymin, ymax)
                if self.getSpectrumInfo("log", index=idx) :
                    ax.set_yscale("linear")
            # y limits should be known at this point for both cases 1D/2D
            if dim == 2:
                ax.set_ylim(ymin, ymax)  
                #getMinMaxInRange gives min and max for 2d
                zmin, zmax = self.getMinMaxInRange(idx, xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax)
                spectrum.set_clim(vmin=zmin, vmax=zmax)
                self.setCmapNorm("linear", idx)
                self.setSpectrumInfo(maxz=zmax, index=idx)
                self.setSpectrumInfo(minz=zmin, index=idx)
            self.drawGateTest(idx)
                
            self.setSpectrumInfo(log=None, index=idx)
            self.setSpectrumInfo(minx=xmin, index=idx)
            self.setSpectrumInfo(maxx=xmax, index=idx)
            self.setSpectrumInfo(miny=ymin, index=idx)
            self.setSpectrumInfo(maxy=ymax, index=idx)
            self.setSpectrumInfo(spectrum=spectrum, index=idx)
        self.currentPlot.canvas.draw()


    #Used by logButton, defines the log scale, for the specified plot at index or for all plots if logAll/unlogAll, calls setAxisScale
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



    #callback when right click on customHomeButton
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


    #callback when right click on logButton
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


    def cutoff_handle_right_click(self):
        pass 


    #button of the cutoff window, sets the cutoff values in the spectrum dict
    def okCutoff(self):
        index = self.currentPlot.selected_plot_index
        if index is None : return 

        cutoffVal = [None, None]
        cutoffMin = self.cutoffp.lineeditMin.text()
        cutoffMax = self.cutoffp.lineeditMax.text()
        #check expected format and save cutoff in spectrum dict 
        if self.cutoffp.lineeditMin.text() != "" and self.cutoffp.lineeditMin.text().isdigit():
            cutoffVal[0] = float(cutoffMin)
            self.setSpectrumInfo(cutoff=cutoffVal, index=index)
        if self.cutoffp.lineeditMax.text() != "" and self.cutoffp.lineeditMax.text().isdigit():
            cutoffVal[1] = float(cutoffMax)
            self.setSpectrumInfo(cutoff=cutoffVal, index=index)
        #Order may be inverted
        if cutoffVal[0] is not None and cutoffVal[1] is not None and cutoffVal[1] < cutoffVal[0]:
            cutoffVal = [cutoffVal[1], cutoffVal[0]]
            self.setSpectrumInfo(cutoff=cutoffVal, index=index)
            print("okCutoff - Warning - cutoff values swapped because min > max")
        self.updatePlot()
        self.cutoffp.close()


    def cancelCutoff(self):
        self.cutoffp.close()


    def resetCutoff(self, doUpdate):
        index = self.currentPlot.selected_plot_index
        if index is None : return 
        cutoffVal = [None, None]
        self.setSpectrumInfo(cutoff=cutoffVal, index=index)
        if doUpdate:
            self.updatePlot()
        self.cutoffp.close()



    #called by cutoffButton, sets the information in the cutoff window
    def cutoffButtonCallback(self, *arg):
        index = self.currentPlot.selected_plot_index
        name = self.nameFromIndex(index)
        if name is not None : 
            self.cutoffp.setWindowTitle("Set cutoff for: " + name)
        else :
            self.cutoffp.setWindowTitle("Set cutoff for: ???" )
        self.cutoffp.setGeometry(300,100,300,100)
        if self.cutoffp.isVisible():
            self.cutoffp.close()
        self.cutoffp.show()

        if self.getSpectrumInfo("cutoff", index=index) is not None and len(self.getSpectrumInfo("cutoff", index=index)) > 0:
            cutoffMin, cutoffMax = self.getSpectrumInfo("cutoff", index=index)
            self.cutoffp.lineeditMin.setText(f"{cutoffMin}")
            self.cutoffp.lineeditMax.setText(f"{cutoffMax}")

        #try to deal with all cases to inform user
        if index is None:
            self.cutoffp.lineeditMin.setText("Select spectrum")
            self.cutoffp.lineeditMax.setText("Select spectrum") 
            return
        if self.getSpectrumInfo("cutoff", index=index) is None:
            self.cutoffp.lineeditMin.setText("None")
            self.cutoffp.lineeditMax.setText("None")
        elif self.getSpectrumInfo("cutoff", index=index) is not None and len(self.getSpectrumInfo("cutoff", index=index)) == 0:
            self.cutoffp.lineeditMin.setText("None")
            self.cutoffp.lineeditMax.setText("None")
        elif self.getSpectrumInfo("cutoff", index=index) is not None and len(self.getSpectrumInfo("cutoff", index=index)) > 0:
            cutoffMin, cutoffMax = self.getSpectrumInfo("cutoff", index=index)
            self.cutoffp.lineeditMin.setText(f"{cutoffMin}")
            self.cutoffp.lineeditMax.setText(f"{cutoffMax}")
        else :
            print("cutoffButtonCallback - warning - only one cutoff value in spectrum dict: ", self.getSpectrumInfo("cutoff", index=index))


    #Used in zoomCallBack to save the new axis limits
    #sleepTime is a small delay to ensure this function is executed after on_release
    #seems necessary to get the updated axis limits (zoom toolbar action ends on_release)
    def updatePlotLimits(self, sleepTime=0):
        # update currentPlot limits with what's on the actual plot
        ax = None
        index = self.currentPlot.selected_plot_index
        ax = self.getSpectrumInfo("axis", index=index)
        if ax is None : return
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
            spectrum = self.getSpectrumInfo("spectrum", index=index)
            spectrum.axes.set_xlim(x_range[0], x_range[1])
            spectrum.axes.set_ylim(y_range[0], y_range[1])
            self.setSpectrumInfo(spectrum=spectrum, index=index)
            self.drawGateTest(index)
        except NameError as err:
            print(err)
            pass


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


    # select axes based on indexing, used only in add()
    def select_plot(self, index):
        for i, axis in enumerate(self.currentPlot.figure.axes):
            # retrieve the subplot from the click
            if (i == index and axis is not None):
                return axis


    # returns position in grid based on indexing
    def plotPosition(self, index):
        cntr = 0
        # convert index to position in geometry
        canvasLayout = self.wTab.layout[self.wTab.currentIndex()]
        for i in range(canvasLayout[0]):
            for j in range(canvasLayout[1]):
                if index == cntr:
                    return i, j
                else:
                    cntr += 1


    # setup histogram limits according to the ReST info
    # called in add(), when the plot is first added
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

            if self.getSpectrumInfo("cutoff", index=index) is not None:
                if len(self.getSpectrumInfo("cutoff", index=index))>0:
                    #if min/maxCutoff mask data bellow/above the cutoff values
                    minCutoff = self.getSpectrumInfo("cutoff", index=index)[0]
                    maxCutoff = self.getSpectrumInfo("cutoff", index=index)[1]
                    if minCutoff is not None:
                        if dim == 1:
                            w = np.ma.masked_where(w < minCutoff, w)
                        if dim == 2:
                            w = np.ma.masked_where(w < minCutoff, w)
                    if maxCutoff is not None:
                        if dim == 1:
                            w = np.ma.masked_where(w < maxCutoff, w)
                        if dim == 2:
                            w = np.ma.masked_where(w < maxCutoff, w)
            #used in getMinMaxInRange to take into account also the cutoff if there is one
            self.setSpectrumInfo(data=w, index=index)

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
                    X = np.array(self.createRange(binx, minx, maxx))
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

                self.palette = copy(plt.cm.plasma)
                w = np.ma.masked_where(w == 0, w)
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
                if self.getEnlargedSpectrum() is None:
                    divider = make_axes_locatable(axis)
                    cax = divider.append_axes('right', size='5%', pad=0.05)
                    self.currentPlot.figure.colorbar(spectrum, cax=cax, orientation='vertical')


    # geometrically add plots to the right place and calls plotting
    # should be called only by addPlot and on_dblclick when entering/exiting enlarged mode
    def add(self, index):
        a = None
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


    # Callback for histo_geo_add button
    # also called in loadGeo
    # geometrically add plots to the right place
    # plot axis as defined in the ReST interface.
    def addPlot(self):
        if (DEBUG):
            print("Inside addPlot")
            print("check tab ",self.wTab.currentIndex(),len(self.wTab)-1,len(self.wTab.selected_plot_index_bak))

        if self.wConf.histo_list.count() == 0 : 
            QMessageBox.about(self, "Warning", 'Please click on "Connection" and fill in the information')

        try:
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

                #Reset cutoff
                self.setSpectrumInfo(cutoff=None, index=index)

                self.setGeo(index, name)
                self.currentPlot.h_limits[index] = {}
                self.currentPlot.h_setup[index] = True
                self.add(index)
                #When add with button "add" the default state is unlog
                self.currentPlot.logButton.setDown(False)
                #Reset log
                self.setSpectrumInfo(log=False, index=index)

                #draw gates
                self.drawGateTest(index)

                #draw dashed red rectangle to indicate where the next plot would be added, based on next_plot_index, selected_plot_index is unchanged.
                #recDashed added only here
                self.removeRectangle()
                self.currentPlot.recDashed = self.createDashedRectangle(self.currentPlot.figure.axes[self.currentPlot.next_plot_index])

                self.currentPlot.figure.tight_layout()
                self.currentPlot.canvas.draw()
                self.currentPlot.isSelected = False
        except NameError:
            raise


    #why not using np.linspace(vmin, vmax, bins)
    def createRange(self, bins, vmin, vmax):
        x = []
        step = (float(vmax)-float(vmin))/float(bins)
        for i in np.arange(float(vmin), float(vmax), step):
            x.append(i + step)
        x.insert(0, float(vmin))
        return x


    # fill spectrum with new data
    # called in addPlot and updatePlot
    # dont actually draw the plot in this function
    def plotPlot(self, axis, index):
        currentPlot = self.currentPlot
        if (DEBUG):
            print("Inside plotPlot")
            print("verification tab index", self.wTab.currentIndex())

        # Use spectrumInfoREST dont want to change the resolution etc.
        dim = self.getSpectrumInfoREST("dim", index=index)
        minx = self.getSpectrumInfoREST("minx", index=index)
        maxx = self.getSpectrumInfoREST("maxx", index=index)
        binx = self.getSpectrumInfoREST("binx", index=index)
        spectrum = self.getSpectrumInfo("spectrum", index=index)
        w = self.getSpectrumInfoREST("data", index=index)

        if self.getSpectrumInfo("cutoff", index=index) is not None:
            if len(self.getSpectrumInfo("cutoff", index=index))>0:
                #if min/maxCutoff mask data bellow/above the cutoff values
                minCutoff = self.getSpectrumInfo("cutoff", index=index)[0]
                maxCutoff = self.getSpectrumInfo("cutoff", index=index)[1]
                if minCutoff is not None:
                    if dim == 1:
                        w = np.ma.masked_where(w < minCutoff, w)
                    if dim == 2:
                        w = np.ma.masked_where(w < minCutoff, w)
                if maxCutoff is not None:
                    if dim == 1:
                        w = np.ma.masked_where(w > maxCutoff, w)
                    if dim == 2:
                        w = np.ma.masked_where(w > maxCutoff, w)
        if w is None or len(w) <= 0:
            return
        #used in getMinMaxInRange to take into account also the cutoff if there is one
        self.setSpectrumInfo(data=w, index=index)

        if dim == 1:
            if (DEBUG):
                print("1d case..")
            X = np.array(self.createRange(binx, minx, maxx))
            spectrum.set_data(X, w)
        else:
            if (DEBUG):
                print("2d case..")
            # color modes for later...
            # if (self.wConf.button2D_option.currentText() == 'Light'):
            w = np.ma.masked_where(w == 0, w)
            spectrum.set_data(w)
        self.setSpectrumInfo(spectrum=spectrum, index=index)
        self.currentPlot = currentPlot


    #Callback for histo_geo_update button
    #also used in various functions
    #redraw plot spectrum, update axis scales, redraw gates
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

        # if gatePopup exit with [X]
        if (self.currentPlot.toCreateGate or self.currentPlot.toEditGate) and not self.gatePopup.isVisible():
            self.cancelGate()

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
                #draw gate if there is one
                self.drawGateTest(0)
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
                    #draw gate if there is one
                    self.drawGateTest(index)

            self.currentPlot.figure.tight_layout()
            self.currentPlot.canvas.draw()
        except NameError:
            pass
            #raise


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


    #To avoid None plot index
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


    #go to next index, used in addPlot, so that one can add spectrum without selecting everytime the pad where to draw
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


    #called in nextIndex
    def forNextIndex(self, tabIndex, index):
        self.currentPlot.index = index
        self.currentPlot.selected_plot_index = index
        self.wTab.selected_plot_index_bak[tabIndex]= self.currentPlot.selected_plot_index
        self.currentPlot.next_plot_index = self.setIndex(index)

    #called in nextIndex and forNextIndex
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


    #callback for copyAttr.okAttr
    def okCopy(self):
        if (DEBUG):
            print("Inside okCopy")
        self.applyCopy()
        self.closeCopy()

    #callback for copyAttr.applyAttr
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
                if instance.text() not in discard and instance.isChecked():
                    labelPos = self.copyAttr.copy_log.labelForField(instance)
                    #following gives [row, col]
                    geoPositionSpectrum = [int(i) for i in labelPos.text().split() if i.isdigit()]
                    indexSpectrum = int(self.wConf.histo_geo_col.currentText())*geoPositionSpectrum[0]+geoPositionSpectrum[1]
                    indexes.append(indexSpectrum)

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
                    self.setSpectrumInfo(minz=zlim_src[0], index=index)
                    self.setSpectrumInfo(maxz=zlim_src[1], index=index)
            self.updatePlot()
        except:
            pass


    #callback for copyAttr.cancelAttr
    def closeCopy(self):
        discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
        for instance in self.copyAttr.findChildren(QPushButton):
            if instance.text() not in discard:
                instance.deleteLater()

        self.copyAttr.close()


    #open copy properties popup
    def copyPopup(self):
        if self.copyAttr.isVisible():
            self.copyAttr.close()
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
            zmin, zmax = spectrum.get_clim()
            self.copyAttr.histoScaleValueminZ.setText(f"{zmin}")
            self.copyAttr.histoScaleValuemaxZ.setText(f"{zmax}")
        self.copyAttr.axisSLabel.setText("Log" if self.getSpectrumInfo("log", index=index) else "Linear")
        xmin = self.getSpectrumInfo("minx", index=index)
        xmax = self.getSpectrumInfo("maxx", index=index)
        ymin = self.getSpectrumInfo("miny", index=index)
        ymax = self.getSpectrumInfo("maxy", index=index)
        self.copyAttr.axisLimLabelX.setText(f"[{xmin:.1f},{xmax:.1f}]")
        self.copyAttr.axisLimLabelY.setText(f"[{ymin:.1f},{ymax:.1f}]")

        #reset QFormLayout
        rowCount = self.copyAttr.copy_log.rowCount()
        for i in range(rowCount) :
            self.copyAttr.copy_log.removeRow(0)

        try:
            for idx, nameTarget in self.getGeo().items():
                if dim == self.getSpectrumInfoREST("dim", index=idx) and idx is not index:
                    instance = QPushButton(nameTarget, self)
                    instance.setCheckable(True)
                    instance.setStyleSheet('QPushButton {color: red;}')
                    row, col = self.plotPosition(idx)
                    self.copyAttr.copy_log.addRow("row: "+str(row)+" col: "+str(col), instance)
                    instance.clicked.connect(lambda state, instance=instance: self.connectCopy(instance))
        except KeyError as e:
            print(f"KeyError occured: {e}")
        self.copyAttr.show()


    #callback to change color when press spectrum button name, 
    def connectCopy(self, instance):
        if (instance.palette().color(QPalette.Text).name() == "#008000"):
            instance.setStyleSheet('QPushButton {color: red;}')
        else:
            instance.setStyleSheet('QPushButton {color: green;}')
        if (DEBUG):
            print(instance.isChecked())


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


    ##############
    # 10) Gates
    ##############


    #add to axes the gate applied to a spectrum (identified by index)
    def drawGateTest(self, index):
        spectrumName = self.nameFromIndex(index)
        spectrumType = self.getSpectrumInfoREST("type", name=spectrumName)
        dim = self.getSpectrumInfoREST("dim", name=spectrumName)
        parameters = self.getSpectrumInfoREST("parameters", name=spectrumName)
        # parameters are used to identify the gates drawable for each spectrum, for gd the parameters list in gate and spectrum definitions are not directly comparable
        if spectrumType == "gd" :
            parametersFormat = []
            for item in parameters:
                pars = item.split(' ')
                if len(pars) == 2:
                    parametersFormat.append(pars[0])
                    parametersFormat.append(pars[1])
            parameters = parametersFormat
        ax = self.getSpectrumInfo("axis", index=index)
        if ax is None:
            return

        drawableTypes = {"b": ["s"], "1": ["s"], "g1": ["gs"], "2": ["c", "b"], "g2": ["gc", "gb"], "gd": ["gc", "gb"], "m2": ["NotDefinedYet"], "s": ["NotDefinedYet"]}

        #get all gates that share the same parameters than spectrum 
        #gateList is a list of gate which is a dictionary e.g.
        #For 1D -> [{'name': 'gateName', 'type': 's', 'parameters': ['parameterName'], 'low': 0.0, 'high': 1.0}, {'name': 'gateName2'...}]
        #For 2D -> [{'name': 'gateName', 'type': 'c', 'parameters': ['parameterNameA', 'parameterNameB'], 'points': [{'x': 1, 'y': 2}, {'x': 3, 'y': 4}, {'x': 5, 'y': 6}] }, {'name': 'gateName2'...}]
        gateList = [dict for dict in self.rest.listGate() if dict["type"] in drawableTypes[spectrumType] and dict["parameters"] == parameters]
        gateListSpecial = [dict for dict in self.rest.listGate() if dict["type"] in drawableTypes[spectrumType]]

        # self.axesChilds()
        for gate in gateList:
            if dim == 1:
                xlim = [gate["low"], gate["high"]]
                ylim = ax.get_ybound()
                for iLine in range(2):
                    #define a label to find the line among axes childs and update its properties (limits, visiblility, annotate...)
                    lineLabel = "gate_-_" + gate["name"] + "_-_" + str(iLine)
                    #Remove gate if exists then redraw it
                    toRemove = [gateLine for gateLine in ax.get_children() if type(gateLine) == matplotlib.lines.Line2D and gateLine.get_label() == lineLabel]
                    # print("Simon - toRemove gateLine",toRemove)
                    # if len(toRemove) == 1:
                    #     toRemove[0].remove()
                    for lineToRemove in toRemove :
                        lineToRemove.remove()

                    if self.extraPopup.options.gateHide.isChecked():
                        continue

                    line = mlines.Line2D([xlim[iLine],xlim[iLine]],[ylim[0],ylim[1]], picker=5, color='red', label=lineLabel)

                    ax.add_artist(line)

            elif dim ==2:
                #define a label to find the line among axes childs and update its properties (limits, visiblility, annotate...)
                lineLabel = "gate_-_" + gate["name"] + "_-_"
                #Remove gate if exists then redraw it
                toRemove = [gateLine for gateLine in ax.get_children() if type(gateLine) == matplotlib.lines.Line2D and lineLabel in gateLine.get_label()]
                for lineToRemove in toRemove :
                    lineToRemove.remove()

                if self.extraPopup.options.gateHide.isChecked():
                    continue

                if spectrumType not in ["s"]:
                    xPoints = [pointDict["x"] for pointDict in gate["points"]]
                    yPoints = [pointDict["y"] for pointDict in gate["points"]]
                    # lineLabelList = [ lineLabel + "_-_" + str(iline)  for iline in range(len(xPoints)) ]
                    # append the first point to close the line2D, exept for b gate type
                    if gate["type"] not in ["b", "gb"]:
                        xPoints.append(gate["points"][0]["x"])
                        yPoints.append(gate["points"][0]["y"])

                    line = mlines.Line2D(xPoints,yPoints, picker=5, color='red', label=lineLabel)
                    ax.add_artist(line)

            if self.extraPopup.options.gateAnnotation.isChecked() and not self.extraPopup.options.gateHide.isChecked():
                self.setGateAnnotation(index,True)
            else :
                self.setGateAnnotation(index,False)


    #callback for gate annotation, calls setGateAnnotation
    def gateAnnotationCallBack(self):
        if self.extraPopup.options.gateAnnotation.isChecked():
            doAnnotate = True 
        else :
            doAnnotate = False
            
        if self.currentPlot.isEnlarged:
            self.setGateAnnotation(0,doAnnotate)
        else:
            for index, name in self.getGeo().items():
                if name:
                    self.setGateAnnotation(index,doAnnotate)
        self.currentPlot.canvas.draw()


    # set and unset gate annotation of a spectrum according to doAnnotate flag
    def setGateAnnotation(self, index, doAnnotate):
        spectrum = self.getSpectrumInfo("spectrum", index=index)
        dim = self.getSpectrumInfoREST("dim", index=index)
        if spectrum is None :
            return 
        ax = spectrum.axes

        for child in ax.get_children():
            if type(child) == matplotlib.lines.Line2D :
                label = child.get_label()
                labelSplit = label.split("_-_")
                if len(labelSplit) == 3 and labelSplit[0] == "gate":
                    gateName = labelSplit[1]
                    gateSegmentNum = labelSplit[2]
                    labelBuff = None

                    # set more explicit labels for user
                    # and a color is chosen for the gate lines
                    if dim == 1:
                        if gateSegmentNum == "0":
                            color = self.getGateColor(gateName)
                            labelBuff = gateName + "_low"
                        elif gateSegmentNum == "1":
                            labelBuff = gateName + "_high"

                        #Remove annotation if exists then will redraw it at updated coordinate if want annotation
                        toRemove = [an for an in ax.get_children() if type(an) == matplotlib.text.Annotation and an.get_text() == labelBuff]
                        if len(toRemove) == 1:
                            toRemove[0].remove()
                        
                        positionX = child.get_xdata()[0]
                        positionY = 0.95
                        offsetX = (ax.get_xlim()[1] - ax.get_xlim()[0])*0.002



                        if doAnnotate:
                            xy = self.getXYAnnotation(self.getSpectrumInfo("name", index=index), gateName, (positionX + offsetX, positionY))
                            # fist xy coordinate in data scale and second coordinate in axes fraction
                            #Need to improve the allocation of annotation position to avoid overlay of annotations
                            annotation = ax.annotate(labelBuff, xy=xy, xycoords=("data", "axes fraction"), color=color, fontsize=8, clip_on=True)
                            # renderer = self.currentPlot.canvas.get_renderer()
                            # bbox = ax.get_tightbbox(renderer)
                            # print("Simon ANNOTATION ", annotation, xy)
                            # renderer = self.currentPlot.canvas.get_renderer()
                            # annotationBbox = annotation.get_tightbbox(renderer)
                            # # ax.annotate(labelBuff, xy=(positionX + 0.05, 0.95), xycoords=("data", "axes fraction"), color=color, fontsize=8)
                            child.set_color(color)
                            # # check if overlay with other annotations, if does then shift bellow the lowest overlaying annotation (intersec in matplotlib > 3.6 would be better than contains)
                            # overlayAnnotationBboxes = [an.get_tightbbox(renderer) for an in ax.get_children() if type(an) == matplotlib.text.Annotation and annotationBbox.contains(an.get_tightbbox(renderer).get_center())]
                            # if len(overlayAnnotationBboxes) > 0:
                            #     yLowestBbox = annotationBbox.get_y()
                            #     for bbox in overlayAnnotationBboxes:
                            #         if bbox.get_y() < yLowestBbox:
                            #             yLowestBbox = bbox.get_y()
                            #     annotation.remove()
                            #     annotation = ax.annotate(labelBuff, xy=(positionX + offsetX, yLowestBbox - annotationBbox.get_height()), xycoords=("data", "data"), color=color, fontsize=8)
                        else :
                            child.set_color('red')
                        spectrum.axes = ax
                        self.setSpectrumInfo(spectrum=spectrum, index=index)
                    elif dim == 2:
                        #Skip annotation if exists
                        toRemove = [an for an in ax.get_children() if type(an) == matplotlib.text.Annotation and an.get_text() == gateName]
                        if len(toRemove) == 1:
                            toRemove[0].remove()
                            
                        if doAnnotate:
                            color = self.getGateColor(gateName)
                            positionX = child.get_xdata()[0]
                            positionY = child.get_ydata()[0]
                            offsetX = (ax.get_xlim()[1] - ax.get_xlim()[0])*0.003
                            offsetY = (ax.get_ylim()[1] - ax.get_ylim()[0])*0.003
                            # fist xy coordinate in data scale and second coordinate in axes fraction
                            #Need to improve the allocation of annotation position to avoid overlay of annotations
                            annotation = ax.annotate(gateName, xy=(positionX+offsetX, positionY+offsetY), xycoords="data", color=color, fontsize=8, clip_on=True)
                            # renderer = self.currentPlot.canvas.get_renderer()
                            # bbox = ax.get_tightbbox(renderer)
                            # print("Simon 2D ANNOTATION ", annotation, positionX,positionY)
                            # renderer = self.currentPlot.canvas.get_renderer()
                            # annotationBbox = annotation.get_tightbbox(renderer)
                            # # ax.annotate(labelBuff, xy=(positionX + 0.05, 0.95), xycoords=("data", "axes fraction"), color=color, fontsize=8)
                            child.set_color(color)
                            # # check if overlay with other annotations, if does then shift bellow the lowest overlaying annotation (intersec in matplotlib > 3.6 would be better than contains)
                            # overlayAnnotationBboxes = [an.get_tightbbox(renderer) for an in ax.get_children() if type(an) == matplotlib.text.Annotation and annotationBbox.contains(an.get_tightbbox(renderer).get_center())]
                            # if len(overlayAnnotationBboxes) > 0:
                            #     yLowestBbox = annotationBbox.get_y()
                            #     for bbox in overlayAnnotationBboxes:
                            #         if bbox.get_y() < yLowestBbox:
                            #             yLowestBbox = bbox.get_y()
                            #     annotation.remove()
                            #     annotation = ax.annotate(labelBuff, xy=(positionX + offsetX, yLowestBbox - annotationBbox.get_height()), xycoords=("data", "data"), color=color, fontsize=8)
                        else :
                            child.set_color('red')
                        spectrum.axes = ax
                        self.setSpectrumInfo(spectrum=spectrum, index=index)

    #goes with the annotation feature, associate a color to a gateName  
    def getGateColor(self, gateName):
        # 8 colors maybe enough (?) (blue and red ignored)
        colorList = ['tab:orange', 'tab:green', 'tab:purple', 'tab:brown', 'tab:pink', 'tab:gray', 'tab:olive', 'tab:cyan']
        # if more gates than colors then restart defining color from colorList[0] etc.
        nextColor = colorList[len(self.gateColor) % len(colorList)]
        if gateName not in self.gateColor:
            self.gateColor[gateName] = nextColor
            return nextColor
        else :
            return self.gateColor[gateName]


    # Need better way to assign position
    def getXYAnnotation(self, spectrumName, gateName, xy):
        if spectrumName not in self.gateAnnotation:
            self.gateAnnotation[spectrumName] = {gateName: xy}
            return xy
        else :
            for gateDict in self.gateAnnotation[spectrumName] :
                if gateName in gateDict:
                    return xy 
                else :
                    return (xy[0], xy[1]-0.05)


    #obselete but keep it here while working on its updated version
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
            pass
            # index = self.get_key(histo_name)
            # ax = self.select_plot(index)
            # if (rtype == "s" or rtype == "gs"):
            #     self.plot1DGate(ax, histo_name, region_name, region_line)
            # else:
            #     self.plot2DGate(ax, histo_name, region_name, region_line)


    #obselete but keep it here while working on its updated version
    def drawAllGates(self):
        artist_dict = self.currentPlot.artist_dict
        region_dict = self.currentPlot.region_dict
        hist_list = self.currentPlot.hist_list

        region_dict_keys = set(region_dict.keys())
        for hist in hist_list:
            if hist in region_dict_keys:
                for region_name, region_line in region_dict[hist].items():
                    self.drawRegion(hist, region_name, region_line)


    #obselete but keep it here while working on its updated version
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


    #obselete but keep it here while working on its updated version
    def createRegion(self):
        ax = plt.gca()
        self.cleanRegion()


    #obselete but keep it here while working on its updated version
    def cleanRegion(self):
        self.currentPlot.xs.clear()
        self.currentPlot.ys.clear()
        self.gatePopup.listGateLine = []


    # callback for gatePopup.ok button, do some checks and push gate to REST
    def okGate(self):
        # check if gateName already exists if yes issue warning
        gateName = self.gatePopup.gateNameList.currentText()
        gateNameList = [gate["name"] for gate in self.rest.listGate()]
        # print("Simon - in okGate ", gateNameList, gateName, self.currentPlot.toEditGate)
        if not self.currentPlot.toEditGate:
            if gateName in gateNameList:
                msgBox = QMessageBox(self)
                msgBox.setIcon(QMessageBox.Warning)
                msgBox.setWindowFlag(Qt.WindowStaysOnTopHint, True)
                msgBox.setText("Gate name already exists.")
                msgBox.setInformativeText('Do you want to overwrite "' + gateName + '" gate definition?')
                msgBox.setStandardButtons(QMessageBox.Yes | QMessageBox.Cancel)
                msgBox.setDefaultButton(QMessageBox.Cancel)
                ret = msgBox.exec()
                if ret == QMessageBox.Yes:
                    pass
                elif ret == QMessageBox.Cancel:
                    return
            #maybe better solution, the separator in gate line labels are _-_ so dont want to find that in the gate name
            elif "_-_" in gateName :
                msgBox = QMessageBox(self)
                msgBox.setIcon(QMessageBox.Warning)
                msgBox.setWindowFlag(Qt.WindowStaysOnTopHint, True)
                msgBox.setText('Gate name must not include "_-_"')
                msgBox.setStandardButtons(QMessageBox.Ok)
                msgBox.setDefaultButton(QMessageBox.Ok)
                ret = msgBox.exec()
                if ret == QMessageBox.Ok:
                    pass
        self.pushGateToREST(gateName,self.gatePopup.listGateType.currentText())
        #in the following lines removed in listGateLine to avoid double drawing, since the gate has been pushed to ReST it will be drawn from ReST info
        self.gatePopup.clearInfo()
        self.cancelGate()
        # self.currentPlot.toCreateGate = False
        # self.currentPlot.toEditGate = False
        # self.gatePopup.close()
        # self.updatePlot()


    #callback for gatePopup.cancel button
    def cancelGate(self):
        self.currentPlot.toCreateGate = False
        self.currentPlot.toEditGate = False
        # self.gatePopup.clearInfo()
        self.disconnectGateSignals()
        self.gatePopup.close()
        self.updatePlot()

    
    def disconnectGateSignals(self):
        try:
            if hasattr(self, 'gateReleaser') :
                self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.gateReleaser)
        except TypeError:
            pass
        try:
            if hasattr(self, 'gateFollower') :
                self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.gateFollower)
        except TypeError:
            pass
        try:
            if hasattr(self, 'sid') :
                self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.sid)
        except TypeError:
            pass
        try:
            if hasattr(self, 'sidGateNameListChanged') :
                self.gatePopup.gateNameList.currentTextChanged.disconnect(self.sidGateNameListChanged)
        except TypeError:
            pass
        try:
            if hasattr(self, 'sidGateTypeListChanged') :
                self.gatePopup.listGateType.currentIndexChanged.disconnect(self.sidGateTypeListChanged)
        except TypeError:
            pass
        try:
            if hasattr(self, 'gatePopupPreview') :
                self.gatePopup.preview.clicked.disconnect(self.gatePopupPreview)
        except TypeError:
            pass
        #Had issue with these two signals when multiple tab(?), use shortcutInsertGatePoint now
        # try:
        #     if hasattr(self, 'sidOnKeyPressEditGate') :
        #         self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.sidOnKeyPressEditGate)
        # except TypeError:
        #     pass
        # try:
        #     if hasattr(self, 'sidOnKeyReleaseEditGate') :
        #         self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.sidOnKeyReleaseEditGate)
        # except TypeError:
        #     pass
        try:
            if hasattr(self, 'shortcutInsertGatePoint') :
                self.shortcutInsertGatePoint.setEnabled(False)
        except TypeError:
            pass
            


    #Open dialog window to specify name/type and draw gate while this window is openned
    #The flag self.currentPlot.toCreateGate determines if by clicking one sets the gate points (in on_singleclick)
    def createGate(self):
        if (DEBUG):
            print("Inside createGate")

        #Default gate actions is gate creation
        self.gatePopup.gateActionCreate.setChecked(True)
        self.gatePopup.preview.setEnabled(False)
        #in otherOptions.py define a checkbox to have the possibility to disable gate edition
        if self.extraPopup.options.gateEditDisable.isChecked():
            self.gatePopup.gateActionEdit.setChecked(False)
            self.gatePopup.gateActionEdit.setEnabled(False)
        else:
            self.gatePopup.gateActionEdit.setEnabled(True)

        try:
            if hasattr(self, 'sidGateNameListChanged') :
                self.gatePopup.gateNameList.currentTextChanged.disconnect(self.sidGateNameListChanged)
        except TypeError:
            pass

        self.gatePopup.gateNameList.setEditable(True)
        self.gatePopup.gateNameList.setInsertPolicy(QComboBox.NoInsert)
        self.gatePopup.gateNameList.setCurrentText("None")


        if self.currentPlot.selected_plot_index is None:
            return QMessageBox.about(self,"Warning!", "Please add at least one spectrum")
        else:
            gateTypesDict = {"b": ["NotDefinedYet"], "1": ["s"], "g1": ["gs"], "2": ["c", "b"], "g2": ["gc", "gb"], "gd": ["gc", "gb"], "m2": ["c", "b"], "s": ["NotDefinedYet"]}
            spectrumType = self.getSpectrumInfoREST("type", index=self.currentPlot.selected_plot_index)
            if spectrumType is None :
                return

            self.gatePopup.clearInfo()
            self.disconnectGateSignals()

            gateTypesList = gateTypesDict[spectrumType]
            for type in gateTypesList:
                if type == "NotDefinedYet":
                    msgBox = QMessageBox(self)
                    msgBox.setIcon(QMessageBox.Warning)
                    msgBox.setWindowFlag(Qt.WindowStaysOnTopHint, True)
                    msgBox.setText('No gate type available for "' + spectrumType + '" spectrum')
                    #msgBox.setInformativeText('Check latest SpecTcl version')
                    msgBox.setStandardButtons(QMessageBox.Ok)
                    msgBox.setDefaultButton(QMessageBox.Ok)
                    ret = msgBox.exec()
                    return
                self.gatePopup.listGateType.addItem(type)

            self.currentPlot.toCreateGate = True
            self.currentPlot.toEditGate = False

            self.sidGateTypeListChanged = self.gatePopup.listGateType.currentIndexChanged.connect(self.gateTypeListChanged)

            self.gatePopup.gateSpectrumIndex = self.currentPlot.selected_plot_index
            self.gatePopup.show()


    #Draw a preview of the gate in case modified by editing the text gatePopup.gatePoint
    def onGatePopupPreview(self):
        ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
        if ax is None :
            return 
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        #points for 1d: [x0, x1] and for 2d:  [[x0, y0], [x1, y1], ...]
        points = self.formatGatePopupPointText(dim)
        #point is not None when all lines have the good format, see formatGatePopupPointText
        if points is None :
            return
        #For 1d there is multiple lines (2), find those with label
        if dim == 1:
            label = self.editThisGateLine.get_label()
            labelSplit = label.split("_-_")
            gateIdentifier = "gate_-_" + labelSplit[1] + "_-_"
            lines = [child for child in ax.get_children() if type(child) == matplotlib.lines.Line2D and gateIdentifier in child.get_label()]
            if len(lines) == len(points):
                for iline, line in enumerate(lines):
                    line.set_xdata([points[iline], points[iline]])
        elif dim == 2:
            lineX = []
            lineY = []
            for point in points:
                lineX.append(point[0])
                lineY.append(point[1])
            specialTypes = ["c", "gc"]
            if self.gatePopup.listGateType.currentText() in specialTypes: 
                #For c and gc the last point need to match the first one to close the contour
                lineX.append(points[0][0])
                lineY.append(points[0][1])
            self.editThisGateLine.set_data(lineX, lineY)
        self.currentPlot.canvas.draw()


    def checkConnections(self):
        # num_connections = 0
        # # Get the current plot canvas
        # current_canvas = self.wTab.wPlot[self.wTab.currentIndex()].canvas
        # # Get the connections list
        # connections = current_canvas.manager.connections
        # # Iterate through the connections list
        # for connection in connections:
        #     num_connections += 1
        #     print("Simon checkConnections-", num_connections, connection)

        # Check if there are any connections to the canvas
        # if self.wTab.wPlot[self.wTab.currentIndex()].canvas.figure.callbacks.callbacks:
        if self.wTab.wPlot[self.wTab.currentIndex()].canvas.callbacks.callbacks:
            print("There are connections to the canvas:")
            for event_name, callbacks_dict in self.wTab.wPlot[self.wTab.currentIndex()].canvas.callbacks.callbacks.items():
                print(f"Event: {event_name}, Callbacks: {callbacks_dict}")



    def find_callbacks(self):
        callbacks_for_main_window = []

        for event_name, callbacks_dict in self.wTab.wPlot[self.wTab.currentIndex()].canvas.callbacks.callbacks.items():
            for callback_id, callback_func in callbacks_dict.items():
                # Check if the callback function is associated with the MainWindow instance
                if hasattr(callback_func, '__self__') and callback_func.__self__ is self:
                    callbacks_for_main_window.append((event_name, callback_func))

        return callbacks_for_main_window


    #Open dialog window to edit an existing gate while this window is openned (can check and change points coordinates)
    def editGate(self):
        if (DEBUG):
            pass
        print("Inside editGate")

        self.gatePopup.gateActionCreate.setChecked(False)
        self.gatePopup.preview.setEnabled(True)


        try:
            if hasattr(self, 'sidGateTypeListChanged') :
                self.gatePopup.listGateType.currentIndexChanged.disconnect(self.sidGateTypeListChanged)
        except TypeError:
            pass

        self.sid = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('pick_event', self.clickOnGateLine)
        self.gatePopupPreview = self.gatePopup.preview.clicked.connect(self.onGatePopupPreview)

        # Hotkeys, insert and delete gate point (2d)
        # self.sidOnKeyPressEditGate = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('key_press_event', self.onKeyPressEditGate)
        # self.sidOnKeyReleaseEditGate = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('key_release_event', self.onKeyReleaseEditGate)
        #two previous lines dont work with multitab, dont understand why because do the same with pick_event and no problem with this one.
        #So use qt shortcut, no release action so reset altPressed flag in on_singleclick_gate_edit
        self.shortcutInsertGatePoint = QShortcut(QKeySequence("Alt+E"), self)
        self.shortcutInsertGatePoint.activated.connect(self.onKeyActivateEditGate)


        if self.gatePopup.gateSpectrumIndex is None:
            return QMessageBox.about(self,"Warning!", "Please add at least one spectrum")
        else:
            spectrumName = self.nameFromIndex(self.gatePopup.gateSpectrumIndex)
            dim = self.getSpectrumInfoREST("dim", name=spectrumName)
            ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
            if ax is None:
                return
            
            self.gatePopup.clearInfo()

            #set gate name list 
            gateLabels = [child.get_label() for child in ax.get_children() if isinstance(child, matplotlib.lines.Line2D) and "_-_" in child.get_label()]
            for label in gateLabels :
                if dim == 1:
                    label = label.split("_-_")
                    #two lines per gate for 1d so just take one label
                    if label[2] == '0':
                        self.gatePopup.gateNameList.addItem(label[1])
                elif dim == 2:
                    label = label.split("_-_")
                    self.gatePopup.gateNameList.addItem(label[1])
            self.gatePopup.gateNameList.setCurrentText("-- select a gate --")
            self.gatePopup.gateNameList.completer().setCompletionMode(QCompleter.PopupCompletion)
            self.gatePopup.gateNameList.completer().setFilterMode(QtCore.Qt.MatchContains)
            self.sidGateNameListChanged = self.gatePopup.gateNameList.currentTextChanged.connect(self.gateNameListChanged)

            self.currentPlot.toEditGate = True
            self.altPressed = False
            self.currentPlot.toCreateGate = False
            self.gatePopup.gatePoint.setReadOnly(False)


    #if change gate type while creating gate, reset the new gate info (name and point)
    def gateTypeListChanged(self):
        self.gatePopup.gateNameList.clear()
        self.gatePopup.gateNameList.setCurrentText("None")
        for line in self.gatePopup.listGateLine:
            line.remove()
        self.gatePopup.listGateLine.clear()
        self.gatePopup.prevPoint.clear()
        self.gatePopup.gatePoint.clear()
        self.gatePopup.gateSpectrumIndex = 0
        self.gatePopup.gateEditOption = None
        

    #change the gate points text and gate type (and line color when edit) according to gate name in the completable combo box
    def gateNameListChanged(self):
        ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
        if ax is None:
            return

        #Check that the text corresponds to an actual gate name
        gateName = self.gatePopup.gateNameList.currentText()
        gateFoundAtIdx = self.gatePopup.gateNameList.findText(gateName)
        if gateFoundAtIdx == -1 :
            self.gatePopup.gatePoint.clear()
            self.gatePopup.listGateType.clear()
            return
        
        #update points text
        gateIdentifier = "gate_-_" + gateName + "_-_"
        lines = [child for child in ax.get_children() if type(child) == matplotlib.lines.Line2D and gateIdentifier in child.get_label()]
        otherLines = [child for child in ax.get_children() if type(child) == matplotlib.lines.Line2D and gateIdentifier not in child.get_label()]

        #set listGateType
        gate = [dict for dict in self.rest.listGate() if dict["name"] == gateName]
        self.gatePopup.gateNameList.setCurrentText(gateName)
        self.gatePopup.listGateType.addItem(gate[0]["type"])

        #update text points corresponding to selected gate
        self.updateTextGatePopup(lines)

        #set lines style of the selected gate and rest of the lines
        for line in lines :
            line.set_marker(marker='o')
            line.set_color("green")
        for line in otherLines :
            line.set_marker(marker=None)
            line.set_color("red")

        self.currentPlot.canvas.draw()


    # called in on_singleclick_gate to add a gate line to the axis
    def addLine(self, posx, posy, index, label=None):
        if (DEBUG):
            print("Inside addLine", posx)
        spectrum = self.getSpectrumInfo("spectrum", index=index)
        dim = self.getSpectrumInfo("dim", index=index)
        if spectrum is None :
            return
        ax = spectrum.axes

        if dim == 1:
            ymin, ymax = ax.get_ybound()
            l = mlines.Line2D([posx,posx], [ymin,ymax], picker=5, label=label)
            ax.add_line(l)
        elif dim == 2:
            xyPrev = self.gatePopup.prevPoint
            self.gatePopup.prevPoint = [posx, posy]
            # If we are adding the closing segment we want to save the n-1 last point (prev prev) 
            # because closing segment will be removed before drawing from click
            if label == "closing_segment":
                self.gatePopup.prevPoint = xyPrev
            if xyPrev is None or len(xyPrev) == 0 :
               return
            else :
                l = mlines.Line2D([xyPrev[0],posx], [xyPrev[1],posy], picker=5, label=label)
                ax.add_line(l)
        if l is None :
            return 
        elif self.currentPlot.toCreateSRegion :
            l.set_color('b')
        else:
            l.set_color('r')
        return l


    #called in on_singleclick_gate to remove previous point by right click while making a new gate
    def removePrevLine(self):
        l = self.gatePopup.listGateLine[0]
        l.remove()
        self.gatePopup.listGateLine.pop(0)
        if len(self.currentPlot.xs)>0:
            self.currentPlot.xs.pop(0)
        if (DEBUG):
            print("after removing lines...", self.currentPlot.xs)
        

    #reset signals to continue adding/modifying gate point
    def releaseonclick(self, event):
        try:
            if hasattr(self, 'gateReleaser') :
                self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.gateReleaser)
        except TypeError:
            pass
        try:
            if hasattr(self, 'gateFollower') :
                self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.gateFollower)
        except TypeError:
            pass
        self.gatePopup.gateEditOption = None
        self.xyRef = None
        self.movingMarker = []


    # #Disable hotkey to edit a gate
    # def onKeyReleaseEditGate(self, event):
    #     #set to false as soon as the key is released
    #     self.altPressed = False


    # # Callback for hotkey to edit a gate
    # def onKeyPressEditGate(self, event):
    #     if not event.inaxes or not self.currentPlot.toEditGate:
    #         return
    #     #Used in on_singleclick_gate_edit
    #     self.altPressed = True
    #     if event.key == 'alt':
    #         self.altPressed = True

    # Callback for shortcut to edit a gate
    def onKeyActivateEditGate(self):
        if not self.currentPlot.toEditGate:
            return
        #Used in on_singleclick_gate_edit
        self.altPressed = True


    #insert a point to the edited (selected) contour (only for 2d gate), at the mouse location close to the contour
    def insertPointGate(self, event):
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
        if ax is None or dim !=2 :
            return 
        
        lineX = self.editThisGateLine.get_xdata()
        lineY = self.editThisGateLine.get_ydata()
        # coordinates in pixels for comparison with event position
        lineXY = self.editThisGateLine.get_transform().transform(self.editThisGateLine.get_xydata())

        p = event.x, event.y
        insertAt = None
        for i in range(len(lineXY) - 1 ):
            s0 = lineXY[i]
            s1 = lineXY[i+1]
            d = self.dist_point_to_segment(p, s0, s1)
            if d <= self.epsilon:
                insertAt = i
                break

        if insertAt is not None:
            lineX.insert(insertAt+1, ax.transData.inverted().transform((event.x, event.y))[0])
            lineY.insert(insertAt+1, ax.transData.inverted().transform((event.x, event.y))[1])
            self.editThisGateLine.set_data(lineX, lineY)
            self.currentPlot.canvas.draw_idle()


    #delete a point to the edited (selected) contour (only for 2d gate), at the mouse location close to the contour
    def deletePointGate(self, event):
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
        if ax is None or dim !=2 :
            return 

        lineX = self.editThisGateLine.get_xdata()
        lineY = self.editThisGateLine.get_ydata()
        markerPos = np.array([lineX, lineY])

        p = ax.transData.inverted().transform((event.x, event.y))

        # calculate the distance between each marker and the event.
        distances = np.linalg.norm(markerPos - p.reshape(2, -1), axis=0)
        dataRadius = abs(ax.transData.inverted().transform((self.epsilon, 0))[0])
        # print("Simon - deletePointGate ", markerPos, dataRadius, p, distances)

        specialGateTypes = ["c", "gc"]

        # get the marker index if marker is close to the click position.
        markerIdx = np.where(distances <= dataRadius)[0]
        if markerIdx.size == 0:
            pass
        else:
            # for contour, if remove first point, set last point to second point to keep contour closed
            if markerIdx[0] == 0 and self.gatePopup.listGateType.currentText() in specialGateTypes:
                lineX[-1] = lineX[1]
                lineY[-1] = lineY[1]
            lineX.pop(markerIdx[0])
            lineY.pop(markerIdx[0])
            self.editThisGateLine.set_data(lineX, lineY)
            dumList = [self.editThisGateLine]
            self.updateTextGatePopup(dumList)
            self.currentPlot.canvas.draw_idle()



    #change "in live" the point/line/gate position according to the mouse position
    def followmouse(self, event):
        if self.gatePopup.gateEditOption == "1d_move_line" :
            self.editThisGateLine.set_color("green")
            self.editThisGateLine.set_xdata([event.xdata, event.xdata])
        elif self.gatePopup.gateEditOption == "2d_move_all" :
            lineX = self.editThisGateLine.get_xdata()
            lineY = self.editThisGateLine.get_ydata()
            shiftX = event.xdata - lineX[0] 
            shiftY = event.ydata - lineY[0]
            #not sure why the += changes the type of lineX lineY from list to ndarray? Want to keep it a list
            # lineX += shiftX
            # lineY += shiftY
            lineX = [item + shiftX for item in lineX]
            lineY = [item + shiftY for item in lineY]
            self.editThisGateLine.set_data(lineX, lineY )
        elif self.gatePopup.gateEditOption == "2d_move_point" :
            lineX = self.editThisGateLine.get_xdata()
            lineY = self.editThisGateLine.get_ydata()
            if hasattr(self, 'movingMarker') and len(self.movingMarker) > 0:
                for mIdx in self.movingMarker:
                    lineX[mIdx] = event.xdata
                    lineY[mIdx] = event.ydata
                self.editThisGateLine.set_data(lineX, lineY )
            else :
                markerPos = np.array([lineX, lineY])
                try :
                    ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
                    if ax is None :
                        return
                    # calculate the distance between each marker and the event.
                    distances = np.linalg.norm(markerPos - self.xyRef.reshape(2, -1), axis=0)
                    dataRadius = abs(ax.transData.inverted().transform([[self.epsilon, 0]])[0, 0])
                    # get the marker index if marker is close to the click position.
                    markerIdx = np.where(distances <= dataRadius)[0]
                    if markerIdx.size == 0:
                        pass
                    else:
                        #If select the first point then one should move the last point too, 2 Idx 
                        self.movingMarker = []
                        for mIdx in markerIdx:
                            lineX[mIdx] = event.xdata
                            lineY[mIdx] = event.ydata
                            self.movingMarker.append(mIdx)
                        self.editThisGateLine.set_data(lineX, lineY )
                        self.gateReleaser = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.releaseonclick)
                except NameError:
                    raise
        else :
            pass
        self.currentPlot.canvas.draw_idle()


    #goes with hotkey to edit gate, depending if left or right click, add or delete gate point (2d)
    def on_singleclick_gate_edit(self, event, index):
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
        if ax is None:
            return
        if dim == 2:
            self.xyRef = np.array([event.xdata,event.ydata])
        #left click
        if event.button == 1 :
            if  hasattr(self, 'altPressed') and self.altPressed :
                self.insertPointGate(event)
        #right click
        if event.button == 3 :
            if hasattr(self, 'altPressed') and self.altPressed :
                self.deletePointGate(event)
            else :
                pass
            self.currentPlot.canvas.draw()
        self.altPressed = False


    #move a gate line if double click on it 
    def on_dblclick_gate_edit(self, event, index):
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        if dim == 2:
            self.gatePopup.gateEditOption = "2d_move_all"
            # self.xyRef = [event.xdata,event.ydata]
            # self.gateFollower = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.followmouse)
            self.gateReleaser = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.releaseonclick)



    #udpate the gatePopup.gatePoint text according to the gateList [line2D]
    def updateTextGatePopup(self, gateList):
        # Add text in gatePopup
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        if dim ==1 :
            lines = gateList
            lineText = ""
            for nbLine in range(len(lines)):
                if nbLine == 0:
                    lineText = lineText + (f"{nbLine}: X= {gateList[nbLine].get_xdata()[0]:.5f}") 
                else :
                    lineText = lineText + (f"\n{nbLine}: X= {gateList[nbLine].get_xdata()[0]:.5f}") 
        elif dim ==2 :
            lines = gateList[0].get_xydata()
            lineText = ""
            specialTypes = ["c", "gc"]
            nbPoints = len(lines)
            if self.gatePopup.listGateType.currentText() in specialTypes:
                #for c or gc range(lineNb-1) because point0 = pointLast
                nbPoints = len(lines)-1
            for nbLine in range(nbPoints):
                if nbLine == 0:
                    lineText = lineText + (f"{nbLine}: X= {lines[nbLine][0]:.5f}   Y= {lines[nbLine][1]:.5f}") 
                else :
                    lineText = lineText + (f"\n{nbLine}: X= {lines[nbLine][0]:.5f}   Y= {lines[nbLine][1]:.5f}") 

        self.gatePopup.gatePoint.clear()
        self.gatePopup.gatePoint.insertPlainText(lineText)


    # For gate editing when pick_event signal
    def clickOnGateLine(self, event):
        #left click
        if event.mouseevent.button != 1 :
            return 
        self.editThisGateLine = None
        # self.xyRef = [event.mouseevent.x,event.mouseevent.y]
        dim = self.getSpectrumInfoREST("dim", index=self.gatePopup.gateSpectrumIndex)
        ax = self.getSpectrumInfo("axis", index=self.gatePopup.gateSpectrumIndex)
        if ax is None :
            return
        
        #Get the line(s) from axis child
        lineCandidate = [child for child in ax.get_children() if type(child) == matplotlib.lines.Line2D and child == event.artist] 
        if len(lineCandidate) > 0:
            self.editThisGateLine = lineCandidate[0]
            if dim == 2 :
                #lineCandidate only one artist from drawGate even for 2d gates, need to decompose into segments and points
                self.editThisGateLine = lineCandidate[0]
        else :
            return
        #Following block sets the name and type in gatePopup and get the segment number to update self.gatePopup.listGateLine
        labelSplit = lineCandidate[0].get_label().split("_-_")
        if len(labelSplit) != 3 or labelSplit[0] != "gate":
            print("Warning - clickOnGateLine - lineLabel not in the expected format")
            return
        gateName = labelSplit[1]
        gate = [dict for dict in self.rest.listGate() if dict["name"] == gateName]

        self.gatePopup.gateNameList.setCurrentText(gateName)
        self.gatePopup.listGateType.clear()
        self.gatePopup.listGateType.addItem(gate[0]["type"])

        #Fill listGateLine with the axis child, in order to use pushGateToRest later in okGate
        gateIdentifier = "gate_-_" + gateName + "_-_"
        gateLines = [child for child in ax.get_children() if type(child) == matplotlib.lines.Line2D and gateIdentifier in child.get_label()]

        self.updateTextGatePopup(gateLines)
        
        #Start mouse follow signal but only effective once self.gatePopup.gateEditOption is set
        self.gateFollower = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.followmouse)
        if dim == 1:
            self.gatePopup.gateEditOption = "1d_move_line"
            self.gateReleaser = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.releaseonclick)
        elif dim ==2:
            #Note that to drag the entire gate the self.gatePopup.gateEditOption is set on_dblclick_gate_edit
            #But still it is necessary to use clickOnGateLine such that one nows which gate to drag
            self.editThisGateLine.set_marker(marker='o')
            self.editThisGateLine.set_color("green")
            self.currentPlot.canvas.draw()
            self.gatePopup.gateEditOption = "2d_move_point"


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


    ############################
    # 12)  Fitting
    ############################

    #callback to open extra function popup
    def spfunPopup(self):
        if self.extraPopup.isVisible():
            self.extraPopup.close()

        self.extraPopup.show()

    #set axis limit text of the fit in extraPopup and returns these limits
    def axisLimitsForFit(self, ax):
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


    #Main fit function, reads user defined intial parameters and calls appropriate fit model
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
                    xtmp = self.createRange(binx, minxREST, maxxREST)

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
                    xmin, xmax = self.axisLimitsForFit(ax)

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
        try:
            index = self.currentPlot.selected_plot_index
            ax = self.getSpectrumInfo("axis", index=index)
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
            xtmp = self.createRange(binx, minxREST, maxxREST)
            ytmp = (self.getSpectrumInfoREST("data", index=index)).tolist()
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
        i, j = self.plotPosition(index)
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
        rec = matplotlib.patches.Rectangle((0, 0), 1, 1, ls="-", lw=2, ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)
        return rec


    def createDashedRectangle(self, plot):
        rec = matplotlib.patches.Rectangle((0, 0), 1, 1, ls=":", lw=2, ec="red", fc="none", transform=plot.transAxes)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)
        return rec


    def removeRectangle(self):
        try:                       
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

    #for debug
    def axesChilds(self):
        try:                       
            for ax in self.currentPlot.figure.axes:
                for child in ax.get_children():
                    # print("Simon - axesChilds - ",type(child))
                    if type(child) == matplotlib.lines.Line2D :
                        print("Simon - axesChild get_c", child.get_c())
                        print("Simon - axesChild get_lw", child.get_lw())
                        print("Simon - axesChild get_ls", child.get_ls())
                        print("Simon - axesChild get_xdata:",child.get_xdata())
                        print("Simon - axesChild get_ydata:",child.get_ydata())
        except NameError:
            raise


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


class cutoffPopup(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.lineeditMin = QLineEdit(self)
        self.lineeditMax = QLineEdit(self)
        self.labelMin = QLabel(self)
        self.labelMin.setText("Min")
        self.labelMax = QLabel(self)
        self.labelMax.setText("Max")
        self.okButton = QPushButton("Ok", self)
        self.cancelButton = QPushButton("Cancel", self)
        self.resetButton = QPushButton("Reset", self)

        mainLayout = QGridLayout()
        buttonsLayout = QHBoxLayout()
        fieldsLayout = QHBoxLayout()
        fieldsLayout.addWidget(self.labelMin)
        fieldsLayout.addWidget(self.lineeditMin)
        fieldsLayout.addWidget(self.labelMax)
        fieldsLayout.addWidget(self.lineeditMax)
        buttonsLayout.addWidget(self.okButton)
        buttonsLayout.addWidget(self.resetButton)
        buttonsLayout.addWidget(self.cancelButton)

        mainLayout.addLayout(fieldsLayout, 1, 0, 1, 0)
        mainLayout.addLayout(buttonsLayout, 2, 0, 1, 0)
        self.setLayout(mainLayout)

class centeredNorm(colors.Normalize):
    def __init__(self, data, vcenter=0, halfrange=None, clip=False):
        if halfrange is None:
            halfrange = np.max(np.abs(data - vcenter))
        super().__init__(vmin=vcenter - halfrange, vmax=vcenter + halfrange, clip=clip)
