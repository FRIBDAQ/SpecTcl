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
            
            self.width = 270
            self.oldrow = 0
            self.oldcol = 0

            layout = QHBoxLayout()
            layout.addWidget(self.create_typeBox())
            layout.addWidget(self.create_defBox())
            layout.addWidget(self.create_algoBox())            
            layout.addWidget(self.create_lstBox())
            self.setLayout(layout)

    def create_typeBox(self):
        spectrumTypeBox = QGroupBox("Spectrum Type")
            
        self.button1D = QRadioButton("1D")
        self.button2D = QRadioButton("2D")
        self.button2D_option = QComboBox()
        self.button2D_option.addItem("Light")
        self.button2D_option.addItem("Dark")
            
        typelayout = QVBoxLayout()
        typelayout.addWidget(self.button1D)
        hl = QHBoxLayout()
        hl.addWidget(self.button2D)
        hl.addWidget(self.button2D_option)
        typelayout.addLayout(hl)
        typelayout.addStretch(1)
        
        spectrumTypeBox.setLayout(typelayout)
        spectrumTypeBox.setMaximumHeight(self.width)
            
        return spectrumTypeBox
    
    def create_defBox(self):
        spectrumCreateBox = QGroupBox("Spectrum Definition")
                
        self.spectrum_name_label = QLabel("Spectrum Name")
        self.spectrum_name = QLineEdit(self)
        self.buttonSpectrumCreate = QPushButton("Create/Replace", self)
        self.buttonSpectrumClear = QPushButton("Clear", self)
        self.buttonSpectrumDelete = QPushButton("Delete", self)
        
        self.listParams_label = []
        self.listParams_label_low = []
        self.listParams_label_high = []
        self.listParams_label_bins = []
        self.listParams = []
        self.listParams_low = []
        self.listParams_high = []
        self.listParams_bins = []
        for i in range(2):
            self.listParams_label.append(QLabel("Parameter"))
            self.listParams_label_low.append(QLabel("Low"))
            self.listParams_label_high.append(QLabel("High"))
            self.listParams_label_bins.append(QLabel("Bins"))
            self.listParams.append(QComboBox())
            self.listParams_low.append(QLineEdit(self))
            self.listParams_high.append(QLineEdit(self))
            self.listParams_bins.append(QLineEdit(self))
        
        deflayout = QGridLayout()
        deflayout.addWidget(self.spectrum_name_label, 0, 0)
        deflayout.addWidget(self.spectrum_name, 1, 0)
        deflayout.addWidget(self.buttonSpectrumCreate, 1, 1)
        deflayout.addWidget(self.buttonSpectrumClear, 1, 2)
        deflayout.addWidget(self.buttonSpectrumDelete, 1, 3)
        for i in range(2):
            deflayout.addWidget(self.listParams_label[i], i*2+2, 0)
            deflayout.addWidget(self.listParams_label_low[i] , i*2+2, 1)
            deflayout.addWidget(self.listParams_label_high[i] , i*2+2, 2)
            deflayout.addWidget(self.listParams_label_bins[i], i*2+2, 3)
            deflayout.addWidget(self.listParams[i], i*2+3, 0)
            deflayout.addWidget(self.listParams_low[i], i*2+3, 1)
            deflayout.addWidget(self.listParams_high[i], i*2+3, 2)
            deflayout.addWidget(self.listParams_bins[i], i*2+3, 3)
        deflayout.setSpacing(1)
                    
        spectrumCreateBox.setLayout(deflayout)
        spectrumCreateBox.setMaximumHeight(self.width)

        return spectrumCreateBox

    def create_algoBox(self):
        algoBox = QGroupBox("Special Functions")
        
        self.peak_name_label = QLabel("Peak Finding")
        self.peak_option = QPushButton("Options", self)
        self.cluster_name_label = QLabel("Clustering 2D")
        self.cluster_option = QPushButton("Options", self)

        vlayout = QVBoxLayout()
        vlayout.addWidget(self.peak_name_label)        
        vlayout.addWidget(self.peak_option)
        vlayout.addWidget(self.cluster_name_label)
        vlayout.addWidget(self.cluster_option)
        vlayout.addStretch()
        
        algoBox.setLayout(vlayout)
        algoBox.setMaximumHeight(self.width)

        return algoBox
        
    
    def create_lstBox(self):
        spectrumListBox = QGroupBox("Spectrum List")

        self.histo_list_label = QLabel("List of Spectra")
        self.histo_list = QComboBox()
        self.histo_geo_label = QLabel("Geometry")
        self.histo_geo_row = QComboBox()
        self.histo_geo_col = QComboBox()
        self.histo_geo_add = QPushButton("Add", self)
        self.histo_geo_update = QPushButton("Update", self)
        self.histo_geo_delete = QPushButton("Erase", self)
        self.histo_geo_all = QCheckBox("Select All",self)
        self.fit_label = QLabel("Fitting Functions 1D")
        self.fit_list = QComboBox()
        self.fit_list.addItem("Gaussian")
        self.fit_list.addItem("Expo")
        self.fit_list.addItem("Pol1")
        self.fit_list.addItem("Pol2")
        self.fit_list.addItem("Pol3")
        self.fit_list.addItem("GPol1")
        self.fit_list.addItem("GPol2")                
        self.fit_list.addItem("Custom")
        self.fit_button = QPushButton("Fit", self)
        self.fit_range_label = QLabel("Fitting Range")
        self.fit_range_label_min = QLabel("Min X")
        self.fit_range_label_max = QLabel("Max X")
        self.fit_range_min = QLineEdit(self)
        self.fit_range_max = QLineEdit(self)
        self.fit_range_label_min_2 = QLabel("Min Y")
        self.fit_range_label_max_2 = QLabel("Max Y")
        self.fit_range_min_2 = QLineEdit(self)
        self.fit_range_max_2 = QLineEdit(self)        
        self.fit_results_label = QLabel("Fit output")
        self.fit_results = QTextEdit()
        self.fit_results.setReadOnly(True)
        
        hl = QHBoxLayout()
        hl.addWidget(self.histo_geo_add)
        hl.addWidget(self.histo_geo_update)
        hl.addWidget(self.histo_geo_delete)

        hl4 = QHBoxLayout()
        hl4.addWidget(self.histo_geo_row)
        hl4.addWidget(self.histo_geo_col)
        hl4.addWidget(self.histo_geo_all)
        for i in range(1,6):
            self.histo_geo_row.addItem(str(i))
            self.histo_geo_col.addItem(str(i))
        self.row = int(self.histo_geo_row.currentText())
        self.col = int(self.histo_geo_col.currentText())
        self.oldrow = self.row
        self.oldcol = self.col
                        
        vlayout = QVBoxLayout()
        vlayout.addWidget(self.histo_list_label)
        vlayout.addWidget(self.histo_list)
        vlayout.addWidget(self.histo_geo_label)
        vlayout.addLayout(hl4)
        vlayout.addLayout(hl)
        vlayout.addStretch()
                
        vlayout2a = QHBoxLayout()
        vlayout2a.addWidget(self.fit_range_label_min)
        vlayout2a.addWidget(self.fit_range_label_max)
        
        vlayout2b = QHBoxLayout()
        vlayout2b.addWidget(self.fit_range_min)
        vlayout2b.addWidget(self.fit_range_max)

        vlayout2c = QHBoxLayout()
        vlayout2c.addWidget(self.fit_range_label_min_2)
        vlayout2c.addWidget(self.fit_range_label_max_2)
        
        vlayout2d = QHBoxLayout()
        vlayout2d.addWidget(self.fit_range_min_2)
        vlayout2d.addWidget(self.fit_range_max_2)        
                
        vlayout2 = QVBoxLayout()
        vlayout2.addWidget(self.fit_label)
        vlayout2.addWidget(self.fit_list)
        vlayout2.addWidget(self.fit_button)
        vlayout2.addWidget(self.fit_range_label)
        vlayout2.addLayout(vlayout2a)
        vlayout2.addLayout(vlayout2b)
        vlayout2.addLayout(vlayout2c)
        vlayout2.addLayout(vlayout2d)        
        vlayout2.addStretch()
        
        vlayout3 = QVBoxLayout()
        vlayout3.addWidget(self.fit_results_label)
        vlayout3.addWidget(self.fit_results)
                
        lstlayout = QHBoxLayout()
        lstlayout.addLayout(vlayout)
        lstlayout.addLayout(vlayout2)
        lstlayout.addLayout(vlayout3)
        
        spectrumListBox.setLayout(lstlayout)
        spectrumListBox.setMaximumHeight(self.width)
                
        return spectrumListBox
                                                                                        
