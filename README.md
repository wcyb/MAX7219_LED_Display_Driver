# MAX7219 LED Display Driver
Full support of MAX7219 LED display driver.
* Includes functions for displaying numbers (int and double, positive and negative)
* When displaying floating point numbers, you can set what precision of displayed number you need
* Includes a demo sketch to show how you can use this library

Known issues:
* Because Arduino (AVR based) stores double as 4 byte long, remember that there may be rounding errors

Important things:
* You should not display 3 or less digits, due to excessive power dissipation in this case. If you need to do this, then change Rset resistor on module to lower current for all segments.
* You can add pull-down resistors if needed.

### Only for non-commercial use.
