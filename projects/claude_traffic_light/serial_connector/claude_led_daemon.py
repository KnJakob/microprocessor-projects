import serial, time, os, sys
import select
from collections import OrderedDict
from claude_settings import check_hooks

PORT = "/dev/tty.usbserial-1130" # mac
# PORT = "/dev/ttyUSB0"               # linux
BAUD_RATE = 115200
TIMEOUT = 1

FIFO_PATH = "/tmp/claude_led_daemon"
CACHE_TTL = 1.0  # Cache time-to-live in seconds

# Global serial connection object
ser = None

class ToneCache:
    def __init__(self, ttl_seconds):
        self.cache = OrderedDict()
        self.ttl = ttl_seconds
        self.access_times = {}

    def get(self, key):
        now = time.time()
        if key in self.cache:
            # Check if entry is still valid
            if now - self.access_times[key] < self.ttl:
                # Update access time for LRU
                self.access_times[key] = now
                self.cache.move_to_end(key)
                return self.cache[key]
            else:
                # Remove expired entry
                del self.cache[key]
                del self.access_times[key]
        return None

    def put(self, key, value):
        now = time.time()
        self.cache[key] = value
        self.access_times[key] = now
        self.cache.move_to_end(key)

        # Remove oldest entries if cache gets too large (optional)
        if len(self.cache) > 100:  # Limit cache size
            oldest = next(iter(self.cache))
            del self.cache[oldest]
            del self.access_times[oldest]

def connect_serial():
    """Attempt to establish a single serial connection.
    Returns True on success, False on failure."""
    global ser
    try:
        if ser and ser.is_open:
            ser.close()
        ser = serial.Serial(PORT, BAUD_RATE, timeout=TIMEOUT)
        time.sleep(1.5)  # Wait for Arduino to reset
        print("Serial connection established")
        return True
    except Exception as e:
        print(f"Failed to open serial port: {e}")
        ser = None
        return False

def connect_with_retry(max_attempts=5):
    """Attempt to connect with exponential backoff.
    Wait times: 1, 2, 4, 8, 16 seconds for attempts 1..5.
    Exits the program if max_attempts is reached."""
    global ser
    for attempt in range(max_attempts):
        if connect_serial():
            return True
        # If this was the last attempt, don't sleep
        if attempt == max_attempts - 1:
            break
        wait_time = 2 ** attempt  # 1, 2, 4, 8, ...
        print(f"Connection attempt {attempt+1} failed. Retrying in {wait_time}s...")
        time.sleep(wait_time)
    print(f"Failed to establish serial connection after {max_attempts} attempts. Exiting.")
    sys.exit(1)
    return False  # Should not reach here

def main():
    print("=== Starting claude daemon ===")

    # Delete and recreate FIFO to ensure clean state
    if os.path.exists(FIFO_PATH):
        os.remove(FIFO_PATH)
    os.mkfifo(FIFO_PATH)

    print("Comparing claude settings")
    check_hooks.cmp_hooks()

    # Initialize tone cache
    tone_cache = ToneCache(CACHE_TTL)

    # Connection state tracking
    global ser
    ser = None
    last_connection_check = 0
    connection_check_interval = 5.0  # Check connection every 5 seconds

    # Initial connection with retry
    if not connect_with_retry():
        # This point should not be reached due to exit in connect_with_retry
        pass

    try:
        while True:
            current_time = time.time()

            # Periodic connection check
            if current_time - last_connection_check >= connection_check_interval:
                last_connection_check = current_time
                if not ser or not ser.is_open:
                    print("Serial connection lost, attempting to reconnect...")
                    if connect_with_retry():
                        print("Reconnected successfully")
                    else:
                        # This point should not be reached due to exit in connect_with_retry
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
                                        # Check cache first
                                        cached_result = tone_cache.get(line)
                                        if cached_result is not None:
                                            ser.write((cached_result + '\n').encode())
                                        else:
                                            # Not in cache, send and cache
                                            ser.write((line + '\n').encode())
                                            tone_cache.put(line, line)

                                        if line == "exit":
                                            print("Exit command received")
                                            ser.write("waiting\n".encode())
                                            sys.exit(0)
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
