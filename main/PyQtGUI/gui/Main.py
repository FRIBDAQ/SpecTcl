#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())
sys.path.append("../Script")

from PyQt5 import QtCore
from PyQt5.QtWidgets import *

import cv2

# GUI graphical skeleton
from GUI import MainWindow

import algo_factory
# skeleton for user-based implementation
import skel_creator
import kmean_creator
import gmm_creator
import imgseg_creator
import cannye_creator

factory = algo_factory.AlgoFactory()

# Configurable parameters for Skel (for the full param list, please see skel_creator.py)
config_skel = {
    'param_1': 5,
    'param_2': 'test',
    'param_3': 100
}

# Configurable parameters for KMean (for the full param list, please see kmean_creator.py)
config_kmean = {
    'n_clusters': 3,
    'n_init': 10
}

# Configurable parameters for Gaussian Mixture Model (for the full param list, please see gmm_creator.py)
config_gmm = {
    'n_components': 4,
}

# Configurable parameters for Image Segmentation (for the full param list, please see imgseg_creator.py)
config_img = {
    'nclusters': 5,
    'criteria' : (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.2),
    'attempts' :10,
    'flags': cv2.KMEANS_RANDOM_CENTERS
}

# Configurable parameters for Canny Edge (for the full param list, please see cannye_creator.py)
config_canny = {
    'sigma': 1,
    'kernel_size': 7,
    'lowthreshold': 0.05,
    'highthreshold': 0.15,
    'weak_pixel': 75,
    'strong_pixel': 255
}

# Algorithm registration
factory.register_builder('Skeleton', skel_creator.SkelAlgoBuilder(), config_skel)
factory.register_builder('KMean', kmean_creator.KMeanAlgoBuilder(), config_kmean)
factory.register_builder('Gaussian MM', gmm_creator.GMMAlgoBuilder(), config_gmm)
factory.register_builder('Image Segmentation', imgseg_creator.ImgSegAlgoBuilder(), config_img)
factory.register_builder('Canny Edge', cannye_creator.CannyEdgeAlgoBuilder(), config_canny)

app = QApplication(sys.argv)
gui = MainWindow(factory)
gui.show()
sys.exit(app.exec_())
