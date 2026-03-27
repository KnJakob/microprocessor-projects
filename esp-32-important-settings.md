# Settings in the Arduino IDE for the Esp32

## Connect
The Esp32 should be connected in a port like /dev/ttyUSB*
-> to check connect it and then search in the kernel logs for the device 
>sudo dmesg | tail -20
>ls /dev/ttyUSB*

## Settings
Board: Arduino Nano
Port: /dev/ttyUSB*
Processor: ATmega328P (old bootloader)

For more informaion see the pdf about the device in this dir.
