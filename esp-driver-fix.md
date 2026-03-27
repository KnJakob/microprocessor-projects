# ESP32 Upload Error on Linux (Arch) — Summary

## Problem

While trying to upload code to an ESP32 using the Arduino IDE, the following error appeared:

```
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 4 of 10: not in sync: resp=0x00
```

Additionally:

* Only `/dev/tty4` was selectable (incorrect device)
* The ESP32 was detected via USB (`lsusb` showed `1a86:7523`)
* No `/dev/ttyUSB0` or `/dev/ttyACM0` device was created
* `modprobe ch341` failed with:

  ```
  FATAL: Module ch341 not found
  ```

## Root Cause

The system detected the USB-to-serial chip (CH340), but the required kernel driver (`ch341`) was missing.

As a result:

* No serial device (`/dev/ttyUSB0`) was created
* Arduino IDE could not communicate with the ESP32

## Fixes

### 1. Install Kernel Modules / Headers

```bash
sudo pacman -S linux-headers
sudo pacman -Syu
reboot
```

---

### 2. Load the Driver

```bash
sudo modprobe ch341
```

---

### 3. Verify Device Creation

After reconnecting the ESP32:

```bash
dmesg | tail -20
```

Expected output:

```
ch341-uart converter now attached to ttyUSB0
```

Check:

```bash
ls /dev/ttyUSB*
```

---

### 4. Fix Permissions

```bash
sudo usermod -aG dialout $USER
```

Then log out and back in.

---

### 5. Alternative (if driver still missing)

Install manually via DKMS:

```bash
sudo pacman -S dkms git base-devel
git clone https://github.com/juliagoda/CH341SER.git
cd CH341SER
make
sudo make install
```

---

## Result

After applying the fix:

* `/dev/ttyUSB0` becomes available
* ESP32 can be selected in Arduino IDE
* Upload works correctly

