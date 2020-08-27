# Arduino Clock
This is a personal project made with the intent to learn more about computer-controlled electronics.

You can read more about this project over at <a href="https://jarod-f.github.io/portfolio-site/pages/clock.html">jarod-f.github.io/portfolio-site/pages/clock.html</a>

Below is an image showing what the final product looked like.

<img style="max-height: 350px;" src="https://jarod-f.github.io/portfolio-site/media/clock/clock-turned-on.jpg">

# Summary
This project has two components, the hardware and the software.

## Hardware
For hardware, the three shift registers are daisy-chained together.  This minimizes the connections to the Arduino.

The shift register outputs are connected to the displays.  This is how the displays know which numbers to show.

The resistors are required, if they are missing, the display LEDs could potentially be damaged.

## Software
For the software side of things, a computer running Python 3 will send the current time to the connected Arduino.
- This happens every half a second and is done through the COM5 usb/serial port.

The arduino is constantly looping through two functions:
- One checks if serial data has arrived.  If it has, it processes it into a time format that the Arduino can understand.
- The other handles the displays.  It is responsible for activating the displays and shifting out the correct data to the shift registers.

## Dependencies
<a href="https://pypi.org/project/pyserial/">pyserial 3.4</a>

# Materials
- An OS capable of running Python 3 (I used Windows 10)
- An Arduino Uno mounted with a ATmega328P microcontroller.
- One 3461bs 4-digit 7-segment display
- Two 5101as 7-segment displays
- Three 74HC595 shift registers
- Twenty 220Ω resistors
- A breadboard
- A lot of 22 AWG sold wires. (Jumper cables will work, they're just not as clean)


# Contact
Email: forbesjarod@gmail.com

My portfolio: <a href="https://jarod-f.github.io/portfolio-site/">jarod-f.github.io/portfolio-site/</a>
