# Brew Bubbles Shield v1.0

This shield provides the necessary component connections and circuitry for the [Brew Bubbles](https://www.brewbubbles.com/) project.  It is nearly identical in size to the Wemos D1 mini we use to provide a very compact and lightweight footprint.

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
| 1 | Wemos D1 Mini R2 | U2 |
| 2 | 8-pin male stackable header (nearly always comes with the D1) |  |
| 2 | 8-pin female stackable header (nearly always comes with the D1) |  |
| *2 | Waterproof DS18B20 Temperature Sensor with lead | VESSEL, ROOM |

### Optional Parts

The items marked with an *asterisk above are optional.  They are in the design to provide a means to monitor and trend one's ambient temperature where the fermenter is placed, and/or the fermenting liquid's temperature via a thermowell or insulated in contact with the fermenter.  If one chooses not to use these, the firmware will automatically skip reporting these readings.
