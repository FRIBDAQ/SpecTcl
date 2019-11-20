#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())
#import threading
#import dash
#import dash_core_components as dcc
#import dash_html_components as html
#import dash_renderer as dren
#import pandas_datareader.data as web
#import datetime
#import plotly
#import random
#import time
#import plotly.graph_objs as go
import numpy as np                
import scipy as sp
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.gridspec as gridspec
import matplotlib.path as mpltPath

import CPyConverter as cpy

from PyQt5 import QtCore
from PyQt5.QtWidgets import *
#from collections import deque
#from dash.dependencies import Input, Output, Event
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from itertools import chain, compress, zip_longest
from scipy import optimize
from scipy.stats import *

############################
## widget class and defs
############################

# dashboard for plots
def run_dash():
    app = dash.Dash()

    max_length = 50
    times = deque(maxlen=max_length)
    chn0 = deque(maxlen=max_length)
    chn1 = deque(maxlen=max_length)
    chn2 = deque(maxlen=max_length)
    chn3 = deque(maxlen=max_length)
    chn4 = deque(maxlen=max_length)
    chn5 = deque(maxlen=max_length)
    
    data_dict = {"Channel 0": chn0,
                 "Channel 1": chn1,
                 "Channel 2": chn2,
                 "Channel 3": chn3,
                 "Channel 4": chn4,
                 "Channel 5": chn5}

    def update_values(times, chn0, chn1, chn2, chn3, chn4, chn5):
        
        times.append(time.time())
        if len(times) == 1:
            #starting relevant values
            chn0.append(random.randrange(180,230))
            chn1.append(random.randrange(95,115))
            chn2.append(random.randrange(170,220))
            chn3.append(random.randrange(1000,9500))
            chn4.append(random.randrange(30,140))
            chn5.append(random.randrange(10,90))
        else:
            for data_of_interest in [chn0, chn1, chn2, chn3, chn4, chn5]:
                data_of_interest.append(data_of_interest[-1]+data_of_interest[-1]*random.uniform(-0.0001,0.0001))
                
        return times, chn0, chn1, chn2, chn3, chn4, chn5

    times, chn0, chn1, chn2, chn3, chn4, chn5 = update_values(times, chn0, chn1, chn2, chn3, chn4, chn5)

    app.layout = html.Div([
            html.Div([
                html.H2('Sample Data',
                        style={'float': 'left',
                        }),
            ]),
            dcc.Dropdown(id='sample-data-name',
                         options=[{'label': s, 'value': s}
                                  for s in data_dict.keys()],
                         value=['Channel 0','Channel 1','Channel 2'],
                         multi=True
            ),
            html.Div(children=html.Div(id='graphs'), className='row'),
            dcc.Interval(
                id='graph-update',
                interval=1000),
            ], className="container",style={'width':'98%','margin-left':10,'margin-right':10,'max-width':50000})

    @app.callback(
            dash.dependencies.Output('graphs','children'),
            [dash.dependencies.Input('sample-data-name', 'value')],
            events=[dash.dependencies.Event('graph-update', 'interval')]
    )

    def update_graph(data_names):
        graphs = []
        update_values(times, chn0, chn1, chn2, chn3, chn4, chn5)
        if len(data_names)>2:
            class_choice = 'col s12 m6 l4'
        elif len(data_names) == 2:
            class_choice = 'col s12 m6 l6'
        else:
            class_choice = 'col s12'

            
        for data_name in data_names:

            data = go.Scatter(
                x=list(times),
                y=list(data_dict[data_name]),
                name='Scatter',
                fill="tozeroy",
                fillcolor="#6897bb"
            )
            
            graphs.append(html.Div(dcc.Graph(
                id=data_name,
                animate=True,
                figure={'data': [data],'layout' : go.Layout(xaxis=dict(range=[min(times),max(times)]),
                                                            yaxis=dict(range=[min(data_dict[data_name]),max(data_dict[data_name])]),
                                                            margin={'l':50,'r':1,'t':45,'b':1},
                                                            title='{}'.format(data_name))}
            ), className=class_choice))
        
        return graphs
        
    external_css = ["https://cdnjs.cloudflare.com/ajax/libs/materialize/0.100.2/css/materialize.min.css"]
    for css in external_css:
        app.css.append_css({"external_url": css})
        
    external_js = ['https://cdnjs.cloudflare.com/ajax/libs/materialize/0.100.2/js/materialize.min.js']
    for js in external_css:
        app.scripts.append_script({'external_url': js})
    
    app.run_server(debug=False)

    ##############################
    # Gate builder classes
    ##############################
