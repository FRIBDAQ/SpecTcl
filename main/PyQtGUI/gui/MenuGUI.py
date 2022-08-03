import os
import getpass
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

            try:
                    host = os.environ["RESThost"]
                    port = os.environ["RESTport"]
                    mirrorport = os.environ["MIRRORport"]
            except:
                    os.environ['RESThost'] = "hostname"
                    os.environ["RESTport"] = "port"
                    os.environ["MIRRORport"] = "mirror"
                    host = os.environ["RESThost"]
                    port = os.environ["RESTport"]
                    mirrorport = os.environ["MIRRORport"]                        
 
            self.serverLabel = QLabel(self)
            self.serverLabel.setText('Server')
            self.server = QLineEdit(self)
            self.server.setText(host)
            self.restLabel = QLabel(self)
            self.restLabel.setText('REST port')
            self.rest = QLineEdit(self)
            self.rest.setText(port)
            self.exitButton = QPushButton("Exit", self)
            self.exitButton.setStyleSheet("background-color:rgb(252, 48, 3);")
            self.updateButton = QPushButton("Get Data", self)
            self.updateButton.setStyleSheet("background-color:#7ec0ee;")
            self.extraButton = QPushButton("Extra", self)
            self.extraButton.setStyleSheet("background-color:#eead0e;")
            self.saveButton = QPushButton("Save Geometry", self)
            self.saveButton.setStyleSheet("background-color:#ffd700;")
            self.loadButton = QPushButton("Load Geometry", self)
            self.loadButton.setStyleSheet("background-color:#ffd700;")
            
            self.userLabel = QLabel("User")
            self.user = QLineEdit(self)                
            self.user.setText(getpass.getuser())
            self.mirrorLabel = QLabel("Mirror port")
            self.mirror = QLineEdit(self)
            self.mirror.setText(mirrorport)
            self.slider_label = QLabel("Refresh Interval (0 s)")
            self.slider = QSlider(QtCore.Qt.Horizontal, self)
            self.slider.setMinimum(0)
            self.slider.setMaximum(30)
            self.slider.setDisabled(True) #temporarily
            
            hlayout = QHBoxLayout()
            hlayout.addWidget(self.userLabel)
            hlayout.addWidget(self.user)
            hlayout.addWidget(self.serverLabel)
            hlayout.addWidget(self.server)
            hlayout.addWidget(self.restLabel)
            hlayout.addWidget(self.rest)                                
            hlayout.addWidget(self.mirrorLabel)
            hlayout.addWidget(self.mirror)                                
            hlayout.addWidget(self.updateButton)
            hlayout.addWidget(self.slider_label)
            hlayout.addWidget(self.slider)
            hlayout.addWidget(self.extraButton)
            hlayout.addWidget(self.loadButton)
            hlayout.addWidget(self.saveButton)
            hlayout.addWidget(self.exitButton)
            
            layout = QVBoxLayout()
            layout.addLayout(hlayout)
            self.setLayout(layout)
