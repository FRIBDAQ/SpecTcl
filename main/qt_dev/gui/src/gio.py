#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt

data = np.random.random((10,10))

# To make a standalone example, I'm skipping initializing the
# `Figure` and `FigureCanvas` and using `plt.figure()` instead...
# `plt.draw()` would work for this figure, but the rest is identical.
fig, ax = plt.subplots()
ax.set(title='Click to update the data')
im = ax.imshow(data)

def update(event):
        im.set_data(np.random.random((10,10)))
        fig.canvas.draw()
            
fig.canvas.mpl_connect('button_press_event', update)
plt.show()
