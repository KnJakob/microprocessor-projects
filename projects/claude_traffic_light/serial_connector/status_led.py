import serial, sys, time

PORT = "/dev/tty.usbserial-1130" # mac
# PORT = "/dev/USB0"               # linux
BAUD_RATE = 115200
TIMEOUT = 1

ser = serial.Serial(PORT, BAUD_RATE, timeout=TIMEOUT)
time.sleep(1.5)
ser.write((sys.argv[1] + "\n").encode())
ser.close()
