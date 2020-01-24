import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

class Menu(QWidget):
        def __init__(self, *args, **kwargs):
            super(Menu, self).__init__(*args, **kwargs)

            layout = QHBoxLayout()
            layout.addWidget(self.create_menuBox())
            self.setLayout(layout)

        def create_menuBox(self):
                menuBox = QGroupBox("Menu")
                
                self.exitButton = QPushButton("Exit", self)
                self.updateButton = QPushButton("Get Data", self)
                self.configButton = QPushButton("Configure", self)
                self.saveButton = QPushButton("Save Geometry", self)
                self.loadButton = QPushButton("Load Geometry", self)
                
                self.slider_label = QLabel("Refresh Interval (0 s)")
                self.slider = QSlider(QtCore.Qt.Horizontal, self)
                self.slider.setMinimum(0)
                self.slider.setMaximum(30)
                
                layout = QHBoxLayout()
                layout.addWidget(self.exitButton)
                layout.addWidget(self.updateButton)
                layout.addWidget(self.slider_label)
                layout.addWidget(self.slider)
                layout.addWidget(self.configButton)
                layout.addWidget(self.saveButton)
                layout.addWidget(self.loadButton)
                layout.addStretch(1)
                
                menuBox.setLayout(layout)
                menuBox.setMaximumHeight(100)
                
                return menuBox
                                                                                                                                                                                                                                
