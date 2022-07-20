import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

class Tabs(QTabWidget):
    def __init__(self, plot):
        QTabWidget.__init__(self)       
        self.index = 0
        self.wPlot = plot
        self.createTabs()
        
    def createTabs(self):
        self.setUpdatesEnabled(True)

        self.insertTab(0,self.wPlot, "Tab" )
        #self.insertTab(1,QWidget(),'  +  ') 

        #self.currentChanged.connect(self.addTab) 

    def addTab(self, index):    
        print("Inside Tabs.addTab")
        if index == self.count()-1 :    
            '''last tab was clicked. add tab'''
            self.insertTab(index, self.wPlot, "Tab %d" %(index+1)) 
            self.setCurrentIndex(index)
            
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
        unwanted_buttons = ['Back','Forward']
        for x in self.toolbar.actions():
            if x.text() in unwanted_buttons:
                self.toolbar.removeAction(x)
        
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
                                                                                                            
        
