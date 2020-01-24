#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

# GUI graphical skeleton
from GUISkel import MainWindow

app = QApplication(sys.argv)
gui = MainWindow()
gui.show()
sys.exit(app.exec_())
