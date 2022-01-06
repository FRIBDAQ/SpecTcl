import time
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *
import CPyConverter as cpy

class Configuration(QWidget):
    def __init__(self, *args, **kwargs):
            super(Configuration, self).__init__(*args, **kwargs)
            
            self.oldrow = 0
            self.oldcol = 0

            layout = QHBoxLayout()
            layout.setContentsMargins(0, 0, 0, 0)
            layout.addWidget(self.create_lstBox())
            self.setLayout(layout)
    
    def create_lstBox(self):
        #spectrumListBox = QGroupBox("Spectrum List")
        spectrumListBox = QGroupBox("")        
        spectrumListBox.setStyleSheet("QGroupBox{padding-top:200em; margin-top:-200em}")
        
        self.histo_list_label = QLabel("Spectrum")
        self.histo_list = QComboBox()
        self.histo_list.setFixedWidth(100)        
        self.histo_geo_label = QLabel("Geometry")
        self.histo_geo_row = QComboBox()
        self.histo_geo_col = QComboBox()
        self.histo_geo_add = QPushButton("Add", self)
        self.histo_geo_add.setStyleSheet("background-color:#bcee68;")
        self.histo_geo_update = QPushButton("Update", self)
        self.histo_geo_update.setStyleSheet("background-color:#bcee68;")
        self.histo_geo_delete = QPushButton("Clear", self)
        self.histo_geo_delete.setStyleSheet("background-color:#bcee68;")
        self.histo_geo_all = QCheckBox("All",self)
        self.listParams_label = []
        self.listParams = []
        for i in range(2):
            self.listParams_label.append(QLabel("Parameter"))
            self.listParams.append(QComboBox())
        self.button1D = QRadioButton("1D")
        self.button2D = QRadioButton("2D")
        self.button2D_option = QComboBox()
        self.button2D_option.addItem("Light")
        self.button2D_option.addItem("Dark")

        self.gate_list_label = QLabel("Gate")        
        self.listGate = QComboBox()
        self.listGate.setFixedWidth(100)
        self.listGate_type = QComboBox()
        self.listGate_type.addItem("s")
        self.listGate_type.addItem("c")
        self.listGate_type.addItem("b")
        self.listGate_type.addItem("gs")
        self.listGate_type.addItem("gc")
        self.listGate_type.addItem("gb")
        self.listGate_type.setCurrentIndex(-1)
        self.createGate = QPushButton("Create", self)
        self.createGate.setStyleSheet("background-color:#ffc7fd;")        
        self.editGate = QPushButton("Edit", self)
        self.editGate.setStyleSheet("background-color:#ffc7fd;")                
        self.deleteGate = QPushButton("Delete", self)
        self.deleteGate.setStyleSheet("background-color:#cd96cd;")
        self.drawGate = QPushButton("Draw", self)
        self.drawGate.setStyleSheet("background-color:#cd96cd;")
        self.cleanGate = QPushButton("Clear", self)
        self.cleanGate.setStyleSheet("background-color:#cd96cd;")
        
        self.integrateGate = QPushButton("Integrate", self)
        self.integrateGate.setStyleSheet("background-color:#9f79ee;")
        self.outputGate = QPushButton("Output", self)
        self.outputGate.setStyleSheet("background-color:#9f79ee;")
        
        for i in range(1,10):
            self.histo_geo_row.addItem(str(i))
            self.histo_geo_col.addItem(str(i))
        self.row = int(self.histo_geo_row.currentText())
        self.col = int(self.histo_geo_col.currentText())
        self.oldrow = self.row
        self.oldcol = self.col
                        
        hlayout = QHBoxLayout()
        hlayout.addWidget(self.histo_list_label)
        hlayout.addWidget(self.histo_list)
        hlayout.addWidget(self.histo_geo_add)
        hlayout.addWidget(self.histo_geo_update)
        hlayout.addWidget(self.histo_geo_delete)
        hlayout.addWidget(self.histo_geo_all)
        hlayout.addWidget(self.listParams_label[0])
        hlayout.addWidget(self.listParams[0])
        hlayout.addWidget(self.listParams_label[1])        
        hlayout.addWidget(self.listParams[1])
        hlayout.addWidget(self.button1D)
        hlayout.addWidget(self.button2D)
        hlayout.addWidget(self.button2D_option)
        hlayout.addWidget(self.histo_geo_label)
        hlayout.addWidget(self.histo_geo_row)
        hlayout.addWidget(self.histo_geo_col)
        hlayout.addWidget(self.gate_list_label)
        hlayout.addWidget(self.listGate)
        hlayout.addWidget(self.listGate_type)        
        hlayout.addWidget(self.createGate)
        hlayout.addWidget(self.editGate)
        hlayout.addWidget(self.drawGate)
        hlayout.addWidget(self.cleanGate)        
        hlayout.addWidget(self.deleteGate)
        hlayout.addWidget(self.integrateGate)
        hlayout.addWidget(self.outputGate)

        spectrumListBox.setLayout(hlayout)
        return spectrumListBox
                                                                                        
