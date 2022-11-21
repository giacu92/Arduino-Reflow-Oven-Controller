from reflow import App

class led(App):

    _onColor = "#000000"
    _offColor = "#000000"
    _cvas = None

    def __init__(self, cvas, x, y, radius, color, name, toff_x=7, toff_y=10, offColor="#000000"):
        
        self._cvas = cvas
        
        if color == 'RED':
            self._onColor  = "#ff0000"
            self._offColor = "#d18a8a"
        elif color == 'GREEN':
            self._onColor  = "#00ff00"
            self._offColor = "#a5d4b6"
        elif color == 'YELLOW':
            self._onColor  = "#ffff00"
            self._offColor = "#e6e6c8"
        else:
            self._onColor = color
            self._offColor = offColor

        


        self.led = self._cvas.create_oval(x, y+toff_y, x+radius, y+radius+toff_y, fill=self._offColor)   #   #e6e6c8 - yellow off
        self.l_text = self._cvas.create_text(x+toff_x, y, fill="black", text=name)
    
    def setState(self, stat):
        if stat == True:
            #self.led['fill'] = str(self._onColor)
            self._cvas.itemconfig(self.led, fill=self._onColor)
        else:
            #self.led['fill'] = str(self._offColor)
            self._cvas.itemconfig(self.led, fill=self._offColor)
        
    def setColor(self, color):
        self._cvas.itemconfig(self.led, fill=color)
