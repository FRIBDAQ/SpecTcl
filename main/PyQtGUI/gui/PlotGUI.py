import random
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.gridspec as gridspec

from PyQt5 import QtCore
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

debug = False

class Tabs(QTabWidget):
    def __init__(self):
        QTabWidget.__init__(self)
        self.wPlot = {}
        #spectrum_dict {key:geo_index_spectrum, value:{info_spectrum - including spectrum name}}, user can change spectrum_info.
        self.spectrum_dict = {} #dict of dict
        self.spectrum_dict[0] = {}
        self.zoomPlotInfo = {} #Histo [index origin, name] in the zoomed/expanded mode
        self.zoomPlotInfo[0] = [] #dict of list
        self.countClickTab = {} #dict of flag to know if widgets already have dynamic bind
        self.createTabs()

    def createTabs(self):
        self.wPlot[0] = Plot()
        self.countClickTab[0] = False
        self.setUpdatesEnabled(True)
        self.insertTab(0, self.wPlot[0], "Tab" )
        self.insertTab(1,QWidget(),'  +  ')
        self.selected_plot_index_bak = []
        self.selected_plot_index_bak.append(None)
        #layout is a list that keeps for each tab [numberOfRow, numberOfColumn]
        self.layout = []
        self.layout.append([1,1])
        self.h_dict_geo_bak = {}
        self.h_log_bak = {}
        self.currentChanged.connect(self.addTab)

    def addTab(self, index):
        if index == self.count()-1:
            self.wPlot[index] = Plot()
            if (debug):
                print("Inserting tab at index", index)
            # last tab was clicked. add tab
            self.insertTab(index, self.wPlot[index], "Tab %d" %(index+1))
            self.setCurrentIndex(index)
            self.selected_plot_index_bak.append(None)
            self.layout.append([1,1])
            self.spectrum_dict[index] = {}
            self.zoomPlotInfo[index] = []
            self.countClickTab[index] = False