class Gate1DDrawer:
    def __init__(self, line):
        self.line = line
        self.xs = list(line.get_xdata())
        self.ys = list(line.get_ydata())
        self.cid = line.figure.canvas.mpl_connect('button_press_event', self)
        self.l1 = []
        self.l2 = []        
        
    def __call__(self, event):
        if event.inaxes !=self.line.axes: return

        if (len(self.xs)<2):
            self.xs.append(event.xdata)
        else:
            self.xs.clear()
            self.xs.append(event.xdata)            

        c = 'r'
        self.l1 = plt.axvline(x=self.xs[0], color=c)
        if (len(self.xs)==2):
              self.l2 = plt.axvline(x=self.xs[1], color=c)        
        self.line.figure.canvas.draw()        
        
        if (len(self.xs)==2 and self.xs[0]>self.xs[1]):
            tmp = self.xs[1]
            self.xs[1] = self.xs[0]
            self.xs[0] = tmp
            
class Gate2DDrawer:
    def __init__(self, line):
        self.line = line
        self.xs = list(line.get_xdata())
        self.ys = list(line.get_ydata())
        self.cid = line.figure.canvas.mpl_connect('button_press_event', self)
        self.X = []
        
    def __call__(self, event):
        if event.inaxes !=self.line.axes: return
        if event.dblclick:
            self.xs.append(self.xs[-1])
            self.xs.append(self.xs[0])
            self.ys.append(self.ys[-1])
            self.ys.append(self.ys[0])
            self.line.set_data(self.xs, self.ys)
            self.line.figure.canvas.draw()
            self.X = [self.xs, self.ys]
        else:
            self.xs.append(event.xdata)
            self.ys.append(event.ydata)
            self.line.set_data(self.xs, self.ys)
            self.line.figure.canvas.draw()

    ##############################
    # GUI class
    ##############################            
            
class GUIClass(QDialog):
    def __init__(self, parent=None):
        super(GUIClass, self).__init__(parent)

        self.originalPalette = QApplication.palette()
        self.clear = False
        
        ########################
        # Button creation
        ########################

        # top layout
        self.exitButton = QPushButton("Exit", self)

        # spectrum type
        self.button1D = QRadioButton("1D")
        self.button1D.setChecked(True)
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
        
        # spectrum creation
        self.data_type_g = QRadioButton("Gaussian")
        self.data_type_g.setChecked(True)
        self.data_type_exp = QRadioButton("Expo")        
        self.data_type_p1 = QRadioButton("Pol1")
        self.data_type_p2 = QRadioButton("Pol2")
        self.data_type_p3 = QRadioButton("Pol3")        
        self.spectrum_name_label = QLabel("Spectrum Name")                
        self.spectrum_name = QLineEdit(self)
        self.buttonSpectrumCreate = QPushButton("Create/Replace", self)
        self.buttonSpectrumClear = QPushButton("Clear", self)
        self.buttonSpectrumDelete = QPushButton("Delete", self)                
        self.par_x_label = QLabel("Parameter")        
        self.par_x = QComboBox()
        self.par_y_label = QLabel("Parameter")                
        self.par_y = QComboBox()
        self.par_z_label = QLabel("Parameter")                
        self.par_z = QComboBox()                
        self.par_x_low_label = QLabel("Low")        
        self.par_x_low = QLineEdit(self)
        self.par_x_high_label = QLabel("High")                
        self.par_x_high = QLineEdit(self)
        self.par_x_nbins_label = QLabel("Bins")        
        self.par_x_nbins = QLineEdit(self)
        self.par_y_low_label = QLabel("Low")                
        self.par_y_low = QLineEdit(self)
        self.par_y_high_label = QLabel("High")                
        self.par_y_high = QLineEdit(self)
        self.par_y_nbins_label = QLabel("Bins")                
        self.par_y_nbins = QLineEdit(self)
        self.par_z_low_label = QLabel("Low")                
        self.par_z_low = QLineEdit(self)
        self.par_z_high_label = QLabel("High")                
        self.par_z_high = QLineEdit(self)
        self.par_z_nbins_label = QLabel("Bins")                
        self.par_z_nbins = QLineEdit(self)                        

        self.histo_list_label = QLabel("List of Spectra")
        self.histo_list = QComboBox()
        self.histo_geo_label = QLabel("Geometry")        
        self.histo_geo_row = QComboBox()
        self.histo_geo_col = QComboBox()        
        self.histo_geo_add = QPushButton("Add", self)
        self.histo_geo_add.setEnabled(False)
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

        # create list of parameters, this has to be inherit from spectcl somehow, together with low, high, and nbins
        self.par_x_low.setText('0')
        self.par_x_high.setText('1000')
        self.par_x_nbins.setText('100')
        self.par_y_low.setText('0')
        self.par_y_high.setText('1000')
        self.par_y_nbins.setText('100')
        self.par_z_low.setText('0')
        self.par_z_high.setText('1000')
        self.par_z_nbins.setText('100')                                

        # variables
        self.data = []
        self.data_histo = []
        self.data_histo_fit = []        
        self.data_gate = []        
        self.rowT = 0
        self.colT = 0        
        
        # signals

        # button to generate data for testing purposes
        self.data_type_g.clicked.connect(self.btnstate)
        self.data_type_exp.clicked.connect(self.btnstate)        
        self.data_type_p1.clicked.connect(self.btnstate)
        self.data_type_p2.clicked.connect(self.btnstate)
        self.data_type_p3.clicked.connect(self.btnstate)        

        self.button1D.clicked.connect(self.check_histogramD)
        self.button2D.clicked.connect(self.check_histogramD)
        #        self.button3D.clicked.connect(self.check_histogramD)        
        if self.button1D.isChecked():
            self.check_histogramD()

        if self.data_type_g.isChecked():
            self.btnstate()
            
        self.exitButton.clicked.connect(self.close)

        self.buttonSpectrumCreate.clicked.connect(self.create_histogram)
        self.buttonSpectrumClear.clicked.connect(self.clear_histogram)
        self.buttonSpectrumDelete.clicked.connect(lambda: self.delete_histogram(self.buttonSpectrumDelete))        
        self.histo_geo_reset.clicked.connect(self.reset_histogram)
        self.histo_geo_delete.clicked.connect(lambda: self.delete_histogram(self.histo_geo_delete))                
        self.histo_geo_add.clicked.connect(self.add_plot)
        self.gates_create.clicked.connect(self.create_gate)        
        self.gates_apply.clicked.connect(self.apply_gate)
        self.gates_delete.clicked.connect(self.delete_gate)
        self.fit_button.clicked.connect(self.fit_histo)        
        
        ##################################
        # GUI structure
        ##################################        
        self.create_spectrumTypeBox()
        self.create_spectrumListBox()
        self.create_spectrumCreateBox()
        self.create_plotGroupBox()

    def btnstate(self):
        self.create_listParams(3)
                                          
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

        # spectrum and gate
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
        self.row = int(self.histo_geo_row.currentText())
        self.col = int(self.histo_geo_col.currentText())        
        
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
        self.fit_range_min.setText(self.par_x_low.text())
        self.fit_range_max.setText(self.par_x_high.text())
        
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
        layout.addWidget(self.par_x_label, 2, 0)
        layout.addWidget(self.par_x_low_label, 2, 1)
        layout.addWidget(self.par_x_high_label, 2, 2)
        layout.addWidget(self.par_x_nbins_label, 2, 3)
        layout.addWidget(self.par_x, 3, 0)
        layout.addWidget(self.par_x_low, 3, 1)
        layout.addWidget(self.par_x_high, 3, 2)
        layout.addWidget(self.par_x_nbins, 3, 3)
        layout.addWidget(self.par_y_label, 4, 0)
        layout.addWidget(self.par_y_low_label, 4, 1)
        layout.addWidget(self.par_y_high_label, 4, 2)
        layout.addWidget(self.par_y_nbins_label, 4, 3)
        layout.addWidget(self.par_y, 5, 0)
        layout.addWidget(self.par_y_low, 5, 1)
        layout.addWidget(self.par_y_high, 5, 2)
        layout.addWidget(self.par_y_nbins, 5, 3)
        layout.addWidget(self.par_z_label, 6, 0)
        layout.addWidget(self.par_z_low_label, 6, 1)
        layout.addWidget(self.par_z_high_label, 6, 2)
        layout.addWidget(self.par_z_nbins_label, 6, 3)
        layout.addWidget(self.par_z, 7, 0)
        layout.addWidget(self.par_z_low, 7, 1)
        layout.addWidget(self.par_z_high, 7, 2)
        layout.addWidget(self.par_z_nbins, 7, 3)
        layout.setSpacing(1)
        
        self.spectrumCreateBox.setLayout(layout)
        self.spectrumCreateBox.setMaximumHeight(250)
        self.spectrumCreateBox.setMaximumWidth(400)
        
    def create_plotGroupBox(self):

        self.plotGroupBox = QGroupBox("")

        # plot canvas creation
        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)

        layout = QVBoxLayout()        
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.plotGroupBox.setLayout(layout)

        #######################
        # Main layout GUI
        #######################        


        
        topLayout = QHBoxLayout()
        topLayout.addWidget(self.exitButton)
        topLayout.addWidget(self.data_type_g)
        topLayout.addWidget(self.data_type_exp)        
        topLayout.addWidget(self.data_type_p1)
        topLayout.addWidget(self.data_type_p2)
        topLayout.addWidget(self.data_type_p3)                

        layout = QHBoxLayout()
        layout.addWidget(self.spectrumTypeBox)
        layout.addWidget(self.spectrumCreateBox)
        layout.addWidget(self.spectrumListBox)

        mainLayout = QVBoxLayout()
        mainLayout.addLayout(topLayout)
        mainLayout.addLayout(layout)        
        mainLayout.addWidget(self.plotGroupBox)
        self.setLayout(mainLayout)
        
        ##################################
        # Methods
        ##################################        
        
        #    def getGateName(self):
        #        text, okPressed = QInputDialog.getText(self, "Gate creator", "Choose a name for the gate:", QLineEdit.Normal, "")
        #        if okPressed and text != "":
        #            self.combo_gate.addItem(text)

    def delete_histogram(self, button):
        # remove from QLineEdit and from QComboBox
        histo_name = self.spectrum_name.text()
        if (button.text()=='Delete'):
            self.histo_list.removeItem(self.histo_list.findText(histo_name))
        elif (button.text()=='Erase'):
            histo_name = self.histo_list.currentText()
            self.histo_list.removeItem(self.histo_list.currentIndex())

        # remove data from self.data_histo
        for elem in self.data_histo:
            if histo_name==elem[0]:
                self.data_histo.remove(elem)

        if (self.histo_list.count() != 0):
            self.reset_histogram()
        else:
            self.clear_histogram()
            
    def clear_histogram(self):
        histo_name = self.histo_list.currentText()
        self.clear = True
        self.plot_histogram()
        self.clear = False
        
    def create_histogram(self):
        self.histo_geo_add.setEnabled(True)
        histo_name = self.spectrum_name.text()
        if histo_name=="" :
            text, okPressed = QInputDialog.getText(self, "Spectrum name", "Please choose a name for the spectrum:", QLineEdit.Normal, "")
            if okPressed and text != "":
                histo_name = text
            else:
                histo_name = "default_histo"
        self.spectrum_name.setText(histo_name)
        # creating entry in combobox if it doesn't exist
        allItems = [self.histo_list.itemText(i) for i in range(self.histo_list.count())]
        result = histo_name in chain(*allItems)
        if result==False:
            self.histo_list.addItem(histo_name)
        else:
            # overwrite the histogram
            for elem in self.data_histo:
                if histo_name==elem[0]:
                    self.data_histo.remove(elem)

        # update boxes
        self.histo_list.setCurrentText(histo_name)
        
        # creation of histo arrays
        if self.button1D.isChecked():                
            x = self.find_parameter(self.par_x)                
            self.data_histo.append([histo_name,x])
        elif self.button2D.isChecked():
            x = self.find_parameter(self.par_x)                            
            y = self.find_parameter(self.par_y)
            self.data_histo.append([histo_name,x,y])            
            #        else:
            #            x = self.find_parameter(self.par_x)                            
            #            y = self.find_parameter(self.par_y)
            #            z = self.find_parameter(self.par_z)                                
            #            self.data_histo.append([histo_name,x,y,z])

        self.plot_histogram()

    def fill_histogram(self, name, r, c, index):

        # extract parameters from low, high, nbin and convert to int
        low_x = int(self.par_x_low.text())
        high_x = int(self.par_x_high.text())
        nbins_x = int(self.par_x_nbins.text())
        low_y = int(self.par_y_low.text())
        high_y = int(self.par_y_high.text())
        nbins_y = int(self.par_y_nbins.text())
        low_z = int(self.par_z_low.text())
        high_z = int(self.par_z_high.text())
        nbins_z = int(self.par_z_nbins.text())                        

        a = self.figure.add_subplot(r, c, index)
        if (name == ''):
            return a
        else: 
            X = self.find_spectrum(name)
        if (len(X)==1):
            a.set_title(name)
            if (self.clear==False):
                (n, bins, patches) = a.hist(X, nbins_x, range=[low_x,high_x])
                self.data_histo_fit.append([name,n])            
        elif (len(X)==2):
            a.set_title(name)
            if (self.clear==False):
                if (self.button2D_option.currentText() == 'Dark'):
                    palette = 'viridis'
                else:
                    palette = 'PuBu'
                a.hist2d(X[0], X[1], bins=(nbins_x,nbins_y), range=[(low_x,high_x),(low_y,high_y)], cmap=plt.get_cmap(palette))
        else:
            print('Not implemented yet')

        return a
        
    def plot_histogram(self):
        self.figure.clear()
        name = self.histo_list.currentText()
            
        a = self.fill_histogram(name, 1, 1, 1)
        
        self.canvas.draw()
        return a

    def gauss(self, x, f, mu, sigma):
        return f*np.exp(-(x-mu)**2.0 / (2*sigma**2))
    
    def exp(self, a, b, c):
        return a+b*np.exp(x*c)
            
    def pol1(self, x, p0, p1):
        return p0+p1*x

    def pol2(self, x, p0, p1, p2):
        return p0+p1*x+p2*x**2

    def pol3(self, x, p0, p1, p2, p3):
        return p0+p1*x+p2*x**2+p3*x**3        
             
    def fit_histo(self):
        # reset fit results
        self.fit_results.setPlainText("")
        # fitting function selection
        fit_func = self.fit_list.currentText()
        # check for histogram existance
        histo_name = self.histo_list.currentText()
        histo = self.plot_histogram()
        popt = []
        pcov = []
        x = []
        y = []
        x_fit = []        
        y_fit = []
        if (histo_name==""):
            return QMessageBox.about(self,"Warning!", "Please create at least one spectrum")
        else:
            # input points for fitting function
            xtmp = [n for n in range(int(self.par_x_low.text()),int(self.par_x_high.text()),
                                     int(int(self.par_x_high.text())/int(self.par_x_nbins.text())))]
            ytmp = self.find_histo_data(histo_name)[0]

            xmin = int(self.fit_range_min.text())
            xmax = int(self.fit_range_max.text())
            # crete new tmp list with subrange for fitting
            for i in range(len(xtmp)):
                if (xtmp[i]>=xmin and xtmp[i]<xmax):
                    x.append(xtmp[i])
                    y.append(ytmp[i])
            x = np.array(x)
            y = np.array(y)
            npoints = 10000
            # fitting switch 
            if (fit_func == 'Gaussian'):
                # initial conditions
                f_begin = 100.
                mu_begin = int(self.par_x_high.text())-int(self.par_x_low.text())
                sigma_begin = mu_begin/10
                p_init = np.array([f_begin, mu_begin, sigma_begin])                
                popt, pcov = optimize.curve_fit(self.gauss, x, y, p0=p_init, maxfev=5000)
                x_fit = np.linspace(x[0],x[-1],npoints)
                y_fit = self.gauss(x_fit, *popt)                

            elif (fit_func == 'Expo'):
                a_begin = 1
                b_begin = 5
                c_begin = -1                                
                p_init = np.array([a_begin, b_begin, c_begin])
                popt, pcov = optimize.curve_fit(self.exp, x, y, p0=p_init, maxfev=5000)
                x_fit = np.linspace(x[0],x[-1],npoints)
                y_fit = self.exp(x_fit, *popt)
                
            elif (fit_func == 'Pol1'):
                p0_begin = 100
                p1_begin = 10
                p_init = np.array([p0_begin,p1_begin])
                popt, pcov = optimize.curve_fit(self.pol1, x, y, p0=p_init, maxfev=5000)
                x_fit = np.linspace(x[0],x[-1],npoints)
                y_fit = self.pol1(x_fit, *popt)
                
            elif (fit_func == 'Pol2'):
                p0_begin = 100
                p1_begin = 10
                p2_begin = 10
                p_init = np.array([p0_begin,p1_begin, p2_begin])                
                popt, pcov = optimize.curve_fit(self.pol2, x, y, p0=p_init, maxfev=5000)
                x_fit = np.linspace(x[0],x[-1],npoints)
                y_fit = self.pol2(x_fit, *popt)
                
            elif (fit_func == 'Pol3'):
                p0_begin = 100
                p1_begin = 10
                p2_begin = 10
                p3_begin = 10
                p_init = np.array([p0_begin,p1_begin, p2_begin, p3_begin])                                
                popt, pcov = optimize.curve_fit(self.pol3, x, y, p0=p_init, maxfev=5000)
                x_fit = np.linspace(x[0],x[-1],npoints)
                y_fit = self.pol2(x_fit, *popt)
                
            else:
                print('2D fitting has not been implemented yet')

            histo.plot(x_fit,y_fit, 'r-')
            for i in range(len(popt)):
                s = 'Par['+str(i)+']: '+str(round(popt[i],3))+'+/-'+str(round(pcov[i][i],3))
                self.fit_results.append(s)

        self.canvas.draw()                
                
    def reset_histogram(self):
        self.plot_histogram()
        
    def add_plot(self): 
        n = len(self.figure.axes)
        for i in range(n):
            self.figure.axes[i].change_geometry(1, n+1, i+1)

        name = self.histo_list.currentText()            
        ax = self.fill_histogram(name, 1, n+1, n+1)
        self.canvas.draw()
        
    def create_gate(self):
        # check for histogram existance
        name = self.histo_list.currentText()
        if (name==""):
            return QMessageBox.about(self,"Warning!", "Please create at least one spectrum")
        else:
            gate_name = self.gates_name.text()
            if gate_name=="" :
                text, okPressed = QInputDialog.getText(self, "Gate name", "Please choose a name for the gate:", QLineEdit.Normal, "")
                if okPressed and text != "":
                    gate_name = text
                else:
                    gate_name = "default_gate"
                    self.gates_name.setText(gate_name)

        # creating entry in combobox if it doesn't exist
        allItems = [self.gates_list.itemText(i) for i in range(self.gates_list.count())]
        result = gate_name in chain(*allItems)
        if result==False:
            self.gates_list.addItem(gate_name)
            # update boxes
            self.gates_list.setCurrentText(gate_name)

        # plot histogram
        ax = self.plot_histogram()        

        X = self.find_spectrum(name)
        if (len(X)==1):        
            self.create_1D_gate(gate_name, ax)            
        else:
            self.create_2D_gate(gate_name, ax)
        
    def create_1D_gate(self, gate_name, ax):

        line, = ax.plot([], [])
        regionbuilder = Gate1DDrawer(line)        
        tmpGate = ['1D', gate_name, regionbuilder]
        self.data_gate.append(tmpGate)
        
    def create_2D_gate(self, gate_name, ax):

        line, = ax.plot([], [])
        linebuilder = Gate2DDrawer(line)
        tmpGate = ['2D', gate_name, linebuilder]
        self.data_gate.append(tmpGate) 

    def delete_gate(self):
        gate_name = self.gates_list.currentText()

        for elem in self.data_gate:
                if (elem[1]==gate_name):
                    self.data_gate.remove(elem)
                    self.gates_list.removeItem(self.gates_list.currentIndex())
                    
        print(self.data_gate)
        
        
    def apply_gate(self):
        # histogram we want to gate
        histo_name = self.histo_list.currentText()
        # gate we want to apply
        gate_name = self.gates_name.text()

        # name of the gated histogram 
        histo_name_gated = histo_name+"_"+gate_name
        self.histo_list.addItem(histo_name_gated)
        self.histo_list.setCurrentText(histo_name_gated)
        self.spectrum_name.setText(histo_name_gated)

        X = self.find_spectrum(histo_name)
        polygon = []

        # unpacking of the gate
        for i in self.data_gate:
            if (i[0]=='1D'):
                if (i[1]==gate_name):
                    Xtmp=i[2].xs
                    # xmin, xmax = Xtmp[0], Xtmp[1]
                    print(len(X))

            else:
                if(i[1]==gate_name):
                    Xtmp=(i[2].X)[0]
                    Ytmp=(i[2].X)[1]
                    for x,y in zip(Xtmp,Ytmp):
                        polygon.append([x,y])
                        
                    path = mpltPath.Path(polygon)
            
                    # data points conversion for checking if inside or outside the gate
                    points = []
                    for x,y in zip(X[0],X[1]):
                        points.append([x,y])
                        
                    inside = path.contains_points(points)
                    # gated dataset
                    X_gated = list(compress(points,inside))
                    Xg, Yg = zip(*X_gated)
                    # appending the gated data
                    self.data_histo.append([histo_name_gated,np.array(Xg),np.array(Yg)])        
                        
                    self.plot_histogram()

    def find_parameter(self, parameter):
        par = parameter.currentText()
        for elem in self.data:
            if par==elem[0]:
                return elem[1]
                
    def find_spectrum(self, name):
        for elem in self.data_histo:
            if name==elem[0]:
                if len(elem)==2:
                    return [elem[1]]
                else:
                    return elem[1], elem[2]

    def find_histo_data(self, name):
        for elem in self.data_histo_fit:
            if name==elem[0]:
                return [elem[1]]
            else:
                print('Not implemented yet')
                
    def create_listParams(self, value):
        self.data.clear()
        xmax = int(self.par_x_high.text())
        for item in range(value):
            self.par_x.addItem('Channel '+str(item))
            self.par_y.addItem('Channel '+str(item))
            self.par_z.addItem('Channel '+str(item))                        
            sample_size = 10
            sample_bkg = 5
            if (self.data_type_g.isChecked()):
                mu = int(np.random.uniform(300,500))
                sigma = int(np.random.uniform(10,100))
                #                a = 2.
                #                s1 = np.random.power(a, sample_bkg)*xmax
                #                s2 = np.random.normal(mu, sigma, sample_size)
                #                #                s = [x1 or x2 for x1,x2 in zip_longest(s1,s2)]
                #                s = np.random.normal(mu, sigma, sample_size)
                s = cpy.CPyConverter().generate("", sample_size)
            elif (self.data_type_exp.isChecked()):
                scale = int(np.random.uniform(1,5))
                s = np.random.exponential(scale, sample_size)
            elif (self.data_type_p1.isChecked()):
                a = 2.
                s = np.random.power(a, sample_size)*xmax
            elif (self.data_type_p2.isChecked()):
                a = 3.
                s = np.random.power(a, sample_size)*xmax
            else:
                a = 4.
                s = np.random.power(a, sample_size)*xmax
            tmp = ['Channel '+str(item), s]
            self.data.extend([tmp])
            
    def check_histogramD(self):
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
            self.par_y.setEnabled(False)
            self.par_y_low.setReadOnly(True)
            self.par_y_high.setReadOnly(True)            
            self.par_y_nbins.setReadOnly(True)
        else:
            self.par_y.setEnabled(True)
            self.par_y_low.setReadOnly(False)
            self.par_y_high.setReadOnly(False)            
            self.par_y_nbins.setReadOnly(False)

    def create_disable3D(self, value):
        if value==True:
            self.par_z.setEnabled(False)
            self.par_z_low.setReadOnly(True)
            self.par_z_high.setReadOnly(True)            
            self.par_z_nbins.setReadOnly(True)
        else:
            self.par_z.setEnabled(True)
            self.par_z_low.setReadOnly(False)
            self.par_z_high.setReadOnly(False)            
            self.par_z_nbins.setReadOnly(False)            
            
            
##############
## app code
##############

#threading.Thread(target=run_dash, args=(), daemon=True).start()
app = QApplication(sys.argv)
gui = GUIClass()
gui.show()
sys.exit(app.exec_())
