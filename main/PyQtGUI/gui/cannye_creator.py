#!/usr/bin/env python
import io
import sys, os
sys.path.append(os.getcwd())

import algo_factory
from CannyEdgePlot import CannyEdgePlot

# Canny Edge algo parameters
# sigma = 1,              # Noise reduction
# kernel_size = 5,          One way to get rid of the noise on the image, is by applying Gaussian blur to smooth it.
#                           To do so, image convolution technique is applied with a Gaussian Kernel (3x3, 5x5, 7x7 etc…).
#                           The kernel size depends on the expected blurring effect. Basically, the smallest the kernel, the less visible is the blur.
#                         # Gradient Calculation
#                           The Gradient calculation step detects the edge intensity and direction by calculating the gradient of the image using edge detection operators.
#                           Edges correspond to a change of pixels’ intensity. To detect it, the easiest way is to apply filters that highlight this intensity
#                           change in both directions: horizontal (x) and vertical (y). I defined Sobel filters to be applied to the image to get both intensity and edge direction matrices
#                         # Non-Maximum Suppression
#                           Ideally, the final image should have thin edges. Thus, we must perform non-maximum suppression to thin out the edges. 
#                           The principle is simple: the algorithm goes through all the points on the gradient intensity matrix and finds the pixels
#                           with the maximum value in the edge directions. Each pixel has 2 main criteria (edge direction in radians, and pixel intensity (between 0–255)).
#                           Based on these inputs the non-max-suppression steps are:
#                            - Create a matrix initialized to 0 of the same size of the original gradient intensity matrix;
#                            - Identify the edge direction based on the angle value from the angle matrix;
#                            - Check if the pixel in the same direction has a higher intensity than the pixel that is currently processed;
#                            - Return the image processed with the non-max suppression algorithm.
# lowthreshold = 0.05,    # Double threshold
# highthreshold = 0.15,     The double threshold step aims at identifying 3 kinds of pixels: strong, weak, and non-relevant:
#                            - Strong pixels are pixels that have an intensity so high that we are sure they contribute to the final edge.
#                            - Weak pixels are pixels that have an intensity value that is not enough to be considered as strong ones,
#                              but yet not small enough to be considered as non-relevant for the edge detection.
#                            - Other pixels are considered as non-relevant for the edge.
#                           Now you can see what the double thresholds holds for:
#                            - High threshold is used to identify the strong pixels (intensity higher than the high threshold)
#                            - Low threshold is used to identify the non-relevant pixels (intensity lower than the low threshold)
#                            - All pixels having intensity between both thresholds are flagged as weak and the Hysteresis mechanism (next step)
#                              will help us identify the ones that could be considered as strong and the ones that are considered as non-relevant.
# weak_pixel = 75,        # Edge Tracking by Hysteresis
# strong_pixel = 255        Based on the threshold results, the hysteresis consists of transforming weak pixels into strong ones,
#                           if and only if at least one of the pixels around the one being processed is a strong one

class CannyEdgeAlgo:
    def __init__(self, sigma, kernel_size, lowthreshold, highthreshold, weak_pixel, strong_pixel):
        self.sigma = sigma
        self.kernel_size = kernel_size
        self.lowthreshold = lowthreshold
        self.highthreshold = highthreshold
        self.weak_pixel = weak_pixel
        self.strong_pixel = strong_pixel

        self.cannyEdgePopup = CannyEdgePlot()
        
    # implementation of the algorithm, the argument are mandatory even if not used
    def start(self, data, weigths, nclusters, axis, figure=None):
        xmin, xmax = axis.get_xlim()
        ymin, ymax = axis.get_ylim()

        #create picture for clustering analysis
        filename = 'cannyE.jpg'
        extent = axis.get_window_extent().transformed(figure.dpi_scale_trans.inverted())
        figure.savefig(filename, bbox_inches=extent.expanded(0.8, 0.9))

        self.cannyEdgePopup.show()
        self.cannyEdgePopup.setConfig(self.sigma, self.kernel_size, self.lowthreshold, self.highthreshold, self.weak_pixel, self.strong_pixel)
        self.cannyEdgePopup.plotEdge(filename, xmin, xmax, ymin, ymax)
        
class CannyEdgeAlgoBuilder:
    def __init__(self):
        self._instance = None

    def __call__(self, sigma=1, kernel_size=7, lowthreshold=0.05, highthreshold=0.15, weak_pixel=75, strong_pixel=255, **_ignored):
        if not self._instance:
            self._instance = CannyEdgeAlgo(sigma, kernel_size, lowthreshold, highthreshold, weak_pixel, strong_pixel)
        return self._instance
