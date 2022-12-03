import tkinter as tk
from tkinter import ttk
import serial, threading, leds, os, re
import matplotlib.pyplot as plt
from serial.tools.list_ports import comports
import sv_ttk

class App(tk.Tk):
    _serialport = ""
    _baudrate = 115200
    _portlist = []
    _isConnected = False
    _serialCon = None
    _threadTXled = None
    _baudrate_list = [4800, 9600, 19200, 28800, 57600, 115200]

    _startX = 40
    _startY = 320
    _stopX = 738
    _stopY = 40
    _stepX = int((_stopX-50-_startX)/12)
    _stepY = int((_startY-_stopY+10)/6 )

    TEMPERATURE_SOAK_MIN   = 150
    TEMPERATURE_SOAK_MAX   = 177
    TEMPERATURE_REFLOW_MAX = 230
    TEMPERATURE_COOLDOWN   = 100

    DURATION_PH             = 75
    DURATION_SOAK           = 105
    DURATION_REFLOW_RAMPUP  = 60


    
    def __init__(self):
        super().__init__()
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        
        self._portlist = serial.tools.list_ports.comports()
        
        ## Configure the root window
        self.title("Reflow oven serial controller")
        self.geometry("800x600")

        self.resizable(0, 0)
        #self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)

        #self.rowconfigure(0, weight=1)
        #self.rowconfigure(1, weight=2)

        #self.call("source", "azure.tcl")
        #self.call("set_theme", "light")
        sv_ttk.set_theme("light")

        self.setup_widgets()

        ## Manage serial
        self.manageSerial()
    
    def setup_widgets(self):

        ## Serial Port Frame
        self.cb_frame = ttk.LabelFrame(self, text="Serial port config", padding=(10,10))
        #self.cb_frame.grid(row=0, column=0, padx=(20, 10), pady=(20, 10), sticky="nsew")
        self.cb_frame.grid(row=0, column=0, padx=(20, 20), pady=(10, 10), sticky="nsew")

        #   combo-box - COM list
        self.combobox = ttk.Combobox(self.cb_frame, textvariable="serial_port", width=50, height=10, justify='left')
        self.combobox['values'] = [ p.device for p in self._portlist ]
        self.combobox.set(self._portlist[0].device)
        #self.combobox.place(x=10, y=12)
        self.combobox.grid(row=0, column=0, padx=5, pady=10, sticky="nsew")
        self.combobox.bind("<<ComboboxSelected>>", self.on_select)

        #   combo-box - baudrate
        self.cb_baudrate = ttk.Combobox(self.cb_frame, textvariable="baudvar", width=50, height=10, justify='left')
        self.cb_baudrate['values'] = self._baudrate_list
        self.cb_baudrate.set(self._baudrate_list[1])
        self.cb_baudrate.grid(row=1, column=0, padx=5, pady=10, sticky="nsew")
        #self.cb_baudrate.place(x=10, y=32)
        #self.cb_baudrate.bind("<<ComboboxSelected>>", self.on_select)

        #   button - refresh
        self.btn_refresh = ttk.Button(self.cb_frame, text="Refresh", width=10)
        self.btn_refresh['command'] = self.refresh_clicked
        self.btn_refresh.grid(row=0, column=1, padx=5, pady=10, sticky="nsew")
        #self.btn_refresh.place(x=380, y=8)
        
        #   button - connect
        self.btn_connect = ttk.Button(self.cb_frame, text="Connect", width=10)
        self.btn_connect['command'] = self.connect_clicked
        self.btn_connect.grid(row=1, column=1, padx=5, pady=10, sticky="nsew")
        #self.btn_connect.place(x=380, y=32)

        #   label - selected_port
        self.label = ttk.Label(self.cb_frame, text=self._portlist[self.combobox.current()].description)
        self.label.grid(row=2, column=0, padx=5, pady=10, sticky="nsew")
        #self.label.place(x=10,y=60)


        ## Led Frame
        self.leds_frame = ttk.LabelFrame(self, text="Controls", padding=(10,10))
        self.leds_frame.grid(row=0, column=1, padx=(20, 20), pady=(10, 10), sticky="nsew")
        
        ## Canvas
        #   led canvas
        self.led_canvas = tk.Canvas(self.leds_frame, width=150, height=45)
        self.led_canvas.grid(row=0, column=1, padx=5, pady=10, sticky="nsew")
        #self.led_canvas.place(x=600,y=00)   #grid(row=3,column=0, columnspan=5)

        #   leds
        #self.ledCon = self.canvas.create_oval(10, 20, 25, 35, fill='#e6e6c8')   #   #e6e6c8 - yellow off
        #self.ledTX  = self.canvas.create_oval(40, 20, 55, 35, fill='#a5d4b6')   #   #b4ebc8 - green off
        #self.ledRX  = self.canvas.create_oval(70, 20, 85, 35, fill='#d18a8a')   #   #d18a8a - red off

        self.ledCon = leds.led(self.led_canvas, 20,  20, 15, '#00ff00', "CON", offColor="#ff0000")
        self.ledTX  = leds.led(self.led_canvas, 70,  20, 15, 'YELLOW', "TX",  offColor="#f5dca9")
        self.ledRX  = leds.led(self.led_canvas, 120, 20, 15, 'GREEN', "RX")

        ## Threads
        ##self.ledTX_thread = threading.Timer(1, self.tx_blink, None) #target=self.tx_blink
        ##self.ledTX_thread.start()
        self._threadTXled = leds.Blink

        #self.tx_blink()
        #self.rx_blink()

        #   graph canvas
        self.graph_canvas = tk.Canvas(width=775, height=480, bg="white")
        self.graph_canvas.grid(row=1, column=0, columnspan=2, padx=(20, 20), pady=(10, 10), sticky="nsew")

        # AXIS:
        self.graph_canvas.create_line(self._startX, self._startY, self._stopX, self._startY, width=3) # X AXIS
        self.graph_canvas.create_line(self._startX, self._startY, self._startX, self._stopY, width=3) # Y AXIS
        # lineette
        for i in range(self._stepX, self._stopX-self._startX-10, self._stepX):
            self.graph_canvas.create_line(self._startX+i,self._startY-5,self._startX+i,self._startY+5)
        for i in range(self._stepY, self._startY-self._stopY-10, self._stepY):
            self.graph_canvas.create_line(self._startX-5,self._startY-i,self._startX+5,self._startY-i)

        # Reference plot
        TEMP_SOAK_MIN_PL    = self._startY - round(self._map(int(self.TEMPERATURE_SOAK_MIN), 0, 50, 0, self._stepY))
        TEMP_SOAK_MAX_PL    = self._startY - round(self._map(int(self.TEMPERATURE_SOAK_MAX), 0, 50, 0, self._stepY))
        TEMP_REFLOW_MAX_PL  = self._startY - round(self._map(int(self.TEMPERATURE_REFLOW_MAX), 0, 50, 0, self._stepY))
        TEMP_COOL_MIN_PL    = self._startY - round(self._map(int(self.TEMPERATURE_COOLDOWN), 0, 50, 0, self._stepY))



    def _map(self, x, in_min, in_max, out_min, out_max):
        return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
    
        
    def refresh_clicked(self):
        self._portlist = []
        self._portlist = serial.tools.list_ports.comports()
        
        self.combobox['values'] = [ p.device for p in self._portlist ]
        self.label['text'] = self._portlist[self.combobox.current()].description
        
    def connect_clicked(self):
        if self._isConnected == False:
            self._serialport = self.combobox.get()
            self._baudrate = self.cb_baudrate.get()
            print ('[DEBUG] trying connection to:', self._serialport, "at", self._baudrate, "bauds")
            try:
                self._serialCon = serial.Serial(self._serialport, baudrate=self._baudrate, timeout=2)
                self.ledCon.setColor("#ffb300")
            except serial.SerialTimeoutException:
                print ('[ERROR] timeout occurred on connection')
            except:
                print ('[ERROR] Unidentified error occourred during serial connection')
            else:
                self.onConnect()
        else:
            print ("[DEBUG]" , "Serial port disconnection ...")
            try:
                self._serialCon.close()
            except:
                print ("[ERROR]" , "Unidentified error occourred during serial disconnection")
            else:
                self.onDisconnect()              
        
    def on_select(self, event):
        self._portlist = serial.tools.list_ports.comports()
        event.widget['values'] = [ p.device for p in self._portlist ]
        
        #self.label['text'] = "whuu"

    def onConnect(self):
        self.btn_connect['text'] = "Disconnect"
        self._isConnected = True
        self.combobox['state'] = 'disabled'
        self.cb_baudrate['state'] = 'disabled'
        self.btn_refresh['state'] = 'disabled'
        self.ledCon.setState(True)
        print ('[DEBUG]', self._serialCon)
        #self.ledTX_thread.start()
        
    
    def onDisconnect(self):
        self._isConnected = False
        self.btn_connect['text'] = "Connect"
        self.combobox['state'] = 'enabled'
        self.cb_baudrate['state'] = 'enabled'
        self.btn_refresh['state'] = 'enabled'
        self.ledCon.setState(False)
        #self.ledTX_thread.cancel()

    def on_closing(self):
        os._exit(1)
        #sys.exit()

    def unpack_serial(self, str_in):
        #ser = bytes.decode("utf-8")
        #ser = str(bytes.decode("utf-8"))
        #ser = (aa,bb)
        vals = [int(i.group()) for i in re.finditer(r'\d+', str_in)]

        print(vals)
        
        return vals

    ## Thread function:
    def tx_blink(self):
        #if self._isConnected is False:
        #    print("[DEBUG]", "self._isConnected", self._isConnected)

        thr = threading.Timer(0.1, self.tx_blink)
        thr.start()
        if self._isConnected and self._serialCon.inWaiting() > 0:
            #print("[DEBUG]","self._serialCon.inWaiting()" , self._serialCon.in_waiting)
            self.ledTX.setState(True)
        else:
           self.ledTX.setState(False)
           #thr.cancel()

    def rx_blink(self):
        #if self._isConnected is False:
        #    print("[DEBUG]", "self._isConnected", self._isConnected)

        thr = threading.Timer(0.1, self.rx_blink)
        thr.start()
        if self._isConnected and self._serialCon.inWaiting() > 0:
            #print("[DEBUG]","self._serialCon.inWaiting()" , self._serialCon.in_waiting)
            self.ledRX.setState(True)
        else:
           self.ledRX.setState(False)
           #thr.cancel()

    def manageSerial(self):
        thr = threading.Timer(1, self.manageSerial)
        thr.start()

        if self._isConnected is True:
            bytesToRead = self._serialCon.inWaiting()
            if bytesToRead > 0:
                #ser_bytes = self._serialCon.read(bytesToRead)
                ser_str = self._serialCon.readline().rstrip()
                print(ser_str)
                str = self.unpack_serial(ser_str)
                
                #print(ser_bytes, self._serialCon.inWaiting())


if __name__ == "__main__":
    app = App()
    app.mainloop()