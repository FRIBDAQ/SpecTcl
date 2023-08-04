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
        self.wTab.setMovable(True)
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
        self.ctr = 0

        # extra popup window
        self.extraPopup = SpecialFunctions()

        # Tab editing popup
        self.tabp = TabPopup()

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

        # editing gates
        self.showverts = True
        self.epsilon = 5  # max pixel distance to count as a vertex hit
        self._ind = None # the active vert

        self.bPressed = False

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
        self.wTab.tabBarDoubleClicked.connect(self.doubleclickedTab)

        # config menu signals
        self.wConf.histo_geo_add.clicked.connect(self.addPlot)
        self.wConf.histo_geo_update.clicked.connect(self.updatePlot)
        self.wConf.histo_geo_delete.clicked.connect(self.clearPlot)

        self.wConf.histo_geo_row.activated.connect(lambda: self.wTab.wPlot[self.wTab.currentIndex()].InitializeCanvas(
            int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_col.activated.connect(lambda: self.wTab.wPlot[self.wTab.currentIndex()].InitializeCanvas(
            int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))

        self.wConf.createGate.clicked.connect(self.createGate)
        self.wConf.createGate.setEnabled(False)
        self.wConf.editGate.setEnabled(False)
        self.wConf.menu.triggered.connect(self.editGate)
        self.wConf.editGate.setToolTip("Key bindings for Modify->Edit:\n"
                                      "'i' insert vertex\n"
                                      "'d' delete vertex\n")
        self.wConf.cleanGate.clicked.connect(self.clearGate)
        self.wConf.drawGate.clicked.connect(self.addGate)
        self.wConf.deleteGate.clicked.connect(self.deleteGate)

        self.wConf.integrateGate.clicked.connect(self.integrate)
        self.wConf.outputGate.clicked.connect(self.resultPopup)

        self.wConf.button2D_option.activated.connect(self.changeBkg)

        self.tabp.okButton.clicked.connect(self.okTab)
        self.tabp.cancelButton.clicked.connect(self.cancelTab)

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
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.setAutoscaleAxisBox)
        # log button
        self.wTab.wPlot[self.wTab.currentIndex()].histo_log.clicked.connect(self.setLogBox)
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
        self.wTab.wPlot[self.wTab.currentIndex()].histo_autoscale.clicked.connect(self.setAutoscaleAxisBox)
        self.wTab.wPlot[self.wTab.currentIndex()].histo_log.clicked.connect(self.setLogBox)
        self.wTab.wPlot[self.wTab.currentIndex()].plusButton.clicked.connect(lambda: self.zoomIn(self.wTab.wPlot[self.wTab.currentIndex()].canvas))
        self.wTab.wPlot[self.wTab.currentIndex()].minusButton.clicked.connect(lambda: self.zoomOut(self.wTab.wPlot[self.wTab.currentIndex()].canvas))
        self.wTab.wPlot[self.wTab.currentIndex()].copyButton.clicked.connect(self.copyPopup)

        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.clicked.connect(self.createSRegion)
        self.wTab.wPlot[self.wTab.currentIndex()].createSRegion.setEnabled(False)

        self.resizeID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("resize_event", self.on_resize)
        self.pressID = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.on_press)

        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("motion_notify_event", self.histoHover)

    def connect(self):
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect("button_press_event", self.on_press)

    def disconnect(self):
        self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.pressID)

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
                #if (DEBUG):
                    #Simon - commented following because sometimes it is annoying while debugging
                    #print("Inside histoHover isZoomed")
                index = self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]
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
        index = list(self.currentPlot.figure.axes).index(event.inaxes)
        if self.currentPlot.isZoomed:
            index = self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]
        self.currentPlot.selected_plot_index = index
        if (DEBUG):
            print('Inside on_press self.currentPlot.selected_plot_index ',index)
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

            self.currentPlot.isSelected = True
            self.currentPlot.rec = self.createRectangle(self.currentPlot.figure.axes[index])

            if index in self.currentPlot.h_log:
                if self.currentPlot.h_log[index] :
                    self.wTab.wPlot[self.wTab.currentIndex()].histo_log.setChecked(True)
                else:
                    self.wTab.wPlot[self.wTab.currentIndex()].histo_log.setChecked(False)
            self.clickToName(index)

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
        if self.currentPlot.h_dict_geo[0] == "empty":
            self.currentPlot.h_dict_geo = deepcopy(self.wTab.h_dict_geo_bak[self.wTab.currentIndex()])

        name = self.currentPlot.h_dict_geo[idx]
        index = self.wConf.histo_list.findText(name)
        self.wConf.histo_list.setCurrentIndex(index)
        self.updateHistoInfo(name)
        if (DEBUG):
            print("UPDATE plot the histogram at index", idx, "with name", self.wConf.histo_list.currentText())

        if self.currentPlot.isZoomed == False: # entering zooming mode
            if (DEBUG):
                print("###### Entering zooming mode...")
            self.currentPlot.isZoomed = True
            # make sure log is correct
            if self.currentPlot.h_log[idx] == False:
                self.currentPlot.histo_log.setChecked(False)
                self.wTab.wPlot[self.wTab.currentIndex()].histo_log.setChecked(False)
            else:
                self.currentPlot.histo_log.setChecked(True)
                self.wTab.wPlot[self.wTab.currentIndex()].histo_log.setChecked(True)
            # disabling adding histograms
            self.wConf.histo_geo_add.setEnabled(False)
            # enabling gate creation
            self.currentPlot.createSRegion.setEnabled(True)
            self.wConf.createGate.setEnabled(True)
            self.wConf.editGate.setEnabled(True)
            if (DEBUG):
                print("inside dblclick: self.selected_plot_index",idx )
            # backing up list of histogram
            self.currentPlot.h_dict_bak = self.currentPlot.h_dict.copy()
            # plot corresponding histogram
            if (DEBUG):
                print("plot the histogram at index", idx, "with name", (self.currentPlot.h_dict[idx])["name"])
            self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]= deepcopy(idx)
            self.updatePlot()

            #Reset colorbar dictionary for replot when back from zoom mode
            self.currentPlot.cbar = {key: False for key, value in self.currentPlot.cbar.items()}

        else:
            # enabling adding histograms
            self.wConf.histo_geo_add.setEnabled(True)
            if self.currentPlot.toCreateGate == True or self.currentPlot.toCreateSRegion == True:
                if (DEBUG):
                    print("Fixing index before closing the gate")
                    print(idx,"has to be",self.wTab.selected_plot_index_bak[self.wTab.currentIndex()])
                    print("Before histo name", self.wConf.histo_list.currentText())
                # fixing the correct index
                if self.currentPlot.h_dict_geo[0] == "empty":
                    self.currentPlot.h_dict_geo = deepcopy(self.wTab.h_dict_geo_bak[self.wTab.currentIndex()])
                if (DEBUG):
                    print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
                name = self.currentPlot.h_dict_geo[self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]]
                index = self.wConf.histo_list.findText(name)
                if (DEBUG):
                    print("histogram",name,"has index", index)
                self.wConf.histo_list.setCurrentIndex(index)
                self.updateHistoInfo(name)
                if (DEBUG):
                    print("Now histo name", self.wConf.histo_list.currentText())
                ax = plt.gca()
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
                self.currentPlot.isZoomed = False
                # disabling gate creation
                self.currentPlot.createSRegion.setEnabled(False)
                self.wConf.createGate.setEnabled(False)
                self.wConf.editGate.setEnabled(False)
                if (DEBUG):
                    #following self.h_setup deos not work, prevent retruning to unzoomed view
                    #print("Reinitialization self.h_setup", self.h_setup)
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
        if (DEBUG):
            print("Clicked zoomCallback in tab", self.wTab.currentIndex())
        try:
            self.updatePlotLimits()

        except:
            pass

    def homeCallback(self, event):
        if (DEBUG):
            print("Clicked homeCallback in tab", self.wTab.currentIndex())
        try:
            print(self.wConf.histo_list)
            name = str(self.wConf.histo_list.currentText())
            index = self.get_key(name)
            self.resetAxisLimits(index)
            self.currentPlot.canvas.draw()
            #Simon - added
            self.currentPlot.isZoomCallback = False
            self.currentPlot.isZoomInOut = False
        except:
            pass

    def closeAll(self):
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
            self.create_gate_list()
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

        if (DEBUG):
            # print(self.currentPlot.artist_dict)
            # hname = self.wConf.histo_list.currentText()
            # select = self.spectrum_list['names'] == hname
            # df = self.spectrum_list.loc[select]
            # hdim = df.iloc[0]['dim']
            for hname in self.spectrum_list['names']:
                select = self.spectrum_list['names'] == hname
                df = self.spectrum_list.loc[select]
                hpar = df.iloc[0]['parameters']
                if hpar == gate_parameters:
                    # if name in self.currentPlot.artist_dict:
                    if gate_type == "s" or gate_type == "gs":
                        self.currentPlot.artist_dict[hname] = [self.currentPlot.artist1D]
                    else:
                        self.currentPlot.artist_dict[hname] = [self.currentPlot.artist2D]

        # adding gate to dictionary of regions
        self.currentPlot.gateTypeDict[name] = gate_type

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
            nlst = []
            nlstPara = []
            nlstType = []
            dictInfo = self.getSpectrumInfo()
            for name in s[1]:
                if name in dictInfo:
                    nlst.append(name)
                    nlstPara.append(dictInfo[name]["parameters"])
                    nlstType.append(dictInfo[name]["type"])
            self.spectrum_list = pd.DataFrame(
                {'id': s[0],
                 'names': nlst,
                 'dim' : s[2],
                 'binx': s[3],
                 'minx': s[4],
                 'maxx': s[5],
                 'biny': s[6],
                 'miny': s[7],
                 'maxy': s[8],
                 'data': s[9],
                 'parameters': nlstPara,
                 'type': nlstType}
            )

            # order the dataframe by id to avoid mismatch later on with id of new spectra
            # self.spectrum_list = self.spectrum_list.sort_values(by=['id'], ascending=True)

            if (DEBUG):
                print(self.spectrum_list)

            # update and create parameter, spectrum, and gate lists
            self.create_spectrum_list()
            self.create_parameter_list()
            self.update_spectrum_info()
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
            #except NameError:
            #raise
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started or hostname/port/mirror are not configured!")


    #get histo name, type and parameters from REST
    def getSpectrumInfo(self):
        if (DEBUG):
            print("Inside getSpectrumInfo")
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


    # create spectrum list for GUI
    def create_spectrum_list(self):
        if (DEBUG):
            print("Inside create_spectrum_list")
        # resetting ComboBox
        self.wConf.histo_list.clear()

        for name in self.spectrum_list['names']:
            if self.wConf.histo_list.findText(name) == -1:
                self.wConf.histo_list.addItem(name)

        self.wConf.histo_list.setEditable(True)
        self.wConf.histo_list.setInsertPolicy(QComboBox.NoInsert)
        self.wConf.histo_list.completer().setCompletionMode(QCompleter.PopupCompletion)
        self.wConf.histo_list.completer().setFilterMode(QtCore.Qt.MatchContains)

    # update parameter list for GUI
    def update_parameter_list(self):
        if (DEBUG):
            print("Inside update_parameter_list")
        try:
            par_dict = self.rest.listParameter()
            tmpid = [dic['id'] for dic in par_dict if 'id' in dic]
            tmpname = [dic['name'] for dic in par_dict if 'name' in dic]
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
        for key, value in self.param_list.items():
            if self.wConf.listParams[0].findText(value) == -1:
                self.wConf.listParams[0].addItem(value)
            if self.wConf.listParams[1].findText(value) == -1:
                self.wConf.listParams[1].addItem(value)

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
    def updateHistoInfo(self, hist_name):
        if (DEBUG):
            print("Inside updateHistoInfo")
            print("hist_name",hist_name)
        try:
            select = self.spectrum_list['names'] == hist_name
            df = self.spectrum_list.loc[select]
            hist_dim = df.iloc[0]['dim']
            if hist_dim == 1:
                self.wConf.button1D.setChecked(True)
            else:
                self.wConf.button2D.setChecked(True)
            self.check_histogram();
            for i in range(hist_dim):
                #Simon - moved the print in the for loop because df.iloc[0]['parameters'] size depends on hist_dim
                if (DEBUG):
                    print(hist_name, hist_dim, df.iloc[0]['parameters'][i])
                index = self.wConf.listParams[i].findText(df.iloc[0]['parameters'][i], QtCore.Qt.MatchFixedString)
                if index >= 0:
                    self.wConf.listParams[i].setCurrentIndex(index)
        except:
            pass

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

    def findGateType(self, gate_name):
        # if gate is loaded and doesn't exist then add it beforehand
        gate_type = self.currentPlot.gateTypeDict[gate_name]
        gate_index = self.wConf.listGate_type.findText(gate_type, QtCore.Qt.MatchFixedString)
        if gate_index < 0:
            gate_index = 0
        return gate_index


    def updateGateType(self):
        try:
            gate_name = self.wConf.listGate.currentText()
            gindex = self.findGateType(gate_name)
            self.wConf.listGate_type.setCurrentIndex(gindex)
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

            self.wTab.h_dict_geo_bak[self.wTab.currentIndex()] = deepcopy(self.currentPlot.h_dict_geo)
            self.wTab.h_log_bak[self.wTab.currentIndex()] = deepcopy(self.currentPlot.h_log)

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
            #Simon - commented following line because drawAllGates is called in updatePlot
            #self.drawAllGates()
            self.updateGateType()

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
    def loadAxis(self, ax, index):
        if (DEBUG):
            print("Inside loadAxis")
        if (self.currentPlot.h_dict[index]["dim"] == 1) :
            if self.currentPlot.h_log[index]:
                if (DEBUG):
                    print("time to become log")
                ymin, ymax = ax.get_ylim()
                if ymin == 0:
                    ymin = 0.001
                ax.set_ylim(ymin,ymax)
                ax.set_yscale("log")
                if (DEBUG):
                    print("at this point should be log")
            else:
                ax.set_yscale("linear")
                if (self.currentPlot.h_limits[index]):
                    ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])
                else:
                    ax.set_ylim(self.minY,self.maxY)
        else:
            if self.currentPlot.h_log[index]:
                zmin = 0
                if self.minZ == 0:
                    zmin = 0.001
                else:
                    zmin = self.minZ
                zmin = math.log10(zmin)
                zmax = math.log10(self.maxZ)
                self.currentPlot.h_lst[index].set_clim(vmin=zmin, vmax=zmax)
            else:
                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)

    # sets y scale for 1d and z scale for 2d depending on h_log flag
    def axisScale(self, ax, index):
        if (DEBUG):
            print("Inside axisScale")
            print("self.currentPlot.isLoaded", self.currentPlot.isLoaded)
        if self.currentPlot.isLoaded:
            self.loadAxis(ax, index)
        #Simon - changed the following line, self.wTab.wPlot has the size of the number of tabs not number of figures
        if self.currentPlot.h_log[index]:
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
                if self.minZ <= 0:
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
                    if len(self.currentPlot.h_limits) > index:
                        if self.currentPlot.h_limits[index]:
                            ax.set_ylim(self.currentPlot.h_limits[index]["y"][0], self.currentPlot.h_limits[index]["y"][1])
                    else:
                        ax.set_ylim(self.minY,self.maxY)
            else:
                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)


    # setting log/linear axes
    def setLogBox(self):
        #in zoom mode use selected_plot_index_bak because on_press sets selected_plot_index to 0
        # if self.currentPlot.isZoomed:
        #     selected_plot_index = self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]
        # else:
        #     selected_plot_index = self.currentPlot.selected_plot_index
        selected_plot_idx = self.currentPlot.selected_plot_index
        wPlot = self.wTab.wPlot[self.wTab.currentIndex()]

        if DEBUG:
            print("Clicked setLogBox in tab", self.wTab.currentIndex())
            print("self.currentPlot.h_log", h_log)
            print("self.currentPlot.selected_plot_index", selected_plot_idx)

        try:
            if selected_plot_idx is not None:
                if DEBUG:
                    print("histogram selected with index", selected_plot_idx)
                self.currentPlot.h_log[selected_plot_idx] = wPlot.histo_log.isChecked()
                if DEBUG:
                    scale_type = "log" if self.currentPlot.h_log[selected_plot_idx] else "linear"
                    print("histogram needs to become", scale_type)
                if self.currentPlot.isZoomed:
                    ax = plt.gca()
                else:
                    ax = self.select_plot(selected_plot_idx)

                self.axisScale(ax, selected_plot_idx)
                # self.currentPlot.canvas.draw()
                wPlot.canvas.draw()

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
        elif self.currentPlot.h_lst[index] is not None:
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

        # Simon - added following lines to avoid None plot index
        index = self.autoIndex()

        if self.currentPlot.isZoomed == False:
            for i, ax in enumerate(self.currentPlot.figure.axes):
                if (i == index):
                    self.zoom(ax, i, "in")
                    try:
                        self.currentPlot.rec.remove()
                        self.currentPlot.rec = self.createRectangle(ax)
                    except:
                        pass
        else:
            ax = plt.gca()
            self.zoom(ax, index, "in")

        #Simon - added lines - save axis limits for updates
        if index in self.currentPlot.h_limits:
            x_range, y_range = self.getAxisProperties(index)
            self.currentPlot.h_limits[index]["x"] = x_range
            self.currentPlot.h_limits[index]["y"] = y_range
            self.currentPlot.isZoomInOut = True

        canvas.draw()

    def zoomOut(self, canvas):
        if (DEBUG):
            print("Inside zoomOut")

        # Simon - added following lines to avoid None plot index
        index = self.autoIndex()

        if self.currentPlot.isZoomed == False:
            for i, ax in enumerate(self.currentPlot.figure.axes):
                if (i == index):
                    self.zoom(ax, i, "out")
                    try:
                        self.currentPlot.rec.remove()
                        self.currentPlot.rec = self.createRectangle(ax)
                    except:
                        pass
        else:
            ax = plt.gca()
            self.zoom(ax, index, "out")

        #Simon - added lines - save axis limits for updates
        if index in self.currentPlot.h_limits:
            x_range, y_range = self.getAxisProperties(index)
            self.currentPlot.h_limits[index]["x"] = x_range
            self.currentPlot.h_limits[index]["y"] = y_range
            self.currentPlot.isZoomInOut = True

        canvas.draw()


    def setAutoscaleAxisBox(self):
        if (DEBUG):
            print("setAutoscaleAxisBox in tab:", self.wTab.currentIndex(), "with name", self.wTab.tabText(self.wTab.currentIndex()))
        wPlot = self.wTab.wPlot[self.wTab.currentIndex()]
        try:
            self.currentPlot.autoScale = wPlot.histo_autoscale.isChecked()
            self.setAutoscaleAxis()
            self.currentPlot.canvas.draw()
        except:
            pass


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

                selected_plot_idx = self.currentPlot.selected_plot_index
                self.currentPlot.h_log[selected_plot_idx] = self.wTab.wPlot[self.wTab.currentIndex()].histo_log.isChecked()
                self.axisScale(ax, selected_plot_idx)
                hist_dict_idx = self.currentPlot.h_dict[selected_plot_idx]
                hist_limits_idx = self.currentPlot.h_limits[selected_plot_idx]

                # set limits if they exist
                if hist_limits_idx:
                    ax.set_xlim(hist_limits_idx["x"][0], hist_limits_idx["x"][1])
                    ax.set_ylim(hist_limits_idx["y"][0], hist_limits_idx["y"][1])
                    if hist_dict_idx["dim"] == 2:
                        zmin, zmax = self.currentPlot.h_lst[selected_plot_idx].get_clim()
                        # ax.set_clim(vmin=self.minZ, vmax=self.maxZ)
                        self.currentPlot.h_lst[selected_plot_idx].set_clim(vmin=zmin, vmax=zmax)
                else:
                    if hist_dict_idx["dim"] == 1:
                        ax.set_xlim(float(hist_dict_idx["xmin"]), float(hist_dict_idx["xmax"]))
                    else:
                        ax.set_xlim(float(hist_dict_idx["xmin"]), float(hist_dict_idx["xmax"]))
                        ax.set_ylim(float(hist_dict_idx["ymin"]), float(hist_dict_idx["ymax"]))
                        #do not self.currentPlot.h_lst[index].set_clim because already done in axisScale

                # (re)sets y or z limits according to the maximum
                if self.currentPlot.autoScale:
                    if (DEBUG):
                        print("Inside self.autoScale for tab with index", self.wTab.currentIndex())
                    if (hist_dict_idx["name"]) != "empty":
                        if (DEBUG):
                            print("histogram exists with index", selected_plot_idx)
                        data = self.get_data(selected_plot_idx)
                        if (hist_dict_idx["dim"] == 1) :
                            ymax_new = max(data)*1.1
                            ax.set_ylim((ax.get_ylim())[0], ymax_new)
                        else:
                            maxZ = np.max(data)*1.1
                            self.currentPlot.h_lst[selected_plot_idx].set_clim(vmin=self.minZ, vmax=maxZ)
            else:
                if (DEBUG):
                    print("Inside multipanel option")
                    print("self.currentPlot.autoScale", self.currentPlot.autoScale)

                hist_list = self.wTab.wPlot[self.wTab.currentIndex()].hist_list
                hist_dict = self.currentPlot.h_dict
                hist_limits = self.currentPlot.h_limits

                if (DEBUG):
                    print("Inside not self.autoScale for tab with index", self.wTab.currentIndex())
                    print(self.currentPlot.h_limits)

                #read hist_list because faster to get index than from h_dict
                for index, name in enumerate(hist_list):
                    hist_dict_idx = hist_dict[index]
                    if (hist_dict_idx["name"]) != "empty":
                        ax = self.select_plot(index)
                        self.axisScale(ax, index)
                        # set limits if they exist
                        if hist_limits[index]:
                            hist_limits_idx = hist_limits[index]
                            ax.set_xlim(hist_limits_idx["x"][0], hist_limits_idx["x"][1])
                            ax.set_ylim(hist_limits_idx["y"][0], hist_limits_idx["y"][1])
                            if hist_dict_idx["dim"] == 2:
                                zmin, zmax = self.currentPlot.h_lst[index].get_clim()
                                self.currentPlot.h_lst[index].set_clim(vmin=zmin, vmax=zmax)
                        else:
                            if hist_dict_idx["dim"] == 1:
                                ax.set_xlim(float(hist_dict_idx["xmin"]), float(hist_dict_idx["xmax"]))
                                # ax.set_ylim(self.minY, self.maxY)
                            else:
                                ax.set_xlim(float(hist_dict_idx["xmin"]), float(hist_dict_idx["xmax"]))
                                ax.set_ylim(float(hist_dict_idx["ymin"]), float(hist_dict_idx["ymax"]))
                                #do not self.currentPlot.h_lst[index].set_clim because already done in axisScale

                        # (re)sets y or z limits according to the maximum
                        if self.currentPlot.autoScale:
                            data = self.get_data(index)
                            if hist_dict_idx["dim"] == 1:
                                ymax_new = max(data)*1.1
                                ax.set_ylim((ax.get_ylim())[0], ymax_new)
                            else:
                                maxZ = np.max(data)*1.1
                                self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=maxZ)
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


    #Simon - used to keep modified axis ranges after zoomCallback unless homeCallback is pressed
    def setAxisLimits(self, index):
        if (DEBUG):
            print("Inside setAxisLimits")
            print("axes", self.currentPlot.h_dict[index])
            print("log ", self.currentPlot.h_log[index])
        ax = None
        if self.currentPlot.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(index)

        if "x" in self.currentPlot.h_limits[index] and "y" in self.currentPlot.h_limits[index]:
            x_limits = self.currentPlot.h_limits[index]["x"]
            y_limits = self.currentPlot.h_limits[index]["y"]
            if x_limits and y_limits:
                ax.set_xlim(float(x_limits[0]), float(x_limits[1]))
                ax.set_ylim(float(y_limits[0]), float(y_limits[1]))

        if self.currentPlot.h_log[index] and self.currentPlot.h_dim[index] == 1:
            self.axisScale(ax, index)

        if self.currentPlot.h_dim[index] == 2:
            self.currentPlot.h_lst[index].set_clim(vmin=self.minZ, vmax=self.maxZ)
            if self.currentPlot.h_log[index]:
                self.axisScale(ax, index)


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
        # for index, value in self.currentPlot.h_dict.items():
        #     values.append(value["name"])

        try:
            values = [value["name"] for value in self.currentPlot.h_dict.values()]
        except TypeError as err:
            print(err)
            return

        if (DEBUG):
            print(keys, values)
        if "empty" in values:
            self.currentPlot.index = keys[values.index("empty")]
        else:
            if (DEBUG):
                print("list is full, set index to full")
            self.currentPlot.index = keys[-1]
            self.currentPlot.isFull = True

        # Simon modified following lines
        if self.currentPlot.isFull:
            try:
                if self.currentPlot.index <= self.wConf.row*self.wConf.col-1:
                    if self.currentPlot.index == self.wConf.row*self.wConf.col-1:
                        self.currentPlot.index = 0
                    else:
                        self.currentPlot.index += 1
            except IndexError as e:
                print(f"An IndexError occured: {e}")

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
            self.currentPlot.cbar[index] = False
        except:
            pass

        a.clear()
        return a

    # setup histogram
    def setupPlot(self, axis, index):
        currentPlot = self.currentPlot
        if (DEBUG):
            print("Inside setupPlot")
            print("histoname -->", currentPlot.h_dict[index]["name"])
        if (DEBUG):
            print("self.currentPlot.h_dict", currentPlot.h_dict)
            print("self.currentPlot.h_dict_geo", currentPlot.h_dict_geo)
            print("self.currentPlot.h_limits",currentPlot.h_limits)
            print("self.currentPlot.h_log",currentPlot.h_log)
            print("self.currentPlot.h_setup",currentPlot.h_setup)
            print("self.currentPlot.h_dim",currentPlot.h_dim)
            print("----------------------")

        if currentPlot.h_dict_geo[index] != "empty":
            #Simon - add a line - already a plot at index (not empty) so remove it (use pop here) and then add a new one.
            #Simon - if use insert only it adds new content to the list but does not replace elements.
            currentPlot.h_lst.pop(index)
            #Simon - opti
            if len(self.wTab.wPlot[self.wTab.currentIndex()].hist_list) > index :
                self.wTab.wPlot[self.wTab.currentIndex()].hist_list.pop(index)

            if (DEBUG):
                print("not empty --> ",currentPlot.h_dict_geo)

            dim = int(currentPlot.h_dim[index])
            minx = float(currentPlot.h_dict[index]["xmin"])
            maxx = float(currentPlot.h_dict[index]["xmax"])
            binx = int(currentPlot.h_dict[index]["xbin"])

            if (DEBUG):
                print("Histo", currentPlot.h_dict_geo[index], "dim", dim, minx, maxx, binx)

            # update axis
            if dim == 1:
                if (DEBUG):
                    print("1d case...")
                axis.set_xlim(minx,maxx)
                axis.set_ylim(self.minY,self.maxY)
                # create histogram
                line, = axis.plot([], [], drawstyle='steps')
                # line.set_animated(True)
                currentPlot.h_lst.insert(index, line)
            else:
                currentPlot = self.currentPlot
                if (DEBUG):
                    print("2d case...")

                miny = currentPlot.h_dict[index]["ymin"]
                maxy = currentPlot.h_dict[index]["ymax"]
                biny = currentPlot.h_dict[index]["ybin"]

                # empty data for initialization
                # w = 0*np.random.random_sample((int(binx),int(biny)))
                w = np.zeros((int(binx), int(biny)))

                # setup up palette
                if (self.wConf.button2D_option.currentText() == 'Dark'):
                    #self.palette = 'plasma'
                    self.palette = 'plasma_r'
                else:
                    self.palette = copy(plt.cm.plasma)
                    w = np.ma.masked_where(w < 0.1, w)
                    self.palette.set_bad(color='white')

                # create histogram
                currentPlot.h_lst.insert(index, axis.imshow(w,
                                                                 interpolation='none',
                                                                 extent=[float(minx),float(maxx),float(miny),float(maxy)],
                                                                 aspect='auto',
                                                                 origin='lower',
                                                                 vmin=self.minZ, vmax=self.maxZ,
                                                                 cmap=self.palette))

            currentPlot.axbkg[index] = currentPlot.figure.canvas.copy_from_bbox(axis.bbox)
            if (DEBUG):
                print("self.currentPlot.h_lst",currentPlot.h_lst)

            self.currentPlot = currentPlot
            #Simon - here fill tab dict with gates
            self.updateHistList(index)


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
            #Simon - following if else added to avoid a=NoneType in the else "not loaded"
            a = None
            if self.currentPlot.isZoomed:
                a = plt.gca()
            else:
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

        if (DEBUG):
            print("self.currentPlot.h_setup", self.currentPlot.h_setup)
            print(self.currentPlot.selected_plot_index)


    def updateHistList(self, index):
         #Simon - work optimization
        if (DEBUG):
            print("Inside updateHistList")
        hist_name = self.currentPlot.h_dict[index]['name']

        if hist_name not in self.wTab.wPlot[self.wTab.currentIndex()].hist_list:
        # if hist_name not in self.wTab.wPlot[self.wTab.currentIndex()].hist_list:
            self.wTab.wPlot[self.wTab.currentIndex()].hist_list.insert(index, hist_name)


    # geometrically add plots to the right place
    def addPlot(self):
        if (DEBUG):
            print("Inside addPlot")
            print("Simon - tab -",self.wTab.currentIndex(),len(self.wTab)-1,len(self.wTab.selected_plot_index_bak))

        try:
            currentPlot = self.currentPlot
            # if we load the geometry from file
            if currentPlot.isLoaded:
                if (DEBUG):
                    print("Inside addPlot - loaded")
                    print(currentPlot.h_dict_geo)
                    print(currentPlot.h_dict)
                counter = 0
                for key, value in currentPlot.h_dict_geo.items():
                    if (DEBUG):
                        print("counter -->", counter)
                    index = self.wConf.histo_list.findText(value, QtCore.Qt.MatchFixedString)
                    # changing the index to the correct histogram to load
                    self.wConf.histo_list.setCurrentIndex(index)
                    self.updateHistoInfo(index)
                    if (DEBUG):
                        print(key, value, index)
                    # updating histogram dictionary with the last info needed (dim, xbinx, ybin, parameters, and type)
                    if (index != -1) :
                        currentPlot.h_dict[counter] = self.update_spectrum_info()

                    if (DEBUG):
                        print(self.update_spectrum_info())
                    counter += 1
                if (DEBUG):
                    print("updated self.currentPlot.h_dict")
                    print(currentPlot.h_dict)

                # updating support list for histogram dimension
                if len(currentPlot.h_dict) != 0:
                    currentPlot.h_dim = currentPlot.get_histo_key_list(currentPlot.h_dict, "dim")
                    if (DEBUG):
                        print("self.currentPlot.h_dim",currentPlot.h_dim)

                self.currentPlot = currentPlot
                for key, value in currentPlot.h_dict_geo.items():
                    if (DEBUG):
                        print(key, value)
                    self.add(key)
            else:
                if (DEBUG):
                    print("Inside addPlot - not loaded")
                # self adding

                #reminder - following was using self.currentPlot.index instead of index
                index = self.autoIndex()

                if (DEBUG):
                    print("Adding plot at index ", index)
                    print("self.currentPlot.h_dict", currentPlot.h_dict)
                    print("self.currentPlot.h_dict_geo", currentPlot.h_dict_geo)
                    print("self.currentPlot.h_dim", currentPlot.h_dim)

                # updating histogram dictionary for fast access to information via get_histo_xxx
                currentPlot.h_dict[index] = self.update_spectrum_info()
                if currentPlot.h_dict[index] is None:
                    return
                currentPlot.h_dict_geo[index] = (currentPlot.h_dict[index])["name"]
                currentPlot.h_dim[index] = (currentPlot.h_dict[index])["dim"]
                currentPlot.h_limits[index] = {}

                if (DEBUG):
                    print("self.currentPlot.h_dict", currentPlot.h_dict)
                    print("self.currentPlot.h_dict_geo", currentPlot.h_dict_geo)
                    print("self.currentPlot.h_dim", currentPlot.h_dim)

                currentPlot.h_setup[index] = True
                self.currentPlot = currentPlot
                self.erasePlot(index)
                #self.add(index)
                self.updateSinglePlot(index)
                # if gate in gateList:
                self.drawAllGates()
                #Simon - the following line was commented, I think it is better if one not overlay with the previous plot (?)
                self.currentPlot.canvas.draw()
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
        else :
            select = self.spectrum_list['names'] == name
            df = self.spectrum_list.loc[select]

            try:
                data = df.iloc[0]
            except:
                return

            if "data" in df.iloc[0] :
                w = df.iloc[0]['data']
            else :
                return

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
        currentPlot = self.currentPlot
        if (DEBUG):
            print("Inside plotPlot")
            print("verification tab index", self.wTab.currentIndex())

        dim = int(currentPlot.h_dim[index])
        minx = float(currentPlot.h_dict[index]["xmin"])
        maxx = float(currentPlot.h_dict[index]["xmax"])
        binx = int(currentPlot.h_dict[index]["xbin"])
        if (DEBUG):
            print(dim, minx, maxx, binx)

        if (DEBUG):
            print("self.currentPlot.h_dict", currentPlot.h_dict)
            print("self.currentPlot.h_lst", currentPlot.h_lst)

        w = self.get_data(index)
        if (DEBUG):
            print("data",sum(w))
            print(currentPlot.h_lst[index])

        if dim == 1:
            if (DEBUG):
                print("1d case..")
            X = np.array(self.create_range(binx, minx, maxx))
            if (DEBUG):
                print(len(w),len(X))
                print(type(w),type(X))
            currentPlot.h_lst[index].set_data(X, w)
        else:
            if (DEBUG):
                print("2d case..")
            if (self.wConf.button2D_option.currentText() == 'Light'):
                self.palette = copy(plt.cm.plasma)
                w = np.ma.masked_where(w < threshold, w)
                self.palette.set_bad(color='white')
                currentPlot.h_lst[index].set_cmap(self.palette)
            currentPlot.h_lst[index].set_data(w)

        currentPlot.figure.canvas.restore_region(currentPlot.axbkg[index])
        axis.draw_artist(currentPlot.h_lst[index])
        currentPlot.figure.canvas.blit(axis.bbox)

        # setup colorbar only for 2D
        if dim == 2 and not currentPlot.cbar[index]:
            divider = make_axes_locatable(axis)
            cax = divider.append_axes('right', size='5%', pad=0.05)
            # add colorbar
            currentPlot.figure.colorbar(currentPlot.h_lst[index], cax=cax, orientation='vertical')
            #set a dictionary to avoid overlaying error bars
            currentPlot.cbar[index] = True
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
            a = self.select_plot(index)
            if self.currentPlot.isZoomed:
                a = plt.gca()
            else:
                a = self.select_plot(index)
            a.clear()

        self.currentPlot.canvas.draw()


    def updatePlot(self):
        if (DEBUG):
            print("Inside updatePlot")
            print("self.currentPlot.h_dict", self.currentPlot.h_dict)
            print("self.currentPlot.h_dict_geo", self.currentPlot.h_dict_geo)
            print("self.currentPlot.h_setup", self.currentPlot.h_setup)
            print("verification tab index", self.wTab.currentIndex())

        # try:
        a = None
        #Simon - self.currentPlot.selected_plot_index replaced index from by following lines
        #Simon - because selected_plot_index comes from on_press which in zoom mode is always 0
        name = str(self.wConf.histo_list.currentText())
        index = self.autoIndex()
        if index is None:
            return
        try:
            #x_range, y_range = self.getAxisProperties(index)
            if self.currentPlot.isZoomed:
                #Simon - sca for zoom feature when multiple tabs
                plt.sca(self.currentPlot.figure.axes[0])
                if (DEBUG):
                    print("Inside updatePlot - zoomed")
                self.currentPlot.InitializeCanvas(1,1,False)
                a= plt.gca()

                if (DEBUG):
                    print("self.currentPlot.h_setup", self.currentPlot.h_setup)
                if int(self.currentPlot.h_dim[index]) == 1:
                    self.add(index)
                else:
                    self.setupPlot(a, index)

                self.currentPlot.h_setup[index] = False
                self.plotPlot(a, index)
                # self.setLogAxis()
                # This needs to be removed for gating - may we can fix it
                try:
                    self.removeCb(a)
                except:
                    pass
            else:
                # plt.sca(self.select_plot(index))
                if (DEBUG):
                    print("Inside updatePlot - multipanel mode")
                    debug_info = [(index, value) for index, value in self.currentPlot.h_dict.items()]
                    print("index, value ", debug_info)

                update_single_plot = self.updateSinglePlot

                for index, value in self.currentPlot.h_dict.items():
                    update_single_plot(index)

            self.currentPlot.figure.tight_layout()
            self.setAutoscaleAxis()
            self.drawAllGates()
            self.currentPlot.canvas.draw()
            #Simon - used to keep modified axis ranges after zoomCallback unless homeCallback is pressed
            if (self.currentPlot.isZoomCallback or self.currentPlot.isZoomInOut):
                self.setAxisLimits(index)

            return a
        except NameError:
            raise

    #Simon - added following def
    def updatePlotLimits(self):
        # update currentPlot limits with what's on the actual plot
        ax = None
        index = self.currentPlot.selected_plot_index
        name = self.currentPlot.h_dict_geo[index]

        # name = str(self.wConf.histo_list.currentText())
        # index = self.get_key(name)
        if self.currentPlot.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(index)
        if (DEBUG):
            print(ax.get_xlim(), ax.get_ylim())
            print(self.currentPlot.h_limits)
        #Simon - modified the following lines
        x_range, y_range = self.getAxisProperties(index)
        self.currentPlot.h_limits[index]["x"] = x_range
        self.currentPlot.h_limits[index]["y"] = y_range
        self.currentPlot.isZoomCallback = True
        #x_range, y_range = self.getAxisProperties(index)

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


    ##############
    # 10) Gates
    ##############

    # helper function that converts index of geometry into name of histo list and updates info
    def clickToName(self, idx):
        if (DEBUG):
            print("Inside clickToName")
            print("histo index", idx)
            print("self.currentPlot.h_dict[idx]['name']", self.currentPlot.h_dict[idx]['name'])

        try:
            name = str(self.currentPlot.h_dict[idx]['name'])
            index = self.wConf.histo_list.findText(name, QtCore.Qt.MatchFixedString)
            if (DEBUG):
                print("index and name", index, name)
            self.updateHistoInfo(name)
            self.check_histogram()
        except:
            pass

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
        if self.currentPlot.isZoomed:
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
        if self.currentPlot.isZoomed:
            ax = plt.gca()
            if (self.currentPlot.h_dict_geo[self.wTab.selected_plot_index_bak[self.wTab.currentIndex()]] == histo_name):
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
        hist_list = self.wTab.wPlot[self.wTab.currentIndex()].hist_list

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

            if self.currentPlot.isZoomed:
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
        gate_parameter = self.wConf.listParams[0].currentText()
        gateType = None

        if (name==""):
            return QMessageBox.about(self,"Warning!", "Please create at least one spectrum")
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
        gindex = self.findGateType(gate_name)
        if (DEBUG):
            print("index of the gate in combobox", gindex)
        self.wConf.listGate_type.setCurrentIndex(gindex)
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

                self.e_press = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('button_press_event', self.button_press_callback)
                self.e_motion = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('motion_notify_event', self.motion_notify_callback)
                self.e_release = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('button_release_event', self.button_release_callback)

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

            self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.sid)
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

            self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_disconnect(self.sid)
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

            self.sid = self.wTab.wPlot[self.wTab.currentIndex()].canvas.mpl_connect('pick_event', self.clickonline)

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
            ylim_src = []
            zlim_src = []
            keys = []
            values = []
            scale_src = None
            discard = ["Ok", "Cancel", "Apply", "Select all", "Deselect all"]
            # creating list of target histograms
            for instance in self.copyAttr.findChildren(QPushButton):
                for key, value in self.currentPlot.h_dict.items():
                    for key2, value2, in value.items():
                        if (instance.text() not in discard) and instance.isChecked() and instance.text() == value2 and key != self.currentPlot.selected_plot_index :
                            if (DEBUG):
                                print("histo destination",instance.text())
                            keys.append(key)
                            values.append(value2)
                    # keys=list(self.currentPlot.h_dict_geo.keys())
                    # values=list(self.currentPlot.h_dict_geo.values())

            # index_og = keys[values.index(self.wConf.histo_list.currentText())]
            if (DEBUG):
                # print(self.currentPlot.selected_plot_index, index_og)
                print(self.currentPlot.selected_plot_index)
                print(keys)
                print(values)
            # remove source element
            # keys.pop(self.currentPlot.selected_plot_index)
            # values.pop(self.currentPlot.selected_plot_index)
            # src values to copy to destination
            xlim_src = ast.literal_eval(self.copyAttr.axisLimLabelX.text())
            ylim_src = ast.literal_eval(self.copyAttr.axisLimLabelY.text())
            scale_src = self.copyAttr.axisSLabel.text()
            zlim_src = [float(self.copyAttr.histoScaleValueminZ.text()), float(self.copyAttr.histoScaleValuemaxZ.text())]
            if (DEBUG):
                print(xlim_src, ylim_src, scale_src, zlim_src)
                print(flags)
                print(self.currentPlot.h_limits)
                print(self.currentPlot.h_setup)
                print(self.currentPlot.h_log)
                print(self.minZ,self.maxZ)
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
                    self.minZ = zlim_src[0]
                    self.maxZ = zlim_src[1]
                    self.currentPlot.h_setup[index] = True

            if (DEBUG):
                print("before applying to destination...")
                print(self.currentPlot.h_limits)
                print(self.currentPlot.h_setup)
                print(self.currentPlot.h_log)
                print(self.minZ,self.maxZ)

            # ax = None
            if not self.currentPlot.isZoomed:
                for index in range(len(self.currentPlot.h_setup)):
                    # match dimension of the selected histogram (1d/2d)
                    if self.currentPlot.h_dim[index] == dim and self.currentPlot.h_setup[index]:
                        self.setAxisLimits(index)

                        # # select axes
                        # ax = self.select_plot(index)
                        # # modifying axis limits
                        # ax.set_xlim(xlim_src[0], xlim_src[1])
                        # ax.set_ylim(ylim_src[0], ylim_src[1])
                        # # modifying log/linear
                        # if self.currentPlot.h_log[index]:
                        #     self.axisScale(ax, index)
                        # # for 2D plot sets limits
                        # if dim == 2:
                        #     self.currentPlot.h_lst[index].set_clim(vmin=zlim_src[0], vmax=zlim_src[1])
                        # self.currentPlot.canvas.draw()

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
        try:
            self.updatePlotLimits()
        except:
            return
        index = self.currentPlot.selected_plot_index
        name = self.currentPlot.h_dict_geo[index]

        if (DEBUG):
            print("Clicked copyPopup in tab", self.wTab.currentIndex())

        self.copyAttr.histoLabel.setText(name)
        hdim = 2 if self.wConf.button2D.isChecked() else 1
        # setting up info for source histogram
        try:
            for idx, values in self.currentPlot.h_dict.items():
                if values.get("name") == name and idx == index:
                    if hdim == 2:
                        vmin, vmax = self.currentPlot.h_lst[idx].get_clim()
                        self.copyAttr.histoScaleValueminZ.setText('{:.1f}'.format(vmin))
                        self.copyAttr.histoScaleValuemaxZ.setText('{:.1f}'.format(vmax))

                    self.copyAttr.axisSLabel.setText("Log" if self.currentPlot.h_log[idx] else "Linear")

                    x_limits = self.currentPlot.h_limits[idx].get("x", [])
                    y_limits = self.currentPlot.h_limits[idx].get("y", [])
                    self.copyAttr.axisLimLabelX.setText(f"[{x_limits[0]:.1f},{x_limits[1]:.1f}]")
                    self.copyAttr.axisLimLabelY.setText(f"[{y_limits[0]:.1f},{y_limits[1]:.1f}]")
                else:
                    dim = values.get("dim", 1)
                    if hdim == dim:
                        instance = QPushButton(values.get("name", ""), self)
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
        # Simon - added following lines to avoid None plot index
        index = self.autoIndex()

        if self.currentPlot.isZoomed:
            ax = plt.gca()
        else:
            ax = self.select_plot(index)

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

                    minx = self.currentPlot.h_dict[index]["xmin"]
                    maxx = self.currentPlot.h_dict[index]["xmax"]
                    binx = self.currentPlot.h_dict[index]["xbin"]
                    if (DEBUG):
                        print(minx, maxx, binx)
                    xtmp = self.create_range(binx, minx, maxx)

                    #Simon - added following check
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

                    if (DEBUG):
                        print(fitpar)
                    ytmp = (self.get_data(index)).tolist()
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

                    #The default start method in SkelFit doesn't set fit_results, then it causes error if in the args
                    if fit_funct == "Skeleton":
                        fitln = fit.start(x, y, xmin, xmax, fitpar, ax)
                    else:
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

    def changeBkg(self):
        if any(x == 2 for x in self.currentPlot.h_dim) == True:
            indices = [i for i, x in enumerate(self.currentPlot.h_dim) if x == 2]
            for index in indices:
                self.currentPlot.isSelected = False # this line is important for automatic conversion from dark to light and viceversa
        self.updatePlot()
        #Simon - commented following line because drawAllGates is called in updatePlot
        #self.drawAllGates()
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