class Plot(QWidget):
    def __init__(self, *args, **kwargs):
        super(Plot, self).__init__(*args, **kwargs)

        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)

        self.toolbar = NavigationToolbar(self.canvas, self)
        self.copyButton = QPushButton("Copy Properties", self)
        self.histoLabel = QLabel(self)
        self.histoLabel.setText("Histogram: \nX: Y:")
        self.gateLabel = QLabel(self)
        self.gateLabel.setText("Gate applied: \n")
        self.pointerLabel = QLabel(self)
        self.pointerLabel.setText("Pointer: \nX: Y: Z:")
        self.createSRegion = QPushButton("Summing region", self)
        self.histo_autoscale = QCheckBox("Autoscale",self)
        self.logButton = QPushButton("Log", self)
        self.logButton.setFixedWidth(50)
        self.logButton.setStyleSheet("QPushButton { background-color: light gray }"
            "QPushButton:pressed { background-color: grey }" )
        self.plusButton = QPushButton("+", self)
        self.plusButton.setFixedWidth(30)
        self.minusButton = QPushButton("-", self)
        self.minusButton.setFixedWidth(30)
        self.cutoffButton = QPushButton("Cutoff", self)
        self.cutoffButton.setFixedWidth(70)
        self.cutoffButton.setStyleSheet("QPushButton { background-color: light gray }"
            "QPushButton:pressed { background-color: grey }" )
        self.customHomeButton = QPushButton("Reset", self)
        self.customHomeButton.setFixedWidth(70)



        spacer1 = QWidget()
        spacer1.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        spacer2 = QWidget()
        spacer2.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        

        # save wanted actions before remove all 
        allActions = self.toolbar.actions()
        for idx, x in enumerate(allActions):
            if idx == 5:
                zoomAction = x 
            if idx == 9:
                saveAction = x 
            self.toolbar.removeAction(x)

        # set actions in desired order
        self.toolbar.addWidget(self.histo_autoscale)
        self.toolbar.addAction(zoomAction)
        self.toolbar.addWidget(self.plusButton)
        self.toolbar.addWidget(self.minusButton)
        self.toolbar.addWidget(self.logButton)
        self.toolbar.addWidget(self.cutoffButton)
        self.toolbar.addWidget(self.customHomeButton)
        self.toolbar.addWidget(spacer1)
        self.toolbar.addWidget(self.pointerLabel)
        self.toolbar.addWidget(self.histoLabel)
        self.toolbar.addWidget(self.gateLabel)
        self.toolbar.addWidget(spacer2)
        self.toolbar.addWidget(self.createSRegion)
        self.toolbar.addWidget(self.copyButton)
        self.toolbar.addAction(saveAction)


        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)

        # plotting variables for each tab
        self.old_row = 1
        self.old_col = 1
        self.old_row_idx = 0
        self.old_col_idx = 0

        self.h_dict = {}
        self.h_dict_geo = {}
        self.h_dict_geo_bak = {}
        self.axbkg = {}
        self.h_limits = {} # dictionary with axis limits for the histogram
        self.h_log = {} # bool dict for linear/log axes
        self.h_log_bak = {} # bool dict for linear/log axes - backup
        self.h_setup = {} # bool dict for setting histograms
        self.h_dim = []
        self.h_lst = []
        self.axis_lst = []
        self.hist_list = [] #list with names of the plotted histogram (created for drawAllGates)
        self.cbar = {}
        self.next_plot_index = -1


        self.selected_plot_index = None
        self.index = 0

        self.autoScale = False
        self.logScale = False
        # drawing tools
        self.isLoaded = False
        self.isFull = False
        self.isEnlarged = False #Tells if the canvas is in single pad mode
        self.isSelected = False
        self.rec = None
        self.recDashed = None
        self.recDashedZoom = None
        #Simon - added flag
        self.isZoomCallback = False
        # self.isZoomInOut = False

        self.zoomPress = False #true when mouse press and drag rectangle, false at release

        # gates
        self.gate_dict = {}
        self.style_dict = {}
        self.artist_dict = {}
        self.artist_list = []
        self.artist1D = {}
        self.artist2D = {}
        self.gateTypeDict = {}
        self.region_dict = {}
        self.regionTypeDict = {}
        self.region_name = ""
        self.region_type = ""
        self.counter = 0
        self.counter_sr = 0
        self.toCreateGate = False
        self.toEditGate = False
        self.toCreateSRegion = False
        self.xs = []
        self.ys = []
        # #temporary holds gate lines - to control edition with on_singleclick and on_dblclick (reset once gate is pushed to ReST)
        # self.listGateLine = []

        # default canvas
        self.InitializeCanvas(self.old_row,self.old_col)


    def InitializeCanvas(self, row, col, flag = True):
        if (debug):
            print("InitializeCanvas with dimensions", row, col)
            print("old size", self.old_row, self.old_col, "--> new size", row, col)
        if flag:
            self.h_dict.clear()
            self.h_dict_geo.clear()
            self.h_limits.clear()

            self.index = 0
            self.idx = 0

        if (debug):
            print("The following three should be empty!")
            print("self.h_dict",self.h_dict)
            print("self.h_dict_geo",self.h_dict_geo)
            print("self.h_limits",self.h_limits)

        self.figure.clear()
        self.InitializeFigure(self.CreateFigure(row, col), row, col, flag)
        self.figure.tight_layout()
        self.canvas.draw()

        if (debug):
            print("The following three should NOT be empty!")
            print("self.h_dict",self.h_dict)
            print("self.h_dict_geo",self.h_dict_geo)
            print("self.h_limits",self.h_limits)

    def CreateFigure(self, row, col):
        self.grid = gridspec.GridSpec(ncols=col, nrows=row, figure=self.figure)
        return self.grid

    def InitializeHistogram(self):
        return {"name": "empty", "dim": 1, "xmin": 0, "xmax": 1, "xbin": 1,
                "ymin": 0, "ymax": 1, "ybin": 1, "parameters": [], "type": "", "scale": False}

    # get value for a dictionary at index x with key y
    def get_histo_key_value(self, d, index, key):
        if key in d[index]:
            return d[index][key]

    # get a list of elements identified by the key for a dictionary
    def get_histo_key_list(self, d, keys):
        lst = []
        for key, value in d.items():
            lst.append(self.get_histo_key_value(d, key, keys))
        return lst

    def InitializeFigure(self, grid, row, col, flag = True):
        if (debug):
            print("InitializeFigure")
            print("Test of InitializeHistogram", self.InitializeHistogram())
            print("row", row, "col", col)
        for i in range(row):
            for j in range(col):
                a = self.figure.add_subplot(grid[i,j])

        if flag:
            self.h_dim.clear()
            self.h_lst.clear()
            self.axis_lst.clear()

            if not self.isLoaded:
                self.old_row = row
                self.old_col = col

            for z in range(self.old_row*self.old_col):
                self.h_dict[z] = self.InitializeHistogram()
                self.h_dict_geo[z] = "empty"
                self.h_log[z] = False
                self.h_setup[z] = False
                self.h_lst.append(None)
                self.axis_lst.append(None)
            self.h_dim = self.get_histo_key_list(self.h_dict, "dim")


    # def autoscaleAxis(self, b):
    #     if b.text() == "Autoscale":
    #         if b.isChecked() == True:
    #             self.autoScale = True
    #             if (debug):
    #                 print(b.text()+" is selected")
    #         else:
    #             self.autoScale = False
    #             if (debug):
    #                 print(b.text()+" is deselected")
