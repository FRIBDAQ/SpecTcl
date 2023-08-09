import random
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import matplotlib.gridspec as gridspec

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

debug = False

class Tabs(QTabWidget):
    def __init__(self):
        QTabWidget.__init__(self)
        self.wPlot = {}
        self.createTabs()

    def createTabs(self):
        self.wPlot[0] = Plot()
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
            print("Inserting tab at index", index,self.layout)

class Plot(QWidget):
    def __init__(self, *args, **kwargs):
        super(Plot, self).__init__(*args, **kwargs)

        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)
        self.plusButton = QPushButton("+", self)
        self.minusButton = QPushButton("-", self)
        self.copyButton = QPushButton("Copy Properties", self)
        self.histo_log = QCheckBox("Log",self)
        self.histo_autoscale = QCheckBox("Autoscale",self)
        self.histoLabel = QLabel(self)
        self.histoLabel.setText("Histogram:")
        self.createSRegion = QPushButton("Summing region", self)
        self.toolbar.addWidget(self.copyButton)
        self.toolbar.addWidget(self.createSRegion)
        self.toolbar.addWidget(self.histoLabel)
        self.toolbar.addWidget(self.histo_log)
        self.toolbar.addWidget(self.histo_autoscale)
        self.toolbar.addWidget(self.plusButton)
        self.toolbar.addWidget(self.minusButton)

        # removing buttons from toolbar
        unwanted_buttons = ['Back','Forward', 'Subplots', 'Pan', 'Customize']
        for x in self.toolbar.actions():
            if x.text() in unwanted_buttons:
                self.toolbar.removeAction(x)

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
        self.isZoomed = False
        self.isSelected = False
        self.rec = None
        self.recDashed = None
        #Simon - added flag
        self.isZoomCallback = False
        self.isZoomInOut = False

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
        self.listLine = []

        # default canvas
        self.InitializeCanvas(self.old_row,self.old_col)

        self.histo_autoscale.clicked.connect(lambda:self.autoscaleAxis(self.histo_autoscale))
        self.histo_log.clicked.connect(lambda:self.logAxis(self.histo_log))

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

            if not self.isLoaded:
                self.old_row = row
                self.old_col = col
                self.histo_log.setChecked(False)

            for z in range(self.old_row*self.old_col):
                self.h_dict[z] = self.InitializeHistogram()
                self.h_dict_geo[z] = "empty"
                self.h_log[z] = False
                self.h_setup[z] = False
                self.h_lst.append(None)
            self.h_dim = self.get_histo_key_list(self.h_dict, "dim")

            if (debug):
                print("These should be initialized!")
                print("self.h_dict",self.h_dict)
                print("self.h_dict_geo",self.h_dict_geo)
                print("self.h_log",self.h_log)
                print("self.h_setup",self.h_setup)
                print("self.h_dim",self.h_dim)

    def autoscaleAxis(self, b):
        if b.text() == "Autoscale":
            if b.isChecked() == True:
                self.autoScale = True
                if (debug):
                    print(b.text()+" is selected")
            else:
                self.autoScale = False
                if (debug):
                    print(b.text()+" is deselected")

    def logAxis(self,b):
        if b.text() == "Log":
            if b.isChecked() == True:
                self.logScale = True
                if (debug):
                    print(b.text()+" is selected")
            else:
                self.logScale = False
                if (debug):
                    print(b.text()+" is deselected")
