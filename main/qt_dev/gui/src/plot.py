import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

class Plot(QWidget):
    def __init__(self, *args, **kwargs):
        super(Plot, self).__init__(*args, **kwargs)
        
        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)
        self.plusButton = QPushButton("+", self)
        self.minusButton = QPushButton("-", self)
        self.toolbar.addWidget(self.plusButton)
        self.toolbar.addWidget(self.minusButton)
        
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
                                                                                                            
