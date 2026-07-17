import serial, time, os, sys
import select
from collections import OrderedDict

PORT = "/dev/tty.usbserial-1130" # mac
# PORT = "/dev/ttyUSB0"               # linux
BAUD_RATE = 115200
TIMEOUT = 1

FIFO_PATH = "/tmp/claude_led_daemon"
CACHE_TTL = 1.0  # Cache time-to-live in seconds

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

def main():
    # Delete and recreate FIFO to ensure clean state
    if os.path.exists(FIFO_PATH):
        os.remove(FIFO_PATH)
    os.mkfifo(FIFO_PATH)

    # Initialize tone cache
    tone_cache = ToneCache(CACHE_TTL)

    # Connection state tracking
    ser = None
    last_connection_check = 0
    connection_check_interval = 5.0  # Check connection every 5 seconds

    def connect_serial():
        nonlocal ser
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

    # Initial connection
    if not connect_serial():
        print("Warning: Could not establish initial serial connection")

    try:
        while True:
            current_time = time.time()

            # Periodic connection check
            if current_time - last_connection_check >= connection_check_interval:
                last_connection_check = current_time
                if not ser or not ser.is_open:
                    print("Serial connection lost, attempting to reconnect...")
                    if connect_serial():
                        print("Reconnected successfully")
                    else:
                        print("Reconnection failed, will retry later")

            # Only process FIFO if we have a serial connection
            if ser and ser.is_open:
                try:
                    # Non-blocking read from FIFO with timeout
                    with open(FIFO_PATH, 'r', encoding='utf-8') as fifo:
                        # Use select-like behavior with timeout
                        import select
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
