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

def create_range(self, bins, vmin, vmax):
    x = []
    step = (vmax-vmin)/bins
    for i in np.arange(vmin, vmax, step):
        x.append(i)
    return x
                                                                                                                            
