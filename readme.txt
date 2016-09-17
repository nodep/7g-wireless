This project implements a replacement wireless keyboard controller which can be plugged into the SteelSeries 7G keyboard in place of the original controller.

The controller is designed around an Atmel ATMega169A. It communicated with the USB dongle (nRF24LU1+) through an nRF24L01+ 2.4GHz radio transceiver.

With aggressive power optimizations I have achieved an average consumption of 20 μA on the keyboard controller. It can run 6 to 10 years on two low self-discharge AA batteries or a year on a single CR2032 lithium cell. The USB dongle implements a standard HID keyboard. The keyboard also has a driverless configuration menu.

The dongle code can be compiled with sdcc (Small Device C Compiler) and the keyboard controller with avr-gcc.

The controller schematic and PCB are designed with EagleCAD.