#!/usr/bin/env python
import io
import pickle
import sys, os
sys.path.append(os.getcwd())

import json
import httplib2

import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.gridspec as gridspec
import pickle
from mpl_toolkits.axes_grid1 import make_axes_locatable

import pandas as pd
import numpy as np
import CPyConverter as cpy

# import widgets
from menu import Menu
from plot import Plot
from configuration import Configuration

class MainWindow(QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        self.setWindowTitle("Project Phantom Snake")

        # global variables
        self.timer = QtCore.QTimer()
        self.setWindowFlags(QtCore.Qt.WindowMaximizeButtonHint | QtCore.Qt.WindowMinimizeButtonHint)
        self.originalPalette = QApplication.palette()

        # dictionaries for parameters
        self.param_list = {}
        # dataframe for spectra
        self.spectrum_list = pd.DataFrame()

        # dictionary for histogram
        self.h_dict = {}
        self.h_dict_output = {}  # for saving pane geometry

        # index of the histogram
        self.index = 0
        self.idx = 0 
        
        # click selection of position in canvas
        self.inx = 0
        self.isSelected = False
        self.selected_row = 0
        self.selected_col = 0        
        
        # support lists
        self.h_dim = []
        self.h_lst = []
        self.h_lst_cb = {} # this is a dictionary because we have non-consecutive entries
        self.h_zoom_max = []

        # for zooming 
        self.yhigh = 1024
        self.vmin = 0
        self.vmax = 1024        
        
        # configuration widget flag
        self.isHidden = False

        # flags
        self.isFull = False
        self.Start = False        
        self.forAll = False
        self.isLoaded = False
        self.isZoomed = False
        
        # tools for selected plots
        self.rec = 0
        
        #######################
        # Main layout GUI
        #######################        
        
        mainLayout = QVBoxLayout()
        mainLayout.setContentsMargins(0,0,0,0)
        mainLayout.setSpacing(1)
        
        # top menu
        self.wTop = Menu()
        self.wTop.setFixedHeight(100)
        
        # config menu
        self.wConf = Configuration()
        self.wConf.setFixedHeight(225)
        
        # plot widget
        self.wPlot = Plot()
        
        # gui composition
        mainLayout.addWidget(self.wTop)
        mainLayout.addWidget(self.wConf)
        mainLayout.addWidget(self.wPlot)        
         
        widget = QWidget()
        widget.setLayout(mainLayout)
        
        self.setCentralWidget(widget)

        #################
        # Signals
        #################

        # top menu signals
        self.wTop.exitButton.clicked.connect(self.close)
        self.wTop.updateButton.clicked.connect(self.update)                
        #self.wTop.slider.valueChanged.connect(self.self_update)

        self.wTop.configButton.clicked.connect(self.configure)                        
        self.wTop.saveButton.clicked.connect(self.saveGeo)
        self.wTop.loadButton.clicked.connect(self.loadGeo)          
        
        # configuration signals
        self.at_startup()

        self.wConf.histo_geo_row.activated.connect(lambda: self.initialize_canvas(int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_col.activated.connect(lambda: self.initialize_canvas(int(self.wConf.histo_geo_row.currentText()), int(self.wConf.histo_geo_col.currentText())))
        self.wConf.histo_geo_add.clicked.connect(self.add_plot)

        self.wConf.histo_geo_update.clicked.connect(self.update_plot)        
        self.wConf.histo_geo_delete.clicked.connect(self.delete_plot)        
        self.wConf.button2D_option.activated.connect(self.change_bkg)        
        self.wConf.histo_geo_all.stateChanged.connect(self.applyAll)
        
        # plotting signals
        self.wPlot.plusButton.clicked.connect(lambda: self.zoomIn(self.wPlot.canvas))
        self.wPlot.minusButton.clicked.connect(lambda: self.zoomOut(self.wPlot.canvas))        

        self.wPlot.canvas.mpl_connect("resize_event", self.on_resize)
        self.wPlot.canvas.mpl_connect("button_press_event", self.interact_fig)

    def on_resize(self, event):
        self.wPlot.figure.tight_layout()
        self.wPlot.canvas.draw()

    def interact_fig(self, event):
        if not event.inaxes: return
        self.inx = list(self.wPlot.figure.axes).index(event.inaxes)
        self.selected_row, self.selected_col = self.plot_position(self.inx)
        if event.dblclick:
            # select plot
            if self.isZoomed == False:
                # clear existing figure
                self.wPlot.figure.clear()
                self.wPlot.canvas.draw()
                # create selected figure
                try:
                    a = self.wPlot.figure.add_subplot(111)
                    self.plot_histogram(a, self.inx)
                    self.wPlot.canvas.draw()
                    # we are in zoomed mode
                    self.isZoomed = True
                except:
                    QMessageBox.about(self, "Warning", "There are no histograms defined...")
                    self.initialize_canvas(self.wConf.row, self.wConf.col)
            else:
                #draw the back the original canvas
                self.wPlot.figure.clear()
                for index, value in self.h_dict_output.items():
                    a = self.select_plot(index)
                    x,y = self.plot_position(index)
                    a = self.wPlot.figure.add_subplot(self.grid[x,y])
                    self.plot_histogram(a, index)
                    
                self.wPlot.figure.tight_layout()
                self.wPlot.canvas.draw()
                    
                self.isZoomed = False

        # single click selects the subplot
        else:
            for i, plot in enumerate(self.wPlot.figure.axes):
                # retrieve the subplot from the click
                if (i == self.inx):
                    if self.isSelected == True:
                        self.isSelected= False
                        self.rec.remove()
                    else:
                        self.rec = self.create_rectangle(plot)
                        self.isSelected= True
            self.wPlot.canvas.draw()

    def create_rectangle(self, plot):
        autoAxis = plot.axis()
        percentTB = 0.04;
        percentLR = 0.06;
        tb = (autoAxis[3]-autoAxis[2])*percentTB;        
        lr = (autoAxis[1]-autoAxis[0])*percentLR;
        rec = matplotlib.patches.Rectangle((autoAxis[0]-lr,autoAxis[2]-tb),
                                           (autoAxis[1]-autoAxis[0])+2*lr,
                                           (autoAxis[3]-autoAxis[2])+2*tb, fill=False, color='red', alpha=0.3, lw=2)
        rec = plot.add_patch(rec)
        rec.set_clip_on(False)        
        return rec
    
    # At startup we need:
    # - the full parameter list as dictionary - (id,name) 
    # - a canvas based on the geometry 
    
    def at_startup(self):
        self.create_parameter_list()
        self.initialize_canvas(self.wConf.row, self.wConf.col)

    def create_parameter_list(self):
        self.update_parameter_list()
        for key, value in self.param_list.items():
            for i in range(2):            
                self.wConf.listParams[i].addItem(value)
                    
    def initialize_canvas(self, row, col):
        # everytime a new pane geometry is created the histogram
        # dictionary and the index of the plot has to be reset
        self.h_dict.clear()
        self.h_dict_output.clear()
        self.index = 0
        self.idx = 0
        self.wPlot.figure.clear()
        self.initialize_figure(self.create_figure(row, col))
        self.wPlot.canvas.draw()
        
    def initialize_figure(self, grid):
        self.h_dim.clear()
        self.h_lst.clear()        
        for i in range(self.wConf.row):
            for j in range(self.wConf.col):
                a = self.wPlot.figure.add_subplot(grid[i,j])
        # initialize list of figure
        for z in range(self.wConf.row*self.wConf.col):
            self.h_dict[z] = self.initialize_histogram()
            self.h_zoom_max.append(0)
        self.h_dim = self.get_histo_key_list(self.h_dict, "dim")
        self.h_lst = self.get_histo_key_list(self.h_dict, "name")
        
    def initialize_histogram(self):
        return {"name": 0, "dim": 0, "xmin": 0, "xmax": 0, "xbin": 0,
                "ymin": 0, "ymax": 0, "ybin": 0}
        
    def create_figure(self, row, col):
        self.wConf.row = row
        self.wConf.col = col
        self.grid = gridspec.GridSpec(ncols=self.wConf.col, nrows=self.wConf.row, figure=self.wPlot.figure)
        return self.grid

    # hide/show configuration of the canvas
    def configure(self):
        if self.isHidden == False:
            self.wConf.hide()
            self.isHidden = True
        else:
            self.wConf.show()
            self.isHidden = False

    def update_parameter_list(self):
        try:
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
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started...")

    # when clicking update we need to access the shared memory and
    # update the spectrum information in the data frame
    def update(self):
        # this snippet tests if the rest server is up and running
        try:
            self.update_parameter_list()
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
            # update the list of defined spectra
            self.create_spectrum_list()        
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started...")
            
    def select_plot(self, index):
        for i, plot in enumerate(self.wPlot.figure.axes):
            # retrieve the subplot from the click
            if (i == index):
                return plot
        
    def plot_position(self, index):
        cntr = 0
        # convert index to position in geometry
        for i in range(self.wConf.row):
            for j in range(self.wConf.col):            
                if index == cntr:
                    return i, j
                else:
                    cntr += 1

    # save geometry window
    def saveGeo(self):
        fileName = self.saveFileDialog()
        try:
            f = open(fileName,"w")
            tmp = {"row": self.wConf.row, "col": self.wConf.col, "geo": self.h_dict_output}
            QMessageBox.about(self, "Saving...", "Window configuration saved!")
            f.write(str(tmp))
            f.close()
        except TypeError:
            pass

    # load geometry window
    def loadGeo(self):
        fileName = self.openFileNameDialog()
        try:
            f = open(fileName,"r").read()
            infoGeo = eval(f)
            self.wConf.row = infoGeo["row"]
            self.wConf.col = infoGeo["col"]
            # change index in combobox to the actual loaded values
            index_row = self.wConf.histo_geo_row.findText(str(self.wConf.row), QtCore.Qt.MatchFixedString)
            index_col = self.wConf.histo_geo_col.findText(str(self.wConf.col), QtCore.Qt.MatchFixedString)
            if index_row >= 0 and index_col >= 0:
                self.wConf.histo_geo_row.setCurrentIndex(index_row)
                self.wConf.histo_geo_col.setCurrentIndex(index_col)
                self.initialize_canvas(infoGeo["row"],infoGeo["col"])
                self.h_dict_output = infoGeo["geo"]
                if len(self.h_dict_output) == 0:
                    QMessageBox.about(self, "Warning", "You saved an empty pane geometry...")
            self.isLoaded = True
            self.update_plot()

        except TypeError:
            pass

    def openFileNameDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getOpenFileName(self,"QFileDialog.getOpenFileName()", "","Window Files (*.win);;All Files (*)", options=options)
        if fileName:
            return fileName
        
    def saveFileDialog(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        fileName, _ = QFileDialog.getSaveFileName(self,"QFileDialog.getSaveFileName()","","Window Files (*.win);;All Files (*)", options=options)
        if fileName:
            return fileName+".win"
            
    # add list of parameters for each spectrum to the dataframe        
    def create_spectrum_parameters(self):
        try:
            tmpl = httplib2.Http().request("http://localhost:8080/spectcl/spectrum/list")[1]
            tmp = json.loads(tmpl.decode())
            tmppar = []
            for dic in tmp['detail']:
                for key in dic:
                    if key == 'parameters':
                        tmppar.append(dic[key]) 
                        # adds list to dataframe           
            self.spectrum_list['parameters'] = tmppar
        except:
            QMessageBox.about(self, "Warning", "The rest interface for SpecTcl was not started...")
            
    # update the list of defined spectra
    def create_spectrum_list(self):
        for name in self.spectrum_list['names']:
            if self.wConf.histo_list.findText(name) == -1:
                self.wConf.histo_list.addItem(name)

    # looking for type 0 (not set) histograms 
    def check_index(self):
        # support list is full
        if any(x == 0 for x in self.h_dim) == self.isFull:
            self.isFull = True
            self.index = 0
            return self.index
        # support list not full, then look for the next index with value 0
        else:
            idx = next((i for i, x in enumerate(self.h_dim) if x==0), None)
            if idx is not None:
                self.index = idx
                return self.index
            else:
                self.Start = True
                
        if self.Start == True:
            if self.index == self.wConf.row*self.wConf.col-1:
                self.index = 0
            else:
                self.index += 1
                    
        return self.index
                                                                            
    def mapDictionaryToIndex(self, my_dict):
        # list for mapping dictionary for window geometry
        index_list = []
        for key, value in my_dict.items():
            index = self.wConf.histo_list.findText(value, QtCore.Qt.MatchFixedString)
            if index >= 0:
                index_list.append(index)

        return index_list

    def add_plot(self):
        # self adding 
        if self.isSelected == False:
            self.idx = self.check_index()
        # position selected by user
        else:
            self.idx = self.inx

        # updating histogram info            
        self.h_dict[self.idx] = self.update_spectrum_info()        

        self.add(self.idx)
        self.wPlot.canvas.draw()

        # updating support list
        self.h_dim = self.get_histo_key_list(self.h_dict, "dim")
        
        # updating output dictionary
        self.h_dict_output[self.idx] = self.get_histo_name(self.idx)
        
    def add(self, index):
        a = self.select_plot(index)
        x,y = self.plot_position(index)        
        self.erase_plot(index)
        if self.isSelected == True:
            a = self.wPlot.figure.add_subplot(self.grid[self.selected_row,self.selected_col])
        else:
            a = self.wPlot.figure.add_subplot(self.grid[x,y])
        self.isSelected = False
        self.plot_histogram(a, index)

    def erase_plot(self, index):
        a = self.select_plot(index)
        # if 2d histo I need a bit more efforts for the colorbar
        if self.h_dim[index] == 2:
            im = a.images
            if im is not None:
                try:
                    cb = im[-1].colorbar
                    cb.remove()
                except IndexError:
                    pass
            
        a.clear()

    def delete_plot(self):
        if self.forAll == True:
            for index in range(self.wConf.col*self.wConf.row):
                self.erase_plot(index)
                # reinitialize the histograms
                self.h_dict_output[index] = self.initialize_histogram()
                # reset support list
                self.isFull = False
        else:
            if self.isSelected == False:
                idx = next((i for i, x in enumerate(self.h_dim) if x), None)
                if idx is not None:
                    self.idx = idx
            else:
                self.idx = self.inx
                
            self.erase_plot(self.idx)
            self.isSelected = False
            # reinitialize the histogram at index
            self.h_dict_output[self.idx] = self.initialize_histogram()
            # zero the element in the support list
            self.h_dim[self.idx] = 0            
            self.isFull = False

        if sum(self.h_dim) == 0:
            self.h_dict.clear()            
            self.h_dict_output.clear()
            self.Start = False
            
        self.wPlot.canvas.draw()

    def update_plot(self):
        self.update()
        index_list = self.mapDictionaryToIndex(self.h_dict_output)
        for index, value in self.h_dict_output.items():
            self.wConf.histo_list.setCurrentIndex(index_list[index])
            # updating histogram info
            self.h_dict[index] = self.update_spectrum_info()
            self.h_dim[index] = self.get_histo_dim(index)
            
            self.add(index)
            
        self.wPlot.figure.tight_layout()
        self.wPlot.canvas.draw()        

    def change_bkg(self):
        if any(x == 2 for x in self.h_dim) == True:
            indices = [i for i, x in enumerate(self.h_dim) if x == 2]
            for index in indices:
                self.isSelected = False # this line is important for automatic conversion from dark to light and viceversa
                self.add(index)
        self.wPlot.canvas.draw()

    def applyAll(self, state):
        if state == QtCore.Qt.Checked:
            self.forAll = True
        else:
            self.forAll = False
    '''
    def self_update(self):
        self.wTop.slider_label.setText("Refresh interval ({} s)".format(self.wTop.slider.value()))
        if self.wTop.slider.value() != 0:
            self.timer.setInterval(1000*int(self.wTop.slider.value()))
            self.timer.timeout.connect(self.update)
            self.timer.timeout.connect(self.update_plot)            
            self.timer.start()
        else:
            self.timer.stop()
    '''

    def plot_histogram(self, axis, index):
        name = self.get_histo_name(index)
        if (name == ''):
            return histo
        else:
            select = self.spectrum_list['names'] == name
            df = self.spectrum_list.loc[select]
            w = df.iloc[0]['data']

        if self.get_histo_dim(index) == 1:
            empty = sum(w)
        else:
            empty = sum(w[1])

        if (empty == 0):
            self.isEmpty = True
            axis.set_ylim(0,512)
            QMessageBox.about(self, "Warning", "The shared memory is still empty...")
        else:
            self.isEmpty = False
            dim = df.iloc[0]['dim']        

            hdim = self.get_histo_dim(index)            
            minx = self.get_histo_xmin(index)
            maxx = self.get_histo_xmax(index)
            binx = self.get_histo_xbin(index)            

            if hdim == 1:
                axis.set_ylim(0,self.yhigh)
                X = self.create_range(binx, minx, maxx)
                # create histogram
                self.h_lst[index] = axis.hist(X,
                                              len(X),
                                              weights=w,
                                              range=[minx,maxx],
                                              histtype='step')
                plt.xlim(left=minx, right=maxx)
                x_label = str(df.iloc[0]['parameters'])
                plt.xlabel(x_label,fontsize=10)
                self.h_zoom_max[index] = self.yhigh
            else:
                miny = self.get_histo_ymin(index)
                maxy = self.get_histo_ymax(index)
                binsy = self.get_histo_ybin(index)

                divider = make_axes_locatable(axis)
                cax = divider.append_axes("right", 0.25, pad=0.05)

                if (self.wConf.button2D_option.currentText() == 'Dark'):
                    palette = 'plasma'
                else:
                    palette = copy(plt.cm.plasma)
                    w = np.ma.masked_where(w < 0.1, w)
                    palette.set_bad(color='white')

                # search by name if the histogram has been plotted before
                #if self.checkValue(index, name) == False:
                # print("creating 2d", name, "in index", index)
                self.h_lst[index] = axis.imshow(w,
                                                interpolation='none',
                                                extent=[float(minx),float(maxx),float(miny),float(maxy)],
                                                aspect='auto',
                                                origin='lower',
                                                vmin=self.vmin, vmax=self.vmax,
                                                cmap=palette)
                #else:
                #    print("filling 2d", name, "in index", index)                    
                #    self.h_lst[index].set_data(w)
                self.h_lst_cb[index] = plt.colorbar(self.h_lst[index], cax=cax, orientation='vertical')
                self.h_zoom_max[index] = self.vmax
                
    # if we have already created a 2d image, we just update the dataset
    def checkValue(self, key, value):
        if key in self.h_dict_output.keys() and value == self.h_dict_output[key]: 
            return True
        else: 
            return False

    def zoomIn(self, canvas):
        for i, ax in enumerate(self.wPlot.figure.axes):
            if (i == self.inx):
                ymax = self.get_histo_zoomMax(i)
                ymax /= 2                
                if self.h_dim[i] == 1:
                    ax.set_ylim(0,ymax)
                    self.set_histo_zoomMax(i, ymax)
                if self.h_dim[i] == 2:
                    self.h_lst[i].set_clim(vmax=ymax)
                    self.set_histo_zoomMax(i, ymax)
                # redraw rectangle for new axis
                if (self.isSelected == True):
                    self.rec.remove()
                    self.rec = self.create_rectangle(ax)
        canvas.draw()
            
    def zoomOut(self, canvas):
        for i, ax in enumerate(self.wPlot.figure.axes):
            if (i == self.inx):
                ymax = self.get_histo_zoomMax(i)
                ymax *= 2                
                if self.h_dim[i] == 1:
                    ax.set_ylim(0,ymax)
                    self.set_histo_zoomMax(i, ymax)
                if self.h_dim[i] == 2:
                    self.h_lst[i].set_clim(vmax=ymax)
                    self.set_histo_zoomMax(i, ymax)
                # redraw rectangle for new axis
                if (self.isSelected == True):
                    self.rec.remove()
                    self.rec = self.create_rectangle(ax)
        canvas.draw()

    def create_range(self, bins, vmin, vmax):
        x = []
        step = (float(vmax)-float(vmin))/float(bins)
        for i in np.arange(float(vmin), float(vmax), step):
            x.append(i)
        return x

    def check_histogram(self):
        if self.wConf.button1D.isChecked():
            self.create_disable2D(True)
        else:
            self.create_disable2D(False)
            
    def create_disable2D(self, value):
        if value==True:
            self.wConf.listParams[1].setEnabled(False)
            self.wConf.listParams_low[1].setText("")
            self.wConf.listParams_low[1].setReadOnly(True)
            self.wConf.listParams_high[1].setText("")
            self.wConf.listParams_high[1].setReadOnly(True)
            self.wConf.listParams_bins[1].setText("")
            self.wConf.listParams_bins[1].setReadOnly(True)            
        else:
            self.wConf.listParams[1].setEnabled(True)
            self.wConf.listParams_low[1].setReadOnly(False)
            self.wConf.listParams_high[1].setReadOnly(False)            
            self.wConf.listParams_bins[1].setReadOnly(False)

    ###########################################################
    # update information for current selected spectrum
    ###########################################################
    def update_spectrum_info(self):
        # access name and id for the current spectrum
        hist_tmp = {}
        self.histo_name = str(self.wConf.histo_list.currentText())
        self.histo_dim = 0

        if self.histo_name == "":
            QMessageBox.about(self, "Warning", "Please click 'Get Data' to access the shared memory...")
        else:
            # update info on gui for spectrum name
            self.wConf.spectrum_name.setText(self.histo_name)            
            # extra data from data frame to fill up the gui 
            select = self.spectrum_list['names'] == self.histo_name
            df = self.spectrum_list.loc[select]
            self.histo_dim = df.iloc[0]['dim']
            if self.histo_dim == 1:
                self.wConf.button1D.setChecked(True)
            else:
                self.wConf.button2D.setChecked(True)
            self.check_histogram();
            for i in range(self.histo_dim):
                index = self.wConf.listParams[i].findText(df.iloc[0]['parameters'][i], QtCore.Qt.MatchFixedString)
                if index >= 0:
                    self.wConf.listParams[i].setCurrentIndex(index)
                if i == 0:
                    self.wConf.listParams_low[i].setText(str(df.iloc[0]['minx']))
                    self.wConf.listParams_high[i].setText(str(df.iloc[0]['maxx']))
                    self.wConf.listParams_bins[i].setText(str(df.iloc[0]['binx']))
                    self.histo_minx = self.wConf.listParams_low[i].text().replace(' ', '')
                    self.histo_maxx = self.wConf.listParams_high[i].text().replace(' ', '')
                    self.histo_binx = self.wConf.listParams_bins[i].text().replace(' ', '')
                    self.histo_miny = 0
                    self.histo_maxy = 0
                    self.histo_biny = 0                                                           
                else :
                    self.wConf.listParams_low[i].setText(str(df.iloc[0]['miny']))
                    self.wConf.listParams_high[i].setText(str(df.iloc[0]['maxy']))
                    self.wConf.listParams_bins[i].setText(str(df.iloc[0]['biny']))
                    self.histo_miny = self.wConf.listParams_low[i].text().replace(' ', '')
                    self.histo_maxy = self.wConf.listParams_high[i].text().replace(' ', '')
                    self.histo_biny = self.wConf.listParams_bins[i].text().replace(' ', '')

            hist_tmp = {"name": self.histo_name, "dim": self.histo_dim, "xmin": self.histo_minx, "xmax": self.histo_maxx, "xbin": self.histo_binx,
                        "ymin": self.histo_miny, "ymax": self.histo_maxy, "ybin": self.histo_biny}
            return hist_tmp

    # get value for a dictionary at index x with key y
    def get_histo_key_value(self, d, index, key):
        if key in d[index]:
            return d[index][key]
        
    # get a list of elements identified by the key y for a dictionary
    def get_histo_key_list(self, d, keys):
        lst = []
        for key, value in d.items():
            lst.append(self.get_histo_key_value(d, key, keys))
        return lst
                
    # get histo name
    def get_histo_name(self, index):
        return self.h_dict[index]["name"]

    # get histo dim
    def get_histo_dim(self, index):
        return int(float(self.h_dict[index]["dim"]))

    # get histo xmin
    def get_histo_xmin(self, index):
        return int(float(self.h_dict[index]["xmin"]))

    # get histo xmax
    def get_histo_xmax(self, index):
        return int(float(self.h_dict[index]["xmax"]))

    # get histo xbin
    def get_histo_xbin(self, index):
        return int(float(self.h_dict[index]["xbin"]))

    # get histo ymin
    def get_histo_ymin(self, index):
        return int(float(self.h_dict[index]["ymin"]))

    # get histo ymax
    def get_histo_ymax(self, index):
        return int(float(self.h_dict[index]["ymax"]))

    # get histo ybin
    def get_histo_ybin(self, index):
        return int(float(self.h_dict[index]["ybin"]))
    
    # get histo zoom max
    def get_histo_zoomMax(self, index):
        return int(float(self.h_zoom_max[index]))

    # set histo zoom max
    def set_histo_zoomMax(self, index, value):
        self.h_zoom_max[index] = value


