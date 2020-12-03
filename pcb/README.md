# ![Brew Bubbles Logo](https://github.com/lbussy/brew-bubbles/raw/master/graphics/BB%20full%20logo.png)

# Brew Bubbles Shield

This folder contains the Eagle files for printed circuit boards supporting [Brew Bubbles](https://www.brewbubbles.com/).

The shield provides the necessary component connections and circuitry for the ESP-8266 controller used in this project.  It is nearly identical in size to the Wemos D1 mini we use to provide a very compact and lightweight footprint.

[Order from OSHPark.com](https://oshpark.com/shared_projects/NNPU5x7b):

- 2 layer board of 1.01 x 1.13 inches (25.6 x 28.7 mm) 
- $5.65 per set of 3

Please see [the documentation](https://docs.brewbubbles.com) for additional information.

## BOM

- 3 x C1 0.1μF Ceramic Capacitor (C1-3)
- *2 x 2.2kΩ 1/4W 5% Axial Resistors (R1-2)
- 1 x 150Ω 1/4W 5% Axial Resistor (R3)
- 1 x Sharp GP1A57HR Transmissive Photointerrupter (U1)
- *2 x 3-pin 90° Male Header (ROOM, VESSEL)

This section of the BOM is available on Mouser via [this link](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=216fcbe935).

Additionally you will need:

- 1 x Wemos D1 Mini (U2)
- 2 x 8-Pin stacking male headers (almost always comes with the D1)
- 2 x 8-Pin stacking female headers (almost always comes with the D1)
- *2 x Waterproof DS18B20 Temperature Sensor with lead

(*) - Optional items for use when temperature sensors are desired.  They are in the design to provide a means to monitor and trend one's ambient temperature where the fermenter is placed, and/or the fermenting liquid's temperature via a thermowell or insulated in contact with the fermenter. If one chooses not to use these, the firmware will automatically skip reporting these readings.

## 3D-Printed Bracket

See [this project on Thingiverse](https://www.thingiverse.com/thing:4020905) from @gromitdj on [Homebrewtalk.com](https://homebrewtalk.com) (original from @matridium on [Homebrewtalk.com](https://homebrewtalk.com)) for a bracket you can print.

## Modifying These Files
If you would like to personalize these board designs, you may modify them with [Autodesk's EAGLE](https://www.autodesk.com/products/eagle/overview). EAGLE is a scriptable electronic design automation (EDA) application with schematic capture, printed circuit board (PCB) layout, auto-router and computer-aided manufacturing (CAM) features. EAGLE stands for Easily Applicable Graphical Layout Editor and is developed by CadSoft Computer GmbH. The company was acquired by Autodesk Inc. in 2016.  

The program supports Windows, Linux, and Mac OS X.  EAGLE is available in a [free version](https://www.autodesk.com/products/eagle/free-download), as well as a [subscription-based version with more features](https://www.autodesk.com/products/eagle/compare).
