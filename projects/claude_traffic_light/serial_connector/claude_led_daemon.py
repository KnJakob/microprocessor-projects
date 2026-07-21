import serial, time, os, sys
import select
from claude_settings import check_hooks

PORT = "/dev/tty.usbserial-1130" # mac
# PORT = "/dev/ttyUSB0"               # linux
BAUD_RATE = 115200
TIMEOUT = 1

FIFO_PATH = "/tmp/claude_led_daemon"

def connect_serial():
    """Attempt to establish a single serial connection.
    Returns the serial object on success, None on failure."""
    try:
        ser = serial.Serial(PORT, BAUD_RATE, timeout=TIMEOUT)
        time.sleep(1.5)  # Wait for Arduino to reset
        print("Serial connection established")
        return ser
    except Exception as e:
        print(f"Failed to open serial port: {e}")
        return None

def connect_with_retry(max_attempts=5):
    """Attempt to connect with exponential backoff.
    Wait times: 1, 2, 4, 8, 16 seconds for attempts 1..5.
    Returns the serial object on success, None on failure."""
    for attempt in range(max_attempts):
        ser = connect_serial()
        if ser is not None:
            return ser
        # If this was the last attempt, don't sleep
        if attempt == max_attempts - 1:
            break
        wait_time = 2 ** attempt  # 1, 2, 4, 8, ...
        print(f"Connection attempt {attempt+1} failed. Retrying in {wait_time}s...")
        time.sleep(wait_time)
    print(f"Failed to establish serial connection after {max_attempts} attempts.")
    return None

def main():
    print("=== Starting claude daemon ===")

    # Delete and recreate FIFO to ensure clean state
    if os.path.exists(FIFO_PATH):
        os.remove(FIFO_PATH)
    os.mkfifo(FIFO_PATH)

    print("Comparing claude settings")
    check_hooks.cmp_hooks()


    # Connection state tracking
    last_connection_check = 0
    connection_check_interval = 5.0  # Check connection every 5 seconds

    # Initial connection with retry
    ser = connect_with_retry()
    if ser is None:
        # This point should not be reached as connect_with_retry returns None on failure
        return

    try:
        while True:
            current_time = time.time()

            # Periodic connection check
            if current_time - last_connection_check >= connection_check_interval:
                last_connection_check = current_time
                if not ser or not ser.is_open:
                    print("Serial connection lost, attempting to reconnect...")
                    new_ser = connect_with_retry()
                    if new_ser is not None:
                        ser = new_ser
                        print("Reconnected successfully")
                    else:
                        # This point should not be reached as connect_with_retry returns None on failure
                        pass

            # Only process FIFO if we have a serial connection
            if ser and ser.is_open:
                try:
                    # Non-blocking read from FIFO with timeout
                    with open(FIFO_PATH, 'r', encoding='utf-8') as fifo:
                        # Use select-like behavior with timeout
                        if hasattr(select, 'select'):  # Unix/Linux
                            ready, _, _ = select.select([fifo], [], [], 0.1)
                            if ready:
                                for line in fifo:
                                    line = line.strip()
                                    if line:
                                        if line == "quitall":
                                            print("Exit command received")
                                            sys.exit(0)

                                        ser.write((line + '\n').encode())

                                        time.sleep(1)
                except (OSError, serial.SerialException) as e:
                    if hasattr(e, 'errno') and e.errno == 6:  # Device disconnected
                        print("Device disconnected")
                        if ser:
                            try:
                                ser.close()
                            except:
                                pass
                            ser = None
                    else:
                        print(f"Serial communication error: {e}")
                        if ser:
                            try:
                                ser.close()
                            except:
                                pass
                            ser = None
                except BlockingIOError:
                    # No data available, continue
                    pass
                except Exception as e:
                    print(f"Unexpected error reading FIFO: {e}")

            # Small sleep to prevent busy waiting
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("Received shutdown signal")
        if ser and ser.is_open:
            try:
                ser.write("waiting\n".encode())
            except:
                pass
    except Exception as e:
        print(f"Unexpected error in main loop: {e}")
    finally:
        # Cleanup
        if ser and ser.is_open:
            try:
                ser.close()
            except:
                pass
        if os.path.exists(FIFO_PATH):
            os.remove(FIFO_PATH)

if __name__ == "__main__":
    main()
