import threading, time
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

class Blink(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.paused = False
        self.pause_cond = threading.Condition(threading.Lock())
    
    def run(self):
        while True:
            with self.pause_cond:
                while self.paused:
                    self.pause_cond.wait()

                #thread should do the thing if
                #not paused
                print('do the thing')
            time.sleep(5)

    def pause(self):
        self.paused = True
        # If in sleep, we acquire immediately, otherwise we wait for thread
        # to release condition. In race, worker will still see self.paused
        # and begin waiting until it's set back to False
        self.pause_cond.acquire()

    #should just resume the thread
    def resume(self):
        self.paused = False
        # Notify so thread will wake after lock released
        self.pause_cond.notify()
        # Now release the lock
        self.pause_cond.release()
