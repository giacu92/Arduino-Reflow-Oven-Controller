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
Get a fully assembled board [here](http://giacu92.bigcartel.com/product/reflow-oven-controller-board-v1-1) or etch it yourself.

The board can be easely populated. You need to add a 16x2 HD44780 compatibile display, an Arduino Nano and a couple of normally-open momentary switches. You can omit the switches if using with the Java serial controller.
As done just download the files, upload the Arduino sketch and connect it on the board.
If using the momentary switches, they must be connected as shown:

![Connection](http://i65.tinypic.com/2lwvm1l.png)

In this way A0 can be used to detect which button is pressed.

I previously built a breadboard version and now finally developed a board. I got some spare boards in case you're looking for one of this.
![Board](http://i68.tinypic.com/24o6quf.jpg)

The board and all the electronics will be put inside a 3D printed box. This how it will looks like..
![Box](http://i63.tinypic.com/ae1bwy.jpg)

# PROCESSING Serial Controller
By connecting the Arduino Nano USB cable to the PC (currently i'm working with Raspberry Pi as serial controller) you'll be able to control the microcontroller and get rid of the pushbutton interface and display (or use them both together). So I wrote a Serial Controller Interface in Processing which allows me to read and send data back and forth the microcontroller.
The GUI for now contains the essential. It plot the temperature-vs-time graph respect to a standard profile. Now time to tune PID to match thermal capabilities of my oven.
![GUI](http://i68.tinypic.com/8wd15v.png)

### UPDATE:
The newer version of the Controller allows the user to draw a custom temperature profile and send it to the controller board. The profile can only be set in "IDLE MODE". Just choose the temperatures for each reflowing stage and hit "Set" button to send them to the microcontroller.

# Usage with the LCD Keypad SHIELD
![LCD Keypad](http://i68.tinypic.com/scygyw.jpg)
[LCD Keypad resize]: (http://i65.tinypic.com/6rnaqh.jpg)
If you wish to build the controller by yourself you can use the [DFRobot - LCD Keypad SHIELD](https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)) which provides easy access to the LCD display and has a pre-built button interface.
Since the board has different pin arragement the software must be able to know at which type of board it is interfacing to.
You can enable the usage of the LCD Keypad SHIELD by uncommenting the line of code no. 44:
```cpp
44  #define USE_LCD_KEYPAD_SHIELD
```
Using the LCD Keypad Shield forces you to different pin arrangement. My proposal is the following but feel free to modify to as you wish.
![LCD Key pins](http://i64.tinypic.com/2a65a8k.jpg)

# Newer version v2
A newer version of the pcb is under development. It will fit the 16x2 LCD screen and mounts underneath it. Here's a sample rendering. Also a new pid autotuner function and SD card support for saving custom profiles are getting implemented.
![New](http://i67.tinypic.com/30s9a2s.png)

This project is under continuous update (Jan 2018). Contact me for anything :)
