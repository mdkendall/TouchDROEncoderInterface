# Touch DRO Encoder Interface

Firmware for an ESP32 to provide an interface between quadrature encoders
and the TouchDRO App on a tablet.

The quadrature encoders read the movement of the axis of a mill or lathe.
This firmware was written to work with a Sherline 5400 mill equipped with
encoders on the handwheels. This is the setup supplied by Sherline for use
with their own simple DRO box. It is not great because it reads the position
of the leadscrew rather than the bed itself, and hence there is some backlash.
This firmware provides for backlash compensation in the same way the Sherline
DRO box does, by accounting for a fixed deadzone on each axis.

Although not tested, this firmware should also be compatible with more
conventional scales that have quadrature outputs, e.g. glass or magnetic scales.
It is not compatible with the serial data output of capacitive scales.

The firmware also supports a tachometer input for spindle speed measurement.

Connection between the ESP32 and the tablet is via a Bluetooth serial link.
The format is that required by the TouchDRO App.

https://www.touchdro.com/

Built using PlatformIO, the Arduino framework, and configured for an ESP32Dev board.

Configure the pins to use for the encoders, and the amount of backlash on each axis,
in the initialiser for `axis[]`. The backlash is specified in counts (not inches or mm).
Enter half the required total amount.

Configure the pin to use for the tacho with `PIN_TACHO`.

Configure the name of the offered Bluetooth connection with `BT_NAME`.
