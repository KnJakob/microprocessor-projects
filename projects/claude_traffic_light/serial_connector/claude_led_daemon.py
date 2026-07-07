import serial, time, os, sys

PORT = "/dev/tty.usbserial-1130" # mac
# PORT = "/dev/USB0"               # linux
BAUD_RATE = 115200
TIMEOUT = 1

FIFO_PATH = "/tmp/claude_led_daemon"

def main():
    if not os.path.exists(FIFO_PATH):
        os.mkfifo(FIFO_PATH)

    ser = serial.Serial(PORT, BAUD_RATE, timeout=TIMEOUT)
    time.sleep(1.5)

    try:
        while True:
            with open(FIFO_PATH, 'r') as fifo:
                for line in fifo:
                    line = line.strip()
                    if line:
                        if line == "exit":
                            ser.write("waiting\n".encode())
                            sys.exit(0)
                        ser.write((line + '\n').encode())
    except KeyboardInterrupt:
        ser.write("waiting\n".encode())
        print("Received shutdown..")
    finally:
        if ser.is_open:
            ser.close()
        if os.path.exists(FIFO_PATH):
            os.remove(FIFO_PATH)
    
if __name__ == "__main__":
    main()
