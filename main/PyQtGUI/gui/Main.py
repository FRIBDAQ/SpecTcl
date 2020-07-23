#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())
sys.path.append("../Script")

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

# GUI graphical skeleton
from GUI import MainWindow

import algo_factory
# skeleton for user-based implementation
import skel_creator
import kmean_creator

factory = algo_factory.AlgoFactory()

# Configurable parameters for Skel
# param_1 = 5
# param_2 = 'test',
# param_3 = 100
config_skel = {
    'param_1': 5,
    'param_2': 'test',
    'param_3': 100
}

# Configurable parameters for KMean
# n_clusters=8,
# init='k-means++',
# n_init=10,
# max_iter=300,
# tol=0.0001,
# algorithm='auto'
config_kmean = {
    'n_clusters': 3,
    'n_init': 10
}

# Algorithm registration
factory.register_builder('Skeleton', skel_creator.SkelAlgoBuilder(), config_skel)
factory.register_builder('KMean', kmean_creator.KMeanAlgoBuilder(), config_kmean)

app = QApplication(sys.argv)
gui = MainWindow(factory)
gui.show()
sys.exit(app.exec_())
