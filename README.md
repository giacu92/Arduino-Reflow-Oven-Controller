# GFE-Reflow-Oven-Controller
This repo contains all the files of my Arduino Nano based Reflow Oven PID Controller for reflow smd soldering.

* Author: Giacomo Mammarella
* email: giacomo.mammarella@student.univaq.it

# File list:
* [Arduino files](https://github.com/giacu92/Reflow-Oven-Controller/tree/master/GFE_Reflow_Oven_Controller): The Arduino files source folder.
* [Processing file](https://github.com/giacu92/Reflow-Oven-Controller/tree/master/Processing/Reflow_oven_serial_controller): The Processing files source folder.
* [Prova.brd](https://github.com/giacu92/GFE-Reflow-Oven-Controller/blob/master/Prova.brd): Cadsoft Eagle 6.5 .brd file of my Reflow Oven Controller. 
* [Prova.sch](https://github.com/giacu92/GFE-Reflow-Oven-Controller/blob/master/Prova.sch): Cadsoft Eagle 6.5 .sch file of my Reflow Oven Controller.

# Mounting the board:
The board can be easely populated (if not I can provide a populated one, reflow soldered eheh. Just email me). You need to add a 16x2 HD44780 compatibile display and a couple of normally-open momentary switches. As done just download the files, upload the arduino sketch and connect it to the board.
The momentary switches must be connected as shown:

![Connection](http://i65.tinypic.com/2lwvm1l.png)

In this way A0 can be used to detect which button is pressed.

I previously built a breadboard version and now finally developed a board. I got some spare boards in case you're looking for one of this.
![Board](http://i68.tinypic.com/24o6quf.jpg)

The board and all the electronics will be put inside a 3D printed box. This how it will looks like..
![Box](http://i63.tinypic.com/ae1bwy.jpg)

# PROCESSING Serial Controller
By connecting the Arduino Nano USB cable to the PC (currently i'm working with Raspberry Pi as serial controller) you'll be able to control the microcontroller and get rid of the pushbutton interface and display (or use them both together). So I wrote a Serial Controller Interface in Processing which allows me to read and send data back and forth the microcontroller.
The GUI for now contains the essential. It plot the temperature-vs-time graph respect to a standard profile. Now time to tune PID to match thermal capabilities of my oven.
![GUI](http://i65.tinypic.com/2wqy1bp.png)

### UPDATE:
The newer version of the Controller allows the user to draw a custom temperature profile and send it to the controller board. The profile can only be set in "IDLE MODE". Just choose the temperatures for each reflowing stage and hit "Set" button to send them to the microcontroller.

This project is under continuous update (July 2017). Contact me for anything :)
