import os
import getpass
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

class Menu(QGridLayout):
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
 
            self.serverLabel = QLabel()
        #     self.serverLabel = QLabel(self)
            self.serverLabel.setText('   Server')
            self.serverLabel.setFixedWidth(50) 
        #     self.server = QLineEdit(self)
            self.server = QLineEdit()
            self.server.setFixedWidth(180) 
            self.server.setText(host)

        #     self.restLabel = QLabel(self)
            self.restLabel = QLabel()
            self.restLabel.setText('   REST port')
            self.restLabel.setFixedWidth(70) 
        #     self.rest = QLineEdit(self)
            self.rest = QLineEdit()
            self.rest.setText(port)
            self.rest.setFixedWidth(80) 
            
            self.userLabel = QLabel("   User")
            self.userLabel.setFixedWidth(40) 
        #     self.user = QLineEdit(self)                
            self.user = QLineEdit()                
            self.user.setText(getpass.getuser())
            self.user.setFixedWidth(180) 

            self.mirrorLabel = QLabel("   Mirror port")
            self.mirrorLabel.setFixedWidth(75) 

        #     self.mirror = QLineEdit(self)
            self.mirror = QLineEdit()
            self.mirror.setText(mirrorport)
            self.mirror.setFixedWidth(80) 

        #     self.exitButton = QPushButton("Exit", self)
            self.exitButton = QPushButton("Exit")
            self.exitButton.setStyleSheet("background-color:rgb(252, 48, 3);")
            self.exitButton.setFixedWidth(100) 

        #     self.connectButton = QPushButton("Connect", self)
            self.connectButton = QPushButton("Connect")
            self.connectButton.setStyleSheet("background-color:#7ec0ee;")
            self.connectButton.setFixedWidth(100) 

        #     self.extraButton = QPushButton("Extra", self)
        #     self.extraButton.setStyleSheet("background-color:#eead0e;")

        #     self.saveButton = QPushButton("Save Geometry", self)
            self.saveButton = QPushButton("Save Geometry")
            self.saveButton.setStyleSheet("background-color:#ffd700;")
            self.saveButton.setFixedWidth(100) 

        #     self.loadButton = QPushButton("Load Geometry", self)
            self.loadButton = QPushButton("Load Geometry")
            self.loadButton.setStyleSheet("background-color:#ffd700;")
            self.loadButton.setFixedWidth(100) 

            #Simon - test rest thread
            self.restThreadSwitch = True

        #     self.slider_label = QLabel("Refresh Interval (0 s)")
        #     self.slider = QSlider(QtCore.Qt.Horizontal, self)
        #     self.slider.setMinimum(0)
        #     self.slider.setMaximum(30)
        #     self.slider.setDisabled(True) #temporarily
            
        #     hlayout = QHBoxLayout()
        #     hlayout.addWidget(self.userLabel)
        #     hlayout.addWidget(self.user)
        #     hlayout.addWidget(self.serverLabel)
        #     hlayout.addWidget(self.server)
        #     hlayout.addWidget(self.restLabel)
        #     hlayout.addWidget(self.rest)                                
        #     hlayout.addWidget(self.mirrorLabel)
        #     hlayout.addWidget(self.mirror)                                
        #     hlayout.addWidget(self.connectButton)
        #     hlayout.addWidget(self.slider_label)
        #     hlayout.addWidget(self.slider)
        #     hlayout.addWidget(self.extraButton)
        #     hlayout.addWidget(self.loadButton)
        #     hlayout.addWidget(self.saveButton)
        #     hlayout.addWidget(self.exitButton)
            
        #     layout = QVBoxLayout()
        #     layout.addLayout(hlayout)
        #     self.setLayout(layout)






            #line organized in several blocks connectLayout, geoLayout, othersLayout
        #     menuLayout = QGridLayout()
            connectLayout = QHBoxLayout()
            geoLayout = QHBoxLayout()
            othersLayout = QHBoxLayout()

            #horizontal spacer expandable between blocks
        #     hSpacer = QSpacerItem(QSizePolicy.Expanding, QSizePolicy.MinimumExpanding)
        #     hSpacerLayout.addWidget(QSpacerItem(QSizePolicy.MinimumExpanding))

        #     connectLayout.addWidget(self.userLabel)
        #     connectLayout.addWidget(self.user)
        #     connectLayout.addWidget(self.serverLabel)
        #     connectLayout.addWidget(self.server)
        #     connectLayout.addWidget(self.restLabel)
        #     connectLayout.addWidget(self.rest)
        #     connectLayout.addWidget(self.mirrorLabel)
        #     connectLayout.addWidget(self.mirror)
            connectLayout.addWidget(self.connectButton)
        #     connectLayout.addItem(hSpacer)

            geoLayout.addWidget(self.loadButton)
            geoLayout.addWidget(self.saveButton)
        #     geoLayout.addItem(hSpacer)

            othersLayout.addWidget(self.exitButton)

        #     menuLayout.addLayout(connectLayout, 0, 1, 0, 1)
        #     menuLayout.addLayout(geoLayout, 0, 2, 0, 1)
        #     menuLayout.addLayout(othersLayout, 0, 3, 0, 1)

            self.addLayout(connectLayout, 0, 1, 0, 1)
            self.addLayout(geoLayout, 0, 2, 0, 1)
            self.addLayout(othersLayout, 0, 3, 0, 1)

            

        #     self.setLayout(menuLayout)

