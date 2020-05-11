# Electronic Load
This repository hosts my work in developping a custom electronic load from scratch. 
For more information on what an electronic load does and what it can be used for, see [this](https://www.mouser.com/pdfdocs/DC_Electronic_Load_Application_Note.pdf). 

Specifically, this load targets DC-DC converters testing, as it includes input and output power monitoring.

I you have any question on this repository, feel free to contact the owner.

---

### Features
* Constant Current, Power and Resistance modes
* Battery capacity logging with adjustable cutoff voltage
* Input and output power logging : instant efficiency reading for DC-DC converters
* Rotary encoder driven interface
* Dedicated Load ON/OFF button
* Intuitive LCD menu system with editable values
* 12bit current, voltage and power measurements 
* 30W continuous sinking capability, 250W peak
* Temperature controlled fan.
* Adjustable acquistion rate
* 115200 bauds serial communication through USB with a PC (non-isolated !)

### Specifications
> Enough talking, give us the specs, son !

Altough the device has not been fully qualified through systematic testing, here are the basic specs you can expect. 
* Input voltage range : 0 - 100V DC maximum, 2-80V DC regulating
* Supply voltage range : 0 - 16V maximum, 12V nominal 
* Device current consumption : 50mA nominal (0.6W)
* Load current range : 5mA - 5.25A, in 8mA steps
* Current regulation : 5% +/- 8mA typical @ 8kHz bandwidth
* Transient response : rise time < 1µs/A, overshoot < 2%
* Constant Power/Resistance regulation bandwidth : 10Hz

---

## Schematic and Layout
The heart of the system is an Atmel ATmega328P AVR microcontroller with 32kB of Flash and 2k of RAM running at a whopping 16MHz. It interfaces an I2C 12bit DAC (the MCP47FEB21A1) and the LTC2992 dual power monitor chip.


The USB serial interface uses the MCP2221A bridge. The display is a classic 16x2 LCD.

On the analog part, 10mΩ  and 0.2Ω shunt resistors are used, coupled with two W9NK90Z N-ch power MOSFETS. The heatsink was salvaged from an old graphics card. Current regulation is achieved through the help of a MCP6H02 opamp, and a carefully designed feedback network which guarantees the specified rise time with no overshoot.


The layout was done using the Eagle software (v9.5.2). Gerbers, Eagle project files, top and bottom views are available on the `pcb` folder. PCBs were printed using JLCPCB service.
The schematic is available in a pdf and Eagle `.sch` format.


There are several issues regarding the layout of the PCB, as it is a first version. Improper Kelvin connections to the current sense resistors, or wrong hole size for the power input connector by example. The heatsink originally used is salvaged from old equipement and the footprint may not be relevant to anyone else. Space for rubber feet placement should be included. Also, the LCD is too close to the heatsink, which is impractical during assembly (especially accessing the fan and temp sensor connector).




---

## Firmware
The firmware for this device is currently in revision 1.3. It is written for a barebone ATmega328p Atmel microcontroller.

```Sketch uses 18604 bytes (60%) of program storage space. Maximum is 30720 bytes.```

```Global variables use 1635 bytes (79%) of dynamic memory, leaving 413 bytes for local variables. Maximum is 2048 bytes.```

As you can see it's not the lightest firmware, mostly due to the use of the `Wire` and `LCD` librairies from Arduino, which are bulky. However, there is still enough room for future improvements. Just keep in mind if you want to improve the project to leave space for local variables allocation.
### Programming
An ISP port is provided on the board, making reflashing easy. All you need is an AVR-compatible programmer with the standard 10 pins ICSP header. I used the very good (and very cheap!) [Usbasp] during developpement. Avrdude can be used to download the compiled binary `main.hex`.

### Software structure


### Usage

---

## Improvements

* Design a mechanical mount for the fan / stabilize the board
* Re-think the menu system to be more modular / lower size
* Add logging capability with serial interface
* Add current sweep programming for DC-DC converters automatic characterization (with di/dv auto resolution)
* Fix the minimum current issue (currently min is 5mA)
* Fix the 4 terminal Kelvin connection on the 10mΩ current sense resistors in the PCB layout
* ...
