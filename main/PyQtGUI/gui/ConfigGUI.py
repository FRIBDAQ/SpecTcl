import time
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *
import CPyConverter as cpy

class Configuration(QGridLayout):
    def __init__(self, *args, **kwargs):
            super(Configuration, self).__init__(*args, **kwargs)


            self.oldrow = 0
            self.oldcol = 0

            self.isDrag = False
            self.isEdit = False            

            self.histo_list_label = QLabel("   Spectrum")
            self.histo_list_label.setFixedWidth(70)
            self.histo_list = QComboBox()
            self.histo_list.setFixedWidth(200)

            self.histo_geo_label = QLabel("Geometry")
            self.histo_geo_label.setFixedWidth(63)
            self.histo_geo_row = QComboBox()
            self.histo_geo_row.setFixedWidth(50)
            self.histo_geo_col = QComboBox()
            self.histo_geo_col.setFixedWidth(50)

            self.histo_geo_add = QPushButton("Add")
            self.histo_geo_add.setFixedWidth(100)
            self.histo_geo_add.setStyleSheet("background-color:#bcee68;")

            self.histo_geo_update = QPushButton("Update")
            self.histo_geo_update.setFixedWidth(100)
            self.histo_geo_update.setStyleSheet("background-color:#bcee68;")
             
            self.button1D = QRadioButton("1D")
            # self.button1D.setFixedWidth(40)
            self.button2D = QRadioButton("2D")
            # self.button2D.setFixedWidth(40)                        
            self.button2D_option = QComboBox()
            self.button2D_option.addItem("Light")
            self.button2D_option.addItem("Dark")

            #Gate menu
            self.gate_list_label = QLabel("   Gate")            
            self.gate_list_label.setFixedWidth(40)            
            self.listGate = QComboBox()
            self.listGate.setFixedWidth(125) 
            self.listGate_type = QComboBox()
            self.listGate_type.setFixedWidth(50)            
            self.listGate_type.addItem("s")
            self.listGate_type.addItem("c")
            self.listGate_type.addItem("b")
            self.listGate_type.addItem("gs")
            self.listGate_type.addItem("gc")
            self.listGate_type.addItem("gb")
            self.listGate_type.setCurrentIndex(-1)

            self.createGate = QPushButton("Create")
            self.createGate.setFixedWidth(100)
            self.createGate.setStyleSheet("background-color:#ffc7fd;")        

            self.editGate = QPushButton("Modify")
            self.editGate.setFixedWidth(100)
            self.menu = QMenu()
            self.menu.addAction('Drag', self.drag)
            self.menu.addAction('Edit', self.edit)
            self.editGate.setMenu(self.menu)
            self.editGate.setStyleSheet("background-color:#ffc7fd;")  

            self.integrateGate = QPushButton("Integrate")
            self.integrateGate.setFixedWidth(100)
            self.integrateGate.setStyleSheet("background-color:#9f79ee;")

            self.extraButton = QPushButton("Extra")
            self.extraButton.setFixedWidth(100)
            self.extraButton.setStyleSheet("background-color:#eead0e;")

            
            for i in range(1,10):
                self.histo_geo_row.addItem(str(i))
                self.histo_geo_col.addItem(str(i))
            self.row = int(self.histo_geo_row.currentText())
            self.col = int(self.histo_geo_col.currentText())
            self.oldrow = self.row
            self.oldcol = self.col            




            #line organized in several blocks geoLayout, spectrumLayout, gateLayout, extraLayout
            # configLayout = QGridLayout()
            geoLayout = QHBoxLayout()
            spectrumLayout = QHBoxLayout()
            gateLayout = QHBoxLayout()
            extraLayout = QHBoxLayout()

            geoLayout.addWidget(self.histo_geo_label)
            geoLayout.addWidget(self.histo_geo_row)
            geoLayout.addWidget(self.histo_geo_col)

            spectrumLayout.addWidget(self.histo_list_label)
            spectrumLayout.addWidget(self.histo_list)
            spectrumLayout.addWidget(self.histo_geo_add)
            spectrumLayout.addWidget(self.histo_geo_update)

            gateLayout.addWidget(self.gate_list_label)
            gateLayout.addWidget(self.listGate)
            gateLayout.addWidget(self.listGate_type)
            gateLayout.addWidget(self.createGate)
            gateLayout.addWidget(self.editGate)
            gateLayout.addWidget(self.integrateGate)

            extraLayout.addWidget(self.extraButton)


            self.addLayout(geoLayout, 0, 1, 0, 1)
            self.addLayout(spectrumLayout, 0, 2, 0, 1)
            self.addLayout(gateLayout, 0, 3, 0, 1)
            self.addLayout(extraLayout, 0, 4, 0, 1)


            
    def drag(self):
        self.isDrag = True
        self.isEdit = False
        
    def edit(self):
        self.isDrag = False        
        self.isEdit = True        
        
