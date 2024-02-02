import sys, csv, io, time
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore, QtGui
from PyQt5.QtWidgets import *
import CPyConverter as cpy
from PyQt5.QtCore import Qt

class options(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)

    def create_options(self):
        pCheck = QGroupBox("other options")

        self.gateAnnotation = QCheckBox("Gate annotation",self)
        self.gateEditDisable = QCheckBox("Disable gate edition",self)
        self.gateHide = QCheckBox("Hide gate",self)
        self.debugMode = QCheckBox("Debug mode",self)
        self.debugMode.setToolTip("When checked, print debug info into ./debugCutiePie.log")  


        layout = QGridLayout()
        layout.addWidget(self.gateAnnotation, 1, 1, 1, 1)
        layout.addWidget(self.gateEditDisable, 2, 1, 1, 1)
        layout.addWidget(self.gateHide, 3, 1, 1, 1)
        layout.addWidget(self.debugMode, 4, 1, 1, 1)
        layout.setAlignment(Qt.AlignTop)
        pCheck.setLayout(layout)

        return pCheck


