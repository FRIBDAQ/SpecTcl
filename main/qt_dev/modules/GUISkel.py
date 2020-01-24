#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import json
import httplib2
import pandas as pd

import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm
#import seaborn as sns; sns.set(color_codes=True)

from PyQt5 import QtCore
from PyQt5.QtWidgets import *
import CPyConverter as cpy

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

##############################
# GUI class
##############################            
            
class GUIClass(QDialog):
    def __init__(self, parent=None):
        super(GUIClass, self).__init__(parent)

        self.timer = QtCore.QTimer()
        self.setWindowFlags(QtCore.Qt.WindowMaximizeButtonHint | QtCore.Qt.WindowMinimizeButtonHint)
        self.originalPalette = QApplication.palette()
        self.clear = False

        # default value for plot y-axis max
        self.yhigh = 1024
        # indices for filling the geometry
        self.index = 0
        
        # dictionaries for parameters
        self.param_list = {}
        # dataframe for spectra
        self.spectrum_list = pd.DataFrame()
        
        # for rebinning we want to be able to compare old and new values
        self.oldXmin = 0
        self.oldYmin = 0
        self.oldXmax = 0
        self.oldYmax = 0        
        self.oldXbins = 0
        self.oldYbins = 0                
        
        ########################
        # Button creation
        ########################

        self.exitButton = QPushButton("Exit", self)
        self.updateButton = QPushButton("Update", self)        

        self.slider_label = QLabel("Refresh Interval (s)")                        
        self.slider = QSlider(QtCore.Qt.Horizontal, self)
        self.slider.setMinimum(0)
        self.slider.setMaximum(10)
        
        # spectrum type box
        self.button1D = QRadioButton("1D")
        self.button2D = QRadioButton("2D")
        self.button2D_option = QComboBox()
        self.button2D_option.addItem("Dark")
        self.button2D_option.addItem("Light")                
        self.button3D = QRadioButton("3D")        
        self.button3D_option = QComboBox()
        self.button3D_option.addItem("Line")
        self.button3D_option.addItem("Scatter")
        self.button3D_option.addItem("Contour")                
        self.button3D_option.addItem("Wireframe")                
        self.button3D_option.addItem("Surface")                
        self.button3D_option.addItem("Surface 2") 
        
        # spectrum definition box
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
        for i in range(3):
            self.listParams_label.append(QLabel("Parameter"))
            self.listParams_label_low.append(QLabel("Low"))
            self.listParams_label_high.append(QLabel("High"))            
            self.listParams_label_bins.append(QLabel("Bins"))
            self.listParams.append(QComboBox())            
            self.listParams_low.append(QLineEdit(self))
            self.listParams_high.append(QLineEdit(self))
            self.listParams_bins.append(QLineEdit(self))            
        # disable third variable
        self.listParams[2].setEnabled(False)
        self.listParams_low[2].setReadOnly(True)
        self.listParams_high[2].setReadOnly(True)
        self.listParams_bins[2].setReadOnly(True)        
        
        # spectrum list box
        self.histo_list_label = QLabel("List of Spectra")
        self.histo_list = QComboBox()
        self.histo_geo_label = QLabel("Geometry")        
        self.histo_geo_row = QComboBox()
        self.histo_geo_col = QComboBox()        
        self.histo_geo_add = QPushButton("Add", self)
        self.histo_geo_reset = QPushButton("Update", self)
        self.histo_geo_delete = QPushButton("Erase", self)                
        self.gates_name_label = QLabel("Gate name")                
        self.gates_name = QLineEdit(self)
        self.gates_list_label = QLabel("List of Gates")
        self.gates_list = QComboBox()
        self.gates_create = QPushButton("Create", self)
        self.gates_apply = QPushButton("Apply", self)
        self.gates_delete = QPushButton("Delete", self)                
        self.fit_label = QLabel("Fitting Functions")
        self.fit_list = QComboBox()
        self.fit_list.addItem("Gaussian")
        self.fit_list.addItem("Expo")        
        self.fit_list.addItem("Pol1")
        self.fit_list.addItem("Pol2")
        self.fit_list.addItem("Pol3")                                
        self.fit_list.addItem("Custom")                                        
        self.fit_button = QPushButton("Fit", self)
        self.fit_range_label = QLabel("Fitting Range")
        self.fit_range_label_min = QLabel("Min")
        self.fit_range_label_max = QLabel("Max")        
        self.fit_range_min = QLineEdit(self)
        self.fit_range_max = QLineEdit(self)                
        self.fit_results_label = QLabel("Fit Result")
        self.fit_results = QTextEdit()
        self.fit_results.setReadOnly(True)

        
        
        #######################
        # Main layout GUI
        #######################        
        
        self.create_spectrumTypeBox()
        self.create_spectrumListBox()
        self.create_spectrumCreateBox()
        self.create_plotGroupBox()
        
        topLayout = QHBoxLayout()
        topLayout.addWidget(self.exitButton)
        topLayout.addWidget(self.updateButton)        
        topLayout.addWidget(self.slider_label)        
        topLayout.addWidget(self.slider)        
        topLayout.addStretch()
        
        layout = QHBoxLayout()
        layout.addWidget(self.spectrumTypeBox)
        layout.addWidget(self.spectrumCreateBox)
        layout.addWidget(self.spectrumListBox)        
        
        mainLayout = QVBoxLayout()
        mainLayout.addLayout(topLayout)
        mainLayout.addLayout(layout)
        mainLayout.addWidget(self.plotGroupBox)
        
        self.setLayout(mainLayout)
        
        #################
        # Signals
        #################

        self.exitButton.clicked.connect(self.close)
        self.updateButton.clicked.connect(self.update)                
        
        self.slider.valueChanged.connect(self.self_update)

        self.histo_geo_add.clicked.connect(self.add_histogram)
        
        self.plusButton.clicked.connect(self.zoomIn)
        self.minusButton.clicked.connect(self.zoomOut)        
        
        self.buttonSpectrumCreate.clicked.connect(self.createreplace)
        
        self.at_startup()
        self.histo_geo_reset.clicked.connect(self.update_selected_spectrum)

    def create_plotGroupBox(self):

        self.plotGroupBox = QGroupBox("")

        # plot canvas creation
        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)
        self.plusButton = QPushButton("+", self)
        self.minusButton = QPushButton("-", self)        
        self.toolbar.addWidget(self.plusButton)
        self.toolbar.addWidget(self.minusButton)                
        
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)        
        #layout.addWidget(self.navigation_toolbar)
        layout.addWidget(self.canvas)
        self.plotGroupBox.setLayout(layout)
        
    def create_spectrumTypeBox(self):
        self.spectrumTypeBox = QGroupBox("Spectrum Type")

        layout = QVBoxLayout()        
        layout.addWidget(self.button1D)
        hl2 = QHBoxLayout()
        hl2.addWidget(self.button2D)
        hl2.addWidget(self.button2D_option)        
        hl = QHBoxLayout()
        hl.addWidget(self.button3D)
        hl.addWidget(self.button3D_option)
        layout.addLayout(hl2)
        layout.addLayout(hl)        
        layout.addStretch(1)
        
        self.spectrumTypeBox.setLayout(layout)
        self.spectrumTypeBox.setMaximumHeight(225)
        self.spectrumTypeBox.setMaximumWidth(200)
        
    def create_spectrumListBox(self):
        self.spectrumListBox = QGroupBox("Spectrum List")

        hl = QHBoxLayout()
        hl.addWidget(self.histo_geo_add)
        hl.addWidget(self.histo_geo_reset)
        hl.addWidget(self.histo_geo_delete)        

        hl2 = QHBoxLayout()
        hl2.addWidget(self.gates_name)
        hl2.addWidget(self.gates_create)

        hl3 = QHBoxLayout()
        hl3.addWidget(self.gates_list)
        hl3.addWidget(self.gates_apply)
        hl3.addWidget(self.gates_delete)                                

        hl4 = QHBoxLayout()
        hl4.addWidget(self.histo_geo_row)
        for i in range(1,6):
            self.histo_geo_row.addItem(str(i))
            self.histo_geo_col.addItem(str(i))            
        hl4.addWidget(self.histo_geo_col)
        
        vlayout = QVBoxLayout()
        vlayout.addWidget(self.histo_list_label)
        vlayout.addWidget(self.histo_list)
        vlayout.addWidget(self.histo_geo_label)        
        vlayout.addLayout(hl4)
        vlayout.addLayout(hl)
        vlayout.addWidget(self.gates_name_label)
        vlayout.addLayout(hl2)
        vlayout.addWidget(self.gates_list_label)                
        vlayout.addLayout(hl3)
        vlayout.addStretch()
        
        vlayout2a = QHBoxLayout()
        vlayout2a.addWidget(self.fit_range_label_min)
        vlayout2a.addWidget(self.fit_range_label_max)        

        vlayout2b = QHBoxLayout()        
        vlayout2b.addWidget(self.fit_range_min)
        vlayout2b.addWidget(self.fit_range_max)        
        #        self.fit_range_min.setText(self.par_x_low.text())
        #        self.fit_range_max.setText(self.par_x_high.text())
        
        vlayout2 = QVBoxLayout()        
        vlayout2.addWidget(self.fit_label)
        vlayout2.addWidget(self.fit_list)
        vlayout2.addWidget(self.fit_button)
        vlayout2.addWidget(self.fit_range_label)
        vlayout2.addLayout(vlayout2a)
        vlayout2.addLayout(vlayout2b)        
        vlayout2.addStretch()

        vlayout3 = QVBoxLayout()                
        vlayout3.addWidget(self.fit_results_label)
        vlayout3.addWidget(self.fit_results)                                        
        
        # fitting

        hlayout = QHBoxLayout()
        hlayout.addLayout(vlayout)
        hlayout.addLayout(vlayout2)
        hlayout.addLayout(vlayout3)        
        
        
        self.spectrumListBox.setLayout(hlayout)
        self.spectrumListBox.setMaximumHeight(225)
        
    def create_spectrumCreateBox(self):
        self.spectrumCreateBox = QGroupBox("Spectrum Definition")        

        layout = QGridLayout()
        layout.addWidget(self.spectrum_name_label, 0, 0)
        layout.addWidget(self.spectrum_name, 1, 0)
        layout.addWidget(self.buttonSpectrumCreate, 1, 1)
        layout.addWidget(self.buttonSpectrumClear, 1, 2)                        
        layout.addWidget(self.buttonSpectrumDelete, 1, 3)                
        for i in range(3):
            layout.addWidget(self.listParams_label[i], i*2+2, 0)
            layout.addWidget(self.listParams_label_low[i] , i*2+2, 1)
            layout.addWidget(self.listParams_label_high[i] , i*2+2, 2)
            layout.addWidget(self.listParams_label_bins[i], i*2+2, 3)
            layout.addWidget(self.listParams[i], i*2+3, 0)
            layout.addWidget(self.listParams_low[i], i*2+3, 1)
            layout.addWidget(self.listParams_high[i], i*2+3, 2)
            layout.addWidget(self.listParams_bins[i], i*2+3, 3)
        layout.setSpacing(1)
        
        self.spectrumCreateBox.setLayout(layout)
        self.spectrumCreateBox.setMaximumHeight(250)
        self.spectrumCreateBox.setMaximumWidth(400)

    # At startup we need the full parameter list as dictionary - (id,name) 

    def at_startup(self):
        self.create_parameter_list()

    def create_parameter_list(self):
        self.update_parameter_list()
        for key, value in self.param_list.items():
            for i in range (3):
                self.listParams[i].addItem(value)
        
    def update_parameter_list(self):
        tmpl = httplib2.Http().request("http://localhost:8080/spectcl/parameter/list")[1]
        tmp = json.loads(tmpl.decode())
        tmpid = []
        tmpname = []
        for dic in tmp['detail']:
            for key in dic:
                if key == 'id':
                    tmpid.append(dic[key])
                elif key == 'name':
                    tmpname.append(dic[key])
        ziplst = zip(tmpid, tmpname)
        self.param_list = dict(ziplst)


    def self_update(self):
        self.slider_label.setText("Refresh interval ({} s)".format(self.slider.value()))
        if self.slider.value() != 0:
            self.timer.setInterval(1000*int(self.slider.value()))
            self.timer.timeout.connect(self.update)
            self.timer.start()

    # when clicking update we need to access the shared memory and
    # update the spectrum information in the data frame

    def update(self):
        # creates a dataframe for spectrum info
        s = cpy.CPyConverter().Update()
        self.spectrum_list = pd.DataFrame(
            {'id': s[0],
             'names': s[1],
             'dim' : s[2],
             'binx': s[3],
             'minx': s[4],
             'maxx': s[5],
             'biny': s[6],
             'miny': s[7],
             'maxy': s[8],
             'data': s[9]}
        )
        # add list of parameters for each spectrum to the dataframe
        self.create_spectrum_parameters();
        # print for debug
        #print(self.spectrum_list)
        #self.print_spectrum("A",0)
        # update the list of defined spectra
        self.create_spectrum_list()        
        # update information for current selected spectrum
        self.update_selected_spectrum()
        '''
        # debugging leftovers
        s, f = cpy.CPyConverter().DebugFillSpectra()
        for x in f:
            print(x)
        '''        

    def print_spectrum(self, name, index):
        select = self.spectrum_list['names'] == name
        df = self.spectrum_list.loc[select]
        w = df.iloc[index]['data']
        print(w)
        
    # add list of parameters for each spectrum to the dataframe        
    def create_spectrum_parameters(self):
        tmpl = httplib2.Http().request("http://localhost:8080/spectcl/spectrum/list")[1]
        tmp = json.loads(tmpl.decode())
        tmppar = []
        for dic in tmp['detail']:
            for key in dic:
                if key == 'parameters':
                   tmppar.append(dic[key]) 
        # adds list to dataframe           
        self.spectrum_list['parameters'] = tmppar
        
    # update the list of defined spectra
    def create_spectrum_list(self):
        for name in self.spectrum_list['names']:
            if self.histo_list.findText(name) == -1:
                self.histo_list.addItem(name)

    # update information for current selected spectrum
    def update_selected_spectrum(self):
        # access name and id for the current spectrum
        current_spec = str(self.histo_list.currentText())
        idx = self.histo_list.currentIndex()
        # update info on gui
        self.spectrum_name.setText(current_spec)
        # extra data from data frame
        select = self.spectrum_list['names'] == current_spec
        df = self.spectrum_list.loc[select]
        dim = df.iloc[idx]['dim']
        if dim == 1:
            self.button1D.setChecked(True)
        else:
            self.button2D.setChecked(True)
        self.check_histogram();
        for i in range(dim):
            index = self.listParams[i].findText(df.iloc[idx]['parameters'][i], QtCore.Qt.MatchFixedString)
            if index >= 0:
                self.listParams[i].setCurrentIndex(index)
            if i == 0:
                self.listParams_low[i].setText(str(df.iloc[idx]['minx']))
                self.listParams_high[i].setText(str(df.iloc[idx]['maxx']))
                self.listParams_bins[i].setText(str(df.iloc[idx]['binx']))
            else :
                self.listParams_low[i].setText(str(df.iloc[idx]['miny']))
                self.listParams_high[i].setText(str(df.iloc[idx]['maxy']))
                self.listParams_bins[i].setText(str(df.iloc[idx]['biny']))                

        # plotting histogram
        ix = self.setHistoPosition()        
        print(ix)
        self.current_histo = self.plot_histogram(current_spec, ix, index)

    def add_histogram(self):
        index = self.setHistoPosition()
        current_spec = str(self.histo_list.currentText())
        self.current_histo = self.plot_histogram(current_spec, index)
        
    def setHistoPosition(self):
        self.row = int(self.histo_geo_row.currentText())
        self.col = int(self.histo_geo_col.currentText())
        print(self.row, self.col)
        if (self.row ==1 and self.col == 1):
            return 1
        elif (self.index == self.row*self.col):
            self.index = 1
            return self.index
        else:
            self.index += 1
            print(self.index)            
            return self.index
        
        
    def zoomIn(self):
        ylow, self.yhigh = self.current_histo.get_ylim()
        self.yhigh /= 2
        self.current_histo.set_ylim(0,self.yhigh)
        self.canvas.draw()
        
    def zoomOut(self):
        ylow, self.yhigh = self.current_histo.get_ylim()
        self.yhigh *= 2
        self.current_histo.set_ylim(0,self.yhigh)
        self.canvas.draw()
        
    def plot_histogram(self, name, idx, index):
        self.figure.clear()
        a = self.fill_histogram(name, int(self.histo_geo_row.currentText()), int(self.histo_geo_col.currentText()), idx, index)
        if self.isEmpty == False:
            a.set_ylim(0,self.yhigh)
        self.canvas.draw()
        return a

    def create_range(self, bins, vmin, vmax):
        x = []
        step = (vmax-vmin)/bins
        for i in np.arange(vmin, vmax, step):
            x.append(i)
        return x

    def fill_histogram(self, name, r, c, idx, index):

        a = self.figure.add_subplot(r, c, idx)
        if (name == ''):
            return a
        else:
            select = self.spectrum_list['names'] == name
            df = self.spectrum_list.loc[select]
            w = df.iloc[index]['data']
        if (sum(w) == 0):
            self.isEmpty = True
            a.set_ylim(0,512)
            QMessageBox.about(self, "Warning", "The shared memory is still empty...")
        else:
            self.isEmpty = False
            dim = df.iloc[index]['dim']        
            xbins = self.listParams_bins[0].text().replace(' ', '')
            xmin = self.listParams_low[0].text().replace(' ', '')
            xmax = self.listParams_high[0].text().replace(' ', '')
            self.oldXmin = float(xmin)
            self.oldXmax = float(xmax)        
            self.oldXbins = int(xbins)
            if (dim == 1):
                a.set_title(name)
                X = self.create_range(int(xbins), float(xmin), float(xmax))
                a.hist(X, len(X), weights=w, range=[float(xmin),float(xmax)], histtype='step')
                plt.xlim(xmin=float(xmin), xmax=float(xmax))
                x_label = str(df.iloc[index]['parameters'])
                plt.xlabel(x_label,fontsize=10)
            elif (dim == 2):
                ybins = self.listParams_bins[1].text().replace(' ', '')
                ymin = self.listParams_low[1].text().replace(' ', '')
                ymax = self.listParams_high[1].text().replace(' ', '')
                self.oldYmin = float(ymin)
                self.oldYmax = float(ymax)        
                self.oldYbins = int(ybins)
                a.set_title(name)
                if (self.button2D_option.currentText() == 'Dark'):
                    palette = 'viridis'
                else:
                    palette = 'PuBu'
                    a.imshow(w, interpolation='none', extent=[float(xmin),float(xmax),float(ymin),float(ymax)], aspect='auto', origin='lower', vmin=0, vmax=10, cmap='Blues')
                    axs=plt.gca() #get the current axes
                    color_map=axs.get_children()[2] #get the mappable, the 1st and the 2nd are the x and y axes
                    plt.colorbar(color_map, ax=axs)
                    x_label = str(df.iloc[index]['parameters'][0])
                    y_label = str(df.iloc[index]['parameters'][1])            
                    plt.xlabel(x_label,fontsize=10)
                    plt.ylabel(y_label,fontsize=10)                        
            return a

    def createreplace(self):
        a = self.rebinning()
        self.canvas.draw()
        
    # rebinning - based on Kernel Density Estimation (KDE)
    def rebinning(self):
        print("To be implemented...see kde.py")
        # for 1D histograms
        if self.button1D.isChecked():
            if (str(self.listParams_bins[0].text()) != str(self.oldXbins)):
                print("rebin 1d...")
            else:
                print("Nothing to do here")
        # for 2D histograms
        elif self.button2D.isChecked():
            if (int(self.listParams_bins[0].text()) != self.oldXbins or
                int(self.listParams_bins[1].text()) != self.oldYbins):
                X = self.create_range(int(self.listParams_bins[0].text()), float(self.listParams_low[0].text()), float(self.listParams_high[0].text()))
                Y = self.create_range(int(self.listParams_bins[1].text()), float(self.listParams_low[1].text()), float(self.listParams_high[1].text()))                              
                data = np.vstack([X, Y])
                print(data)
                kde = gaussian_kde(data)
                '''
                xgrid = np.linspace(float(self.listParams_low[0].text()), float(self.listParams_high[0].text()), int(self.listParams_bins[0].text()))
                ygrid = np.linspace(float(self.listParams_low[1].text()), float(self.listParams_high[1].text()), int(self.listParams_bins[1].text()))
                Xgrid, Ygrid = np.meshgrid(xgrid, ygrid)
                Z = kde.evaluate(np.vstack([Xgrid.ravel(), Ygrid.ravel()]))
                a.imshow(w, interpolation='none', extent=[float(self.listParams_low[0].text()),float(self.listParams_high[0].text()),
                                                          float(self.listParams_low[1].text()),float(self.listParams_high[1].text())],
                         aspect='auto', origin='lower', vmin=0, vmax=10, cmap='Blues')                
                return a
                '''
            else:
                print("Nothing to do here")
        else:
            print("Nothing to do here")                                
            
    # this function deals with what will be shown and what not
    def check_histogram(self):
        if self.button1D.isChecked():
            self.create_disable2D(True)
            self.create_disable3D(True)            
        elif self.button2D.isChecked():
            self.create_disable2D(False)
            self.create_disable3D(True)                        
        else:
            self.create_disable2D(False)
            self.create_disable3D(False)
            
    def create_disable2D(self, value):
        if value==True:
            self.listParams[1].setEnabled(False)
            self.listParams_low[1].setText("")
            self.listParams_low[1].setReadOnly(True)
            self.listParams_high[1].setText("")
            self.listParams_high[1].setReadOnly(True)
            self.listParams_bins[1].setText("")
            self.listParams_bins[1].setReadOnly(True)            
        else:
            self.listParams[1].setEnabled(True)
            self.listParams_low[1].setReadOnly(False)
            self.listParams_high[1].setReadOnly(False)            
            self.listParams_bins[1].setReadOnly(False)

    def create_disable3D(self, value):
        if value==True:
            self.listParams[2].setEnabled(False)
            self.listParams_low[2].setText("")
            self.listParams_low[2].setReadOnly(True)
            self.listParams_high[2].setText("")
            self.listParams_high[2].setReadOnly(True)
            self.listParams_bins[2].setText("")
            self.listParams_bins[2].setReadOnly(True)                        
        else:
            self.listParams[2].setEnabled(True)
            self.listParams_low[2].setReadOnly(False)
            self.listParams_high[2].setReadOnly(False)            
            self.listParams_bins[2].setReadOnly(False)            
            
'''
    ####################
    # Gate list
    ####################        
        
    def update_gates(self, memory):
        gates = memory.get_gates();
        if len(gates) != self.gates_size:
            GUIClass.create_gatelist(self,gates)
            self.gates_size = len(gates)        

    def create_gatelist(self, variable):
        for item in variable:
            for key in item:
                if key == 'name':
                    self.gates_list.addItem(item[key])                                            



    def get_listParamsIndex(self):
        for i in range(3):
            self.index_list.append(self.listParams[i].currentIndex())
        
    def update_param(self, variable):
        GUIClass.get_listParamsIndex(self);
        for index in self.index_list:
            for item in variable:
                for key in item:
                    if key == 'id' and item[key] == index :
                        for i in range(3):
                            self.listParams_low[i].setText(str(item['low']))
                            self.listParams_high[i].setText(str(item['hi']))
                            self.listParams_bins[i].setText(str(item['bins']))


'''
