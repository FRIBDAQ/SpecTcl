#!/usr/bin/env python
import sys
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

class Example(QMainWindow):

    def __init__(self):
        super(Example,self).__init__()
        
        self.initUI()


    def initUI(self):
        
        self.statusbar = self.statusBar()
        self.statusbar.showMessage('Ready')
        
        menubar = self.menuBar()
        viewMenu = menubar.addMenu('View')
        viewMenu.menuAction().setStatusTip("File Menu is hovered")
        
        self.myMessage = QLabel()
        self.myMessage.setText("Hello")
        
        self.statusbar.addPermanentWidget(self.myMessage)
        
        viewStatAct = QAction('View statusbar', self, checkable=True)
        #viewStatAct.setStatusTip('View statusbar')
        viewStatAct.setChecked(True)
        viewStatAct.triggered.connect(self.toggleMenu)
        
        viewMenu.addAction(viewStatAct)
        
        self.setGeometry(300, 300, 300, 200)
        self.setWindowTitle('Check menu')
        self.show()

    def event(self, e):
        if e.type() == QtCore.QEvent.StatusTip:
            if e.tip() == '':
                e = QtGui.QStatusTipEvent('Ready')  # Set this to whatever you like
        return super().event(e)
        
    def toggleMenu(self, state):
            
        if state:
            self.statusBar().show()
        else:
            self.statusBar().hide()
            
            
if __name__ == '__main__':
    app=0
    app = QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())


'''
import sys
from PyQt5.Qt import *

class MyPopup(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        
    def paintEvent(self, e):
        dc = QPainter(self)
        dc.drawLine(0, 0, 100, 100)
        dc.drawLine(100, 0, 0, 100)

class MainWindow(QMainWindow):
    def __init__(self, *args):
        QMainWindow.__init__(self, *args)
        self.cw = QWidget(self)
        self.setCentralWidget(self.cw)
        self.btn1 = QPushButton("Click me", self.cw)
        self.btn1.setGeometry(QRect(0, 0, 100, 30))
        self.connect(self.btn1, SIGNAL("clicked()"), self.doit)
        self.w = None
        
    def doit(self):
        print("Opening a new popup window...")
        self.w = MyPopup()
        self.w.setGeometry(QRect(100, 100, 400, 200))
        self.w.show()
        
class App(QApplication):
    def __init__(self, *args):
        QApplication.__init__(self, *args)
    self.main = MainWindow()
    self.connect(self, SIGNAL("lastWindowClosed()"), self.byebye )
    self.main.show()

    def byebye( self ):
        self.exit(0)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())
'''                     
'''
import sys

from PyQt5.QtCore import pyqtSlot
from PyQt5.QtWidgets import QWidget, QListWidget, QListWidgetItem, QLabel, QApplication, QDialog

class ExampleWidget(QWidget):

    def __init__(self):
        super().__init__()
        listWidget = QListWidget(self)
        listWidget.itemDoubleClicked.connect(self.buildExamplePopup)
        for n in ["Jack", "Chris", "Joey", "Kim", "Duncan"]:
            QListWidgetItem(n, listWidget)
        self.setGeometry(100, 100, 100, 100)
        self.show()
        
    @pyqtSlot(QListWidgetItem)
    def buildExamplePopup(self, item):
        exPopup = ExamplePopup(item.text(), self)
        exPopup.setGeometry(100, 200, 100, 100)
        exPopup.show()

class ExamplePopup(QDialog):

        def __init__(self, name, parent=None):
            super().__init__(parent)
            self.name = name
            self.label = QLabel(self.name, self)
        
if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = ExampleWidget()
    sys.exit(app.exec_())
''' 
'''
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

fig2 = plt.figure(constrained_layout=True)
spec2 = gridspec.GridSpec(ncols=2, nrows=2, figure=fig2)
f2_ax1 = fig2.add_subplot(spec2[0, 0])
f2_ax2 = fig2.add_subplot(spec2[0, 1])
f2_ax3 = fig2.add_subplot(spec2[1, 0])
f2_ax4 = fig2.add_subplot(spec2[1, 1])

plt.show()
'''

'''
from matplotlib import pyplot as plt
import numpy as np

x = np.linspace(0,np.pi,100)
y1 = np.sin(x)
y2 = np.cos(x)
fig, (ax1,ax2) = plt.subplots(nrows=1, ncols=2)
fig.tight_layout()
ax1.plot(x,y1)
ax2.plot(x,y2)

def on_resize(event):
    fig.tight_layout()
    fig.canvas.draw()

cid = fig.canvas.mpl_connect('resize_event', on_resize)

plt.show()
'''

'''
import matplotlib.pyplot as plt
import numpy as np

# make some data
a = np.random.randn(10,10)

# mask some 'bad' data, in your case you would have: data == 0
a = np.ma.masked_where(a < 0.05, a)

cmap = plt.cm.OrRd
cmap.set_bad(color='black')

plt.imshow(a, interpolation='none', cmap=cmap)

plt.show()
'''
'''
import matplotlib.pyplot as plt
import numpy as np


def create_plot(num, ax=None):
    ax = ax or plt.gca()
    t = np.linspace(0,2*np.pi,101)
    ax.plot(t, np.sin(num*t), label="Label {}".format(num))
    ax.legend()

fig, axes = plt.subplots(5,4)

for i,ax in enumerate(axes.flat):
    create_plot(i, ax=ax)

    
def create_fig(evt):
    if evt.inaxes:
        newfig, newax = plt.subplots()
        num = fig.axes.index(evt.inaxes)
        create_plot(num, ax=newax)
        newfig.show()
        
fig.canvas.mpl_connect("button_press_event", create_fig)

plt.show()  
'''
