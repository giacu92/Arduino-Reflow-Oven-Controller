# GFE-Reflow-Oven-Controller
This repo contains all the files of my Arduino Nano based Reflow Oven PID Controller for reflow smd soldering.

* Author: Giacomo Mammarella
* email: giacomo.mammarella@student.univaq.it

# File list:
* [GFE_Reflow_Oven.ino](https://github.com/giacu92/GFE-Reflow-Oven-Controller/blob/master/GFE_Reflow_Oven.ino): is the hex file precompiled for my Arduino Nano controller Board.
* [Profile_Setting.ino](https://github.com/giacu92/GFE-Reflow-Oven-Controller/blob/master/Profile_Setting.ino): this file contains a function to choose between Lead-Free or Leaded solder paste.
* [Prova.brd](https://github.com/giacu92/GFE-Reflow-Oven-Controller/blob/master/Prova.brd): Cadsoft Eagle 6.5 .brd file of my Reflow Oven Controller. 
* [Prova.sch](https://github.com/giacu92/GFE-Reflow-Oven-Controller/blob/master/Prova.sch): Cadsoft Eagle 6.5 .sch file of my Reflow Oven Controller.

# Mounting the board:
The board can be easely populated (if not I can provide a populated one, reflow soldered eheh. Just email me). You need to add a 16x2 HD44780 compatibile display and a couple of normally-open momentary switches. As done just download the files, upload the arduino sketch and connect it to the board.
The momentary switches must be connected as shown:

![Connection](http://i65.tinypic.com/2lwvm1l.png)

In this way A0 can be used to detect which button is pressed.

I previously built a breadboard version and now finally developed a board. Can't wait them to arrive! they're on fab right now..
![Board](http://i68.tinypic.com/24o6quf.jpg)

The board and all the electronics will be put inside a 3D printed box. This how it will looks like..
![Box](http://i63.tinypic.com/ae1bwy.jpg)

# Testing the Processing serial controller:
I finally tested my serial controller written in Processing. The GUI for now contains the essential. It plot the temperature vs time graph respect to a standard profile. Now time to tune PID to match thermal capabilities of my oven.
![GUI](http://i65.tinypic.com/2wqy1bp.png)

This project is under continuous updating (July 2017). Contact me for anything :)
