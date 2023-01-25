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

            self.isDrag = False
            self.isEdit = False            

            self.histo_list_label = QLabel("Spectrum")
            self.histo_list_label.setFixedWidth(60)
            self.histo_list = QComboBox()
            self.histo_geo_label = QLabel("Geometry")
            self.histo_geo_label.setFixedWidth(60)
            self.histo_geo_row = QComboBox()
            self.histo_geo_col = QComboBox()
            self.histo_geo_add = QPushButton("Add", self)
            self.histo_geo_add.setStyleSheet("background-color:#bcee68;")
            self.histo_geo_update = QPushButton("Update", self)
            self.histo_geo_update.setStyleSheet("background-color:#bcee68;")
            self.histo_geo_delete = QPushButton("Clear", self)
            self.histo_geo_delete.setStyleSheet("background-color:#bcee68;")
            #self.histo_geo_all = QCheckBox("All",self)
            #self.histo_geo_all.setFixedWidth(35)
            self.listParams_label = []
            self.listParams = []
            for i in range(2):
                self.listParams_label.append(QLabel("Parameter"))
                self.listParams.append(QComboBox())
            self.listParams_label[0].setFixedWidth(65)
            self.listParams_label[1].setFixedWidth(65)                
            self.button1D = QRadioButton("1D")
            self.button1D.setFixedWidth(40)
            self.button2D = QRadioButton("2D")
            self.button2D.setFixedWidth(40)                        
            self.button2D_option = QComboBox()
            self.button2D_option.addItem("Light")
            self.button2D_option.addItem("Dark")
            self.gate_list_label = QLabel("Gate")            
            self.gate_list_label.setFixedWidth(30)            
            self.listGate = QComboBox()
            self.listGate_type = QComboBox()
            self.listGate_type.setFixedWidth(50)            
            self.listGate_type.addItem("s")
            self.listGate_type.addItem("c")
            self.listGate_type.addItem("b")
            self.listGate_type.addItem("gs")
            self.listGate_type.addItem("gc")
            self.listGate_type.addItem("gb")
            self.listGate_type.setCurrentIndex(-1)
            self.createGate = QPushButton("Create", self)
            self.createGate.setStyleSheet("background-color:#ffc7fd;")        
            self.editGate = QPushButton("Modify", self)
            self.menu = QMenu()
            self.menu.addAction('Drag', self.drag)
            self.menu.addAction('Edit', self.edit)
            self.editGate.setMenu(self.menu)

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
            #hlayout.addWidget(self.histo_geo_all)
            hlayout.addWidget(self.listParams_label[0])
            hlayout.addWidget(self.listParams[0])
            hlayout.addWidget(self.listParams_label[1])        
            hlayout.addWidget(self.listParams[1])
            hlayout.addWidget(self.button1D)
            hlayout.addWidget(self.button2D)
            hlayout.addWidget(self.button2D_option)

            hlayout2 = QHBoxLayout()
            hlayout2.addWidget(self.gate_list_label)
            hlayout2.addWidget(self.listGate)
            hlayout2.addWidget(self.listGate_type)        
            hlayout2.addWidget(self.createGate)
            hlayout2.addWidget(self.editGate)
            hlayout2.addWidget(self.drawGate)
            hlayout2.addWidget(self.cleanGate)        
            hlayout2.addWidget(self.deleteGate)
            hlayout2.addWidget(self.integrateGate)
            hlayout2.addWidget(self.outputGate)
            hlayout2.addWidget(self.histo_geo_label)
            hlayout2.addWidget(self.histo_geo_row)
            hlayout2.addWidget(self.histo_geo_col)

            layout = QVBoxLayout()
            layout.addLayout(hlayout)
            layout.addLayout(hlayout2)
            self.setLayout(layout)
            
    def drag(self):
        self.isDrag = True
        self.isEdit = False
        
    def edit(self):
        self.isDrag = False        
        self.isEdit = True        
        
