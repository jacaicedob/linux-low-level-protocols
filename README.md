# linux-low-level-protocols

A learning project for gaining hands-on, protocol-level experience with I2C (and eventually SPI) on Linux. The goal is to build the kind of skills listed in embedded/firmware job postings — register-level hardware programming, datasheet-driven development, and real hardware debugging — by writing everything from scratch in C, using only Linux's raw device interfaces.

**Design philosophy:** no Python, no Adafruit-style sensor libraries. All hardware access goes through `/dev/i2c-1`, `ioctl()`, `open()`, `read()`, and `write()` syscalls. Register bytes are decoded manually, per the device datasheet.

**Target hardware:** Raspberry Pi 3 Model B+ (BCM2837B0), Raspberry Pi OS Lite (64-bit), with peripherals from a SunFounder Raphael Kit — currently an MPU6050 IMU at I2C address `0x68`.

**Sibling project:** [`raspberrypi-baremetal`](https://github.com/jacaicedob/raspberrypi-baremetal) covers the same embedded learning goals but without an OS — direct ARM register access, custom linker, and boot sequence. This repo is specifically about the Linux userspace/`ioctl` layer.

---

## Repository structure

```
.
├── protocols/
│   └── i2c/
│       ├── i2c-driver.h       # i2c_device struct, function declarations
│       └── i2c-driver.c       # open/ioctl/read/write wrappers
├── drivers/
│   └── mpu6050/
│       ├── mpu6050.h          # register map, mpu6050 struct, function declarations
│       └── mpu6050.c          # init, wakeup, and sensor read/decode logic
├── apps/
│   └── i2c_mpu6050_read.c     # main(): reads accel + gyro and prints results
├── docs/
│   └── MPU-6000.PDF           # vendored MPU-6000/6050 register map (rev 4.2)
├── container_config/
│   └── toolchain-aarch64.cmake  # CMake cross-compilation toolchain
├── scripts/
│   ├── containerBuild.sh      # builds the Podman cross-compilation image
│   └── build.sh               # runs cmake + make inside the container
├── Containerfile              # Debian Trixie + aarch64 cross-toolchain + cmake
└── CMakeLists.txt
```

The three conceptual layers:

- **`protocols/`** — bus-transport code that doesn't know about any specific sensor. The I2C driver opens a device node, configures the slave address via `ioctl(I2C_SLAVE)`, and provides `i2c_read()`/`i2c_write()` wrappers that prepend the register offset as the first byte.
- **`drivers/`** — per-device logic. The MPU6050 driver sits on top of the I2C protocol layer and handles chip-specific concerns: wakeup sequence, register addresses, raw byte reconstruction, and sensitivity scaling.
- **`apps/`** — one `main()`-containing executable per exercise, linking the driver and protocol together.

Headers live next to their `.c` files; there are no separate `include/` subdirectories.

---

## What's implemented

### I2C protocol layer (`protocols/i2c/`)

| Function | What it does |
|---|---|
| `open_i2c_dev()` | Opens `/dev/i2c-{id}` and stores the file descriptor |
| `config_i2c_slave()` | Calls `ioctl(fd, I2C_SLAVE, addr)` to select the target device |
| `i2c_write()` | Sends register offset + payload bytes via `write()` |
| `i2c_read()` | Writes the register offset, then reads N bytes via `read()` |

### MPU6050 driver (`drivers/mpu6050/`)

| Function | What it does |
|---|---|
| `mpu6050_open()` | Opens the I2C device and sets the slave address to `0x68` |
| `mpu6050_reset()` | Writes `DEVICE_RESET` (0x80) to `PWR_MGMT_1` (0x6B) and waits 100 ms |
| `mpu6050_wakeup()` | Clears the `SLEEP` bit in `PWR_MGMT_1` and `PWR_MGMT_2` (0x6C) |
| `mpu6050_read_id()` | Reads `WHO_AM_I` (0x75) and prints the device ID |
| `mpu6050_read_pwr_mgmt_config()` | Reads both power management registers |
| `mpu6050_read_accel_config()` | Reads `ACCEL_CONFIG` (0x1C) |
| `mpu6050_read_gyro_config()` | Reads `GYRO_CONFIG` (0x1B) |
| `mpu6050_read_temp()` | Reads `TEMP_OUT_H/L`, reconstructs a signed 16-bit value, applies the datasheet formula: `temp = raw / 340.0 + 36.53` |
| `mpu6050_read_accel()` | Reads an MSB/LSB register pair, reconstructs a signed 16-bit value, reads `AFS_SEL` from `ACCEL_CONFIG` bits 4:3, and divides by the corresponding LSB sensitivity (16384, 8192, 4096, or 2048 LSB/g) |
| `mpu6050_read_gyro()` | Same pattern as accel, using `FS_SEL` from `GYRO_CONFIG` and gyro sensitivities (131, 65.5, 32.8, or 16.4 LSB/°/s) |

### App (`apps/i2c_mpu6050_read.c`)

Wakes the MPU6050, verifies its identity via `WHO_AM_I`, reads temperature, then reads and prints X/Y/Z accelerometer and gyroscope values in physical units (g and °/s).

---

## Build

Cross-compilation runs inside a Podman container (Debian Trixie + `crossbuild-essential-arm64` + `cmake` + `gdb-multiarch`). Binaries are built for `aarch64-linux-gnu` on the host and copied to the Pi to run.

### 1. Build the container image (one-time)

```bash
./scripts/containerBuild.sh
```

This tags the image as `linux-low-level-protocols:latest`.

### 2. Cross-compile

```bash
./scripts/build.sh
```

This runs the container, mounts the repo at `/workspace`, and executes:

```bash
mkdir -p build-arm64 && cd build-arm64 \
  && cmake -DCMAKE_TOOLCHAIN_FILE=/workspace/container_config/toolchain-aarch64.cmake .. \
  && make
```

Output binaries land in `build-arm64/apps/`.

### 3. Deploy and run on the Pi

```bash
scp build-arm64/apps/i2c_mpu6050_read pi@<pi-ip>:~/
ssh pi@<pi-ip> ./i2c_mpu6050_read
```

Expected output (values will vary):

```
Successfully opened I2C device
Successfully set the device at addres 0x68 to slave
PWR_MGMT_1: 0x40
PWR_MGMT_2: 0x0
PWR_MGMT_1: 0x0
PWR_MGMT_2: 0x0
Device ID: 0x68
Temperature: 28.341176 C
Accelerometer data:
    x: 0.012345
    y: -0.003210
    z: 1.001234
Gyroscope data:
    x: 0.312977
    y: -0.122137
    z: 0.091603
```

---

## Not yet built

- **LCD1602 display driver** — PCF8574 I2C-to-GPIO expander driving an HD44780 controller in 4-bit mode. Deprioritized in favor of moving to a separate bare-metal project, since it doesn't add much new protocol-level learning beyond what the MPU6050 work already covers.
- **SPI work** — potential future exercise using the kit's MFRC522 RFID module once ready to explore a second protocol.

---

## Datasheets

- **MPU-6000/MPU-6050 Register Map and Descriptions** (rev 4.2) — register addresses, bit fields, and sensitivity tables used throughout `mpu6050.c`/`mpu6050.h`. Vendored in this repo at [`docs/MPU-6000.PDF`](docs/MPU-6000.PDF); also available [from InvenSense/TDK](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf).
- [BCM2835 ARM Peripherals](https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf) — I2C controller registers and GPIO/bus concepts for the Pi's SoC (BCM2837B0 is register-compatible for peripherals)
