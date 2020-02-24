import io
import sys, os
sys.path.append(os.getcwd())

import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt

from PyQt5 import QtCore
from PyQt5.QtWidgets import *
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

class WidgetPlot(QWidget):
        def __init__(self, *args, **kwargs):
                QWidget.__init__(self, *args, **kwargs)
                self.setLayout(QVBoxLayout())
                self.figure = plt.figure()
                self.canvas = FigureCanvas(self.figure)
                self.toolbar = NavigationToolbar(self.canvas, self)
                self.layout().addWidget(self.toolbar)
                self.layout().addWidget(self.canvas)

        def get_figure(self):
                return self.figure

        def get_canvas(self):
                return self.canvas
                
