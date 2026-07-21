#!/usr/bin/env python3
"""
Reads comma-separated ADC readings from an Arduino over serial and
displays them as voltages.

Requires pyserial:
    pip install pyserial
"""

import serial
import sys
import time

# ---- Configuration ----
PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
VREF = 5.0            # Arduino analog reference voltage (5.0 for Uno/Nano, 3.3 for some boards)
ADC_MAX = 1023.0      # 10-bit ADC -> 0-1023
NUM_CHANNELS = 6       # Send '1'-'6' to change how many channels the Arduino reports

CHANNEL_NAMES = ["A0", "A1", "A2", "A3", "A6", "A7"]


def main():
    try:
        ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    except serial.SerialException as e:
        print(f"Could not open serial port {PORT}: {e}")
        sys.exit(1)

    # Give the Arduino time to reset after opening the serial connection
    time.sleep(2)

    # Tell the Arduino how many channels to report
    ser.write(str(NUM_CHANNELS).encode())

    print(f"Reading {NUM_CHANNELS} channel(s) from {PORT} @ {BAUD_RATE} baud. Press Ctrl+C to stop.\n")

    try:
        while True:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue

            raw_values = line.split(",")

            try:
                readings = [int(v) for v in raw_values]
            except ValueError:
                # Skip malformed/partial lines
                continue

            voltages = [(r / ADC_MAX) * VREF for r in readings]

            display = "  ".join(
                f"{CHANNEL_NAMES[i]}: {v:5.3f} V"
                for i, v in enumerate(voltages)
            )
            print(display)

    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        ser.close()


if __name__ == "__main__":
    main()
