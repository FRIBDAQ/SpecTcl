import os
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
            layout.addWidget(self.create_gateBox())
            layout.addWidget(self.create_outputBox())                        
            self.setLayout(layout)

        def create_menuBox(self):
                menuBox = QGroupBox("Menu")

                host = os.environ["RESThost"]
                port = os.environ["RESTport"]
                
                self.serverLabel = QLabel(self)
                self.serverLabel.setText('Server:')
                self.server = QLineEdit(self)
                self.server.setText(host+":"+port)
                self.exitButton = QPushButton("Exit", self)
                self.updateButton = QPushButton("Get Data", self)
                self.configButton = QPushButton("Options", self)
                self.saveButton = QPushButton("Save Geometry", self)
                self.loadButton = QPushButton("Load Geometry", self)

                self.slider_label = QLabel("Refresh Interval (0 s)")
                self.slider = QSlider(QtCore.Qt.Horizontal, self)
                self.slider.setMinimum(0)
                self.slider.setMaximum(30)

                layout = QHBoxLayout()
                layout.addWidget(self.serverLabel)
                layout.addWidget(self.server)                
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
                                                                                                                                                                                                             
        def create_gateBox(self):
                gateBox = QGroupBox("Gate")                

                self.listGate = QComboBox()
                self.listGate.setFixedWidth(150)
                self.createGate = QPushButton("Create", self)
                self.editGate = QPushButton("Edit", self)
                self.deleteGate = QPushButton("Delete", self)                                
                self.drawGate = QPushButton("Draw", self)
                self.cleanGate = QPushButton("Clear", self)
                #self.test = QPushButton("Test", self)                                                
                
                
                layout = QHBoxLayout()
                layout.addWidget(self.listGate)
                layout.addWidget(self.createGate)
                layout.addWidget(self.editGate)                
                layout.addWidget(self.deleteGate)
                layout.addWidget(self.drawGate)
                layout.addWidget(self.cleanGate)
                #layout.addWidget(self.test)                

                layout.addStretch(1)
                
                gateBox.setLayout(layout)
                gateBox.setMaximumHeight(100)
                
                return gateBox                

        def create_outputBox(self):
                outputBox = QGroupBox("Output")                        

                self.integrateGate = QPushButton("Integrate", self)
                self.outputGate = QPushButton("Output", self)

                layout = QHBoxLayout()
                layout.addWidget(self.integrateGate)
                layout.addWidget(self.outputGate)

                layout.addStretch(1)
                
                outputBox.setLayout(layout)
                outputBox.setMaximumHeight(100)
                
                return outputBox                

                
