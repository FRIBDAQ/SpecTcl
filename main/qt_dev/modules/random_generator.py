import numpy as np
from numpy import random

class rnd():

    def __init__(self, mu=None, sigma=None):
        if mu is None or sigma is None:
            mu = int(np.random.uniform(300,500))
            sigma = int(np.random.uniform(10,100))
            self.mu = mu
            self.sigma = sigma
