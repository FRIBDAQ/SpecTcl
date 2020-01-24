#!/usr/bin/env python
import io
import pickle
import sys, os
sys.path.append(os.getcwd())

import json
import httplib2
import pandas as pd

import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore, QtGui
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import CPyConverter as cpy

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.gridspec as gridspec
from copy import copy
from mpl_toolkits.axes_grid1 import make_axes_locatable


class Color(QWidget):
    def __init__(self, color, *args, **kwargs):
        super(Color, self).__init__(*args, **kwargs)
        self.setAutoFillBackground(True)
        
        self.button = QPushButton(color, self)
        
class MainWindow(QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        self.setWindowTitle("My Awesome App")

        mainLayout = QVBoxLayout()
        
        #widget1
        layout = QHBoxLayout()

        hide = QPushButton("Hide", self)
        close = QPushButton("Exit", self)
        layout.addWidget(hide)
        layout.addWidget(close)        

        self.widget1 = QWidget()
        self.widget1.setLayout(layout)

        # widget2
        layout1 = QHBoxLayout()

        self.child = Color("yellow")        
        layout1.addWidget(self.child)

        self.widget2 = QWidget()
        self.widget2.setLayout(layout1)
        
        mainLayout.addWidget(self.widget1)
        mainLayout.addWidget(self.widget2)        

        widget = QWidget()
        widget.setLayout(mainLayout)
        
        self.setCentralWidget(widget)

        # signal
        hide.clicked.connect(self.hide)
        close.clicked.connect(self.close)
        
        self.child.button.clicked.connect(self.testChild)
        
        
    def hide(self):
        self.widget2.hide()

    def testChild(self):
        print("ciao")

        
app = QApplication(sys.argv)
gui = MainWindow()
gui.show()
sys.exit(app.exec_())
