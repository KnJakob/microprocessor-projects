# status_led.py — keeps the serial port open, called by hooks
import serial, sys, time

PORT = "/dev/ttyUSB0" # linux
ser = serial.Serial(PORT, 115200, timeout=1)
time.sleep(2)  # let ESP32 finish its boot-reset the first time
ser.write((sys.argv[1] + "\n").encode())
ser.close()
