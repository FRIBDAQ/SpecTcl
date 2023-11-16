import sys, csv, io, time
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore, QtGui
from PyQt5.QtWidgets import *
import CPyConverter as cpy

class options(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)

    def create_options(self):
        pCheck = QGroupBox("other options")

        self.gateAnnotation = QCheckBox("Gate annotation",self)
        self.gateEditDisable = QCheckBox("Disable gate edition",self)
        self.gateHide = QCheckBox("Hide gate",self)

        layout = QGridLayout()
        layout.addWidget(self.gateAnnotation)
        layout.addWidget(self.gateEditDisable)
        layout.addWidget(self.gateHide)
        pCheck.setLayout(layout)

        return pCheck


