# Brew Bubbles Shield

This folder contains the Eagle files for printed circuit boards supporting [Brew Bubbles](https://www.brewbubbles.com/).

The shield provides the necessary component connections and circuitry for the ESP-8266 controller used in this project.  It is nearly identical in size to the Wemos D1 mini we use to provide a very compact and lightweight footprint.

| **Top** | **Bottom** |
|---|---|
| ![](Top.png) | ![](Bottom.png) |

## Ease of Manufacture

While the shield is comparatively small, the components chosen are simple through-hole parts which may be easily soldered by a beginner with a little patience.  I recommend the following order for ease of assembly:

1. Resistors - As the shortest mounted components, soldering the three resistors to the board first will be easiest.  They are also some of hte most tolerant components so these will give you some experience to get you going.
2. 3-Pin headers - These components are not sensitive to the heat at all except for the plastic 
3. Capacitors - These are mounted next.  Be sure to get them as close to the board as possible as having them stick up will change their impact on the circuit.
4. 8-pin female headers - These are the tallest items on the front side of the board and will be the last pieces to go on this side.  Lightly tack on one pin and make sure the header is straight.  When you have it positioned correctly, start from the other end and solder the pins properly.  If you have a D1 laying around with the pin headers soldered on it already, using that to steady the parts will help.  This is a chicken or the egg choice with #5.  The first part, either controller or shield, will be hte most difficult.  After that you can use a completed "opposite" to steady the headers of the first.
5. 8-pin male headers - These need to be soldered on the controller board.  See note on #4 above.
6. GP1A57HR photointerrupter - If the controller is still plugged in, take it off temporarily.  The photointerrupter goes on the *back* side of the circuit board in the outline provided, therefore you solder it on the top side.

## Bill of Materials

### General Parts

Since this uses 1 or 2 each of very common, very inexpensive components, you may find you are better off buying a resistor assortment for instance or end up buying 10 or 20 at a time.  Make several!

| **Quan** | **Description** | **Placement** |
|---|---|---|
| 3 | 0.1μF 10V Ceramic Capacitor | C1, C2. C3 |
| *2 | 2.2kΩ 1/4W 5% Axial resistor | R1, R2 |
| 1 | 150Ω 1/4W 5%  Axial resistor | R3 |
| *2 | 3-Pin 90° Header | VESSEL, ROOM |
| 1 | Sharp GP1A57HR Transmissive Photointerrupter | U1 |

This BOM is available on Mouser [via this link](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=216fcbe935).

### Other Parts

| **Quan** | **Description** | **Placement** |
|---|---|---|
| 1 | WeMos D1 Mini R2 | U2 |
| 2 | 8-pin male stackable header (nearly always comes with the D1) |  |
| 2 | 8-pin female stackable header (nearly always comes with the D1) |  |
| *2 | Waterproof DS18B20 Temperature Sensor with lead | VESSEL, ROOM |

### Optional Parts

The items marked with an *asterisk above are optional.  They are in the design to provide a means to monitor and trend one's ambient temperature where the fermenter is placed, and/or the fermenting liquid's temperature via a thermowell or insulated in contact with the fermenter.  If one chooses not to use these, the firmware will automatically skip reporting these readings.

### Order Boards

Order 3 boards for $5.65 from [Oshpark using this link](https://oshpark.com/shared_projects/NNPU5x7b), or download the Brew-Bubbles.brd file and use it with the board fabrication house of your choosing.

## Modifying These Files
If you would like to personalize these board designs, you may modify them with [Autodesk's EAGLE](https://www.autodesk.com/products/eagle/overview). EAGLE is a scriptable electronic design automation (EDA) application with schematic capture, printed circuit board (PCB) layout, auto-router and computer-aided manufacturing (CAM) features. EAGLE stands for Easily Applicable Graphical Layout Editor and is developed by CadSoft Computer GmbH. The company was acquired by Autodesk Inc. in 2016.  

The program supports Windows, Linux, and Mac OS X.  EAGLE is available in a [free version](https://www.autodesk.com/products/eagle/free-download), as well as a [subscription-based version with more features](https://www.autodesk.com/products/eagle/compare).
