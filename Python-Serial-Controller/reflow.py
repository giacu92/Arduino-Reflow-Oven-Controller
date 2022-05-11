import tkinter as tk
from tkinter import ttk
import serial
from serial.tools.list_ports import comports

class App(tk.Tk):
    serialport = ""
    baudrate = 115200
    portlist = []
    
    def __init__(self):
        super().__init__()
        
        self.portlist = serial.tools.list_ports.comports()
        
        # Configure the root window
        self.title("Reflow oven serial controller")
        self.geometry("500x300")
        
        # button
        self.btn_refresh = ttk.Button(self, text="Refresh", width=10)
        self.btn_refresh['command'] = self.refresh_clicked
        self.btn_refresh.place(x=340, y=10)
        
        self.btn_connect = ttk.Button(self, text="Connect", width=10)
        self.btn_connect['command'] = self.connect_clicked
        self.btn_connect.place(x=420, y=10)
       
        # combo-box
        self.combobox = ttk.Combobox(self, textvariable="serial_port", width=10, height=10, justify='left')
        self.combobox['values'] = [ p.device for p in self.portlist ]
        self.combobox.set(self.portlist[0].device)
        self.combobox.place(x=10, y=10)
        self.combobox.bind("<<ComboboxSelected>>", self.on_select)
        
        #label
        self.label = ttk.Label(self, text=self.portlist[self.combobox.current()].description)
        self.label.place(x=100,y=10)
        
    def refresh_clicked(self):
        self.portlist = []
        self.portlist = serial.tools.list_ports.comports()
        
        self.combobox['values'] = [ p.device for p in self.portlist ]
        self.label['text'] = self.portlist[self.combobox.current()].description
        
    def connect_clicked(self):
        self.serialport = self.combobox.get()
        print ('[DEBUG] trying connection to:', self.serialport)
        serial.Serial(self.serialport, baudrate=self.baudrate, timeout=2)
        
    def on_select(self, event):
        self.portlist = serial.tools.list_ports.comports()
        event.widget['values'] = [ p.device for p in self.portlist ]
        
        #self.label['text'] = "whuu"
        
if __name__ == "__main__":
    app = App()
    app.mainloop()