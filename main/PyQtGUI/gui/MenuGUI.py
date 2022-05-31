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

            layout = QHBoxLayout()
            layout.setContentsMargins(0,0,0,0)
            layout.addWidget(self.create_menuBox())
            self.setLayout(layout)

            
        def create_menuBox(self):
                #menuBox = QGroupBox("Menu")
                menuBox = QGroupBox("")                
                menuBox.setStyleSheet("QGroupBox{padding-top:20em; margin-top:-20em}")
                
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
                self.server.setFixedWidth(150);
                self.restLabel = QLabel(self)
                self.restLabel.setText('REST port')
                self.rest = QLineEdit(self)
                self.rest.setText(port)
                self.rest.setFixedWidth(75);                
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
                self.mirror.setFixedWidth(75);                                
                self.slider_label = QLabel("Refresh Interval (0 s)")
                self.slider = QSlider(QtCore.Qt.Horizontal, self)
                self.slider.setMinimum(0)
                self.slider.setMaximum(30)
                
                layout = QHBoxLayout()
                layout.addWidget(self.userLabel)
                layout.addWidget(self.user)
                layout.addWidget(self.serverLabel)
                layout.addWidget(self.server)
                layout.addWidget(self.restLabel)
                layout.addWidget(self.rest)                                
                layout.addWidget(self.mirrorLabel)
                layout.addWidget(self.mirror)                                
                layout.addWidget(self.updateButton)
                layout.addWidget(self.slider_label)
                layout.addWidget(self.slider)
                layout.addWidget(self.extraButton)
                layout.addWidget(self.loadButton)
                layout.addWidget(self.saveButton)
                layout.addWidget(self.exitButton)

                menuBox.setLayout(layout)
                
                return menuBox
