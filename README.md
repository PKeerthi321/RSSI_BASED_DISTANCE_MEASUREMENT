# 📡 RSSI-Based Distance Measurement Using ESP8266

An IoT-based distance estimation system that uses **Wi-Fi Received Signal Strength Indicator (RSSI)** to estimate the distance between an ESP8266 receiver and a Wi-Fi transmitter/access point.

The project explores how wireless signal strength varies with distance and uses the **log-distance path loss model** to convert RSSI values into an approximate physical distance.

## 🚀 Project Overview

Traditional distance measurement systems often require dedicated sensors such as ultrasonic, infrared, or LiDAR sensors.

This project investigates an alternative approach:

**Wi-Fi Signal → RSSI Measurement → Signal Processing → Distance Estimation**

The ESP8266 scans for a specified Wi-Fi network, measures its RSSI in dBm, averages multiple readings to reduce fluctuations, and estimates the transmitter-receiver distance.

The system can also:

* Display RSSI and estimated distance on a 16×2 I²C LCD
* Store measurements on an SD card
* Output measurement data through the Serial Monitor
* Experiment with multiple transmitters
* Perform RSSI field experiments for calibration

## 🧠 Working Principle

RSSI represents the received power of a wireless signal.

As the distance between the transmitter and receiver increases, the received signal strength generally decreases.

The project uses the log-distance path loss model:

```text
d = 10 ^ ((RSSI₀ - RSSI) / (10 × n))
```

Where:

| Parameter | Description                                      |
| --------- | ------------------------------------------------ |
| `d`       | Estimated distance in meters                     |
| `RSSI₀`   | RSSI measured at a reference distance of 1 meter |
| `RSSI`    | Measured RSSI value                              |
| `n`       | Path-loss exponent                               |

In the current implementation:

```text
RSSI₀ = -64 dBm
n = 2.0
```

These values are configurable and should ideally be calibrated for the actual environment.

## 🔄 System Workflow

```text
       Wi-Fi Transmitter
              │
              │ Wireless Signal
              ▼
       ┌───────────────┐
       │    ESP8266    │
       │   Receiver    │
       └───────┬───────┘
               │
               ▼
         Measure RSSI
               │
               ▼
       Average RSSI Samples
               │
               ▼
      Log-Distance Model
               │
               ▼
       Estimate Distance
          ┌────┼────┐
          │    │    │
          ▼    ▼    ▼
        LCD  Serial SD Card
```

## 🔧 Hardware Requirements

* ESP8266 development board
* ESP8266-based Wi-Fi transmitter/access point
* 16×2 I²C LCD
* MicroSD card module / SD interface
* MicroSD card
* Jumper wires
* USB cable
* Computer with Arduino IDE

## 💻 Software Requirements

* Arduino IDE
* ESP8266 Board Support Package
* ESP8266WiFi library
* Wire library
* LiquidCrystal_I2C library
* SD library

## 📁 Repository Structure

```text
RSSI_BASED_DISTANCE_MEASUREMENT/
│
├── AccessPoint_SetUp.ino
├── RSSI_to_Distance.ino
├── Rx_Code.ino
├── Rx_field_exp.ino
├── Rx_field_exps.ino
├── Rx_with_dist_multiple_txs.ino
└── SS-Collection.ino
```

### `AccessPoint_SetUp.ino`

Configures an ESP8266 as a Wi-Fi Soft Access Point.

The receiver can then scan for the configured SSID and use its RSSI as the basis for distance estimation.

### `RSSI_to_Distance.ino`

Main RSSI-to-distance implementation.

It:

1. Scans nearby Wi-Fi networks.
2. Searches for the target SSID.
3. Reads its RSSI.
4. Stores RSSI measurements in a five-sample circular buffer.
5. Calculates the average RSSI.
6. Estimates distance using the log-distance path loss model.
7. Displays the result on an LCD.
8. Prints the result through Serial.
9. Logs RSSI and distance to an SD card.

The current implementation performs a scan approximately every 5 seconds.

### `Rx_Code.ino`

Receiver-side Wi-Fi/RSSI experimentation code.

### `Rx_field_exp.ino`

Used for RSSI field experiments and collecting measurements under different physical conditions.

### `Rx_field_exps.ino`

Additional RSSI experimental implementation for testing signal behavior.

### `Rx_with_dist_multiple_txs.ino`

Experimental implementation for measuring RSSI/distance information from multiple Wi-Fi transmitters.

### `SS-Collection.ino`

Used for collecting signal-strength measurements for analysis and experimentation.

## ⚙️ Setup

### 1. Install ESP8266 Board Support

In Arduino IDE, install the ESP8266 board package.

Select the appropriate ESP8266 board from:

```text
Tools → Board
```

### 2. Configure the Transmitter

Upload:

```text
AccessPoint_SetUp.ino
```

to the ESP8266 that will act as the transmitter/access point.

Configure:

```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

**Do not commit real Wi-Fi credentials to GitHub.**

### 3. Configure the Receiver

Open:

```text
RSSI_to_Distance.ino
```

Set the target SSID:

```cpp
const char* targetSSID = "YOUR_SSID";
```

Adjust the RSSI calibration parameters:

```cpp
const float rssiAtOneMeter = -64.0;
const float pathLossExponent = 2.0;
```

### 4. Upload the Receiver Code

Connect the receiver ESP8266 and upload the program.

Open the Serial Monitor at:

```text
115200 baud
```

### 5. Observe the Results

The system reports information similar to:

```text
SSID: Tx1 | RSSI: -65 dBm | Distance: 1.12 m
```

The LCD displays:

```text
RSSI:-65dBm
Dist:1.1 m
```

Measurements are also written to:

```text
/rssi_log.txt
```

## 📊 RSSI and Distance

A simplified example:

```text
RSSI = -64 dBm
Reference RSSI = -64 dBm
Path-loss exponent = 2
```

Then:

```text
d = 10 ^ ((-64 - (-64)) / (10 × 2))
d = 10^0
d = 1 meter
```

As RSSI decreases, the estimated distance increases.

However, RSSI is **not a precise distance sensor**. Walls, furniture, human bodies, antenna orientation, interference, reflections, and other Wi-Fi devices can significantly affect the measurement.

## 🎯 Calibration

For better accuracy, measure RSSI at known distances.

For example:

```text
Distance       RSSI
--------------------------------
1 m            -64 dBm
2 m            -70 dBm
3 m            -74 dBm
4 m            -77 dBm
5 m            -80 dBm
```

These measurements can be used to determine suitable values for:

```text
RSSI₀
Path-loss exponent (n)
```

for the specific environment.

Calibration is important because the commonly used value:

```text
n = 2
```

is only an approximation. Indoor environments may have significantly different path-loss characteristics.

## 💾 Data Logging

The receiver can save measurements to an SD card.

Example:

```text
RSSI: -64 dBm, Distance: 1.00 m
RSSI: -66 dBm, Distance: 1.26 m
RSSI: -69 dBm, Distance: 1.78 m
RSSI: -72 dBm, Distance: 2.51 m
```

This data can later be imported into Python, MATLAB, Excel, or another analysis tool to study the relationship between RSSI and distance.

## 📈 Experimental Analysis

The collected data can be used to investigate:

* RSSI variation with distance
* Indoor propagation characteristics
* Effect of obstacles
* Effect of transmitter orientation
* RSSI stability
* Distance estimation error
* Path-loss exponent
* Performance of multiple transmitters

A useful next step is to plot:

```text
Distance vs RSSI
```

and compare the measured data against the theoretical log-distance model.

## 🌐 Potential Applications

RSSI-based distance estimation can be explored for:

* Indoor localization
* IoT device tracking
* Asset monitoring
* Robot positioning
* Smart warehouse systems
* Proximity detection
* Wireless sensor networks
* Device-to-device distance estimation
* Indoor navigation
* Multi-node localization systems

## ⚠️ Limitations

RSSI-based distance estimation has several limitations.

### Environmental sensitivity

RSSI can change even when the physical distance remains constant because of:

* Walls
* Furniture
* People
* Multipath propagation
* Interference
* Antenna orientation
* Wi-Fi congestion

### Accuracy

The calculated distance should be treated as an **estimate**, not an exact measurement.

### Calibration dependency

The parameters used in the distance equation need to be calibrated for the specific transmitter, receiver, antenna configuration, and environment.

### Initialization

The current implementation averages five RSSI samples. The buffer should ideally be initialized properly before using the average, otherwise startup readings can affect the first estimate.

## 🔮 Future Improvements

Possible improvements include:

* [ ] Dynamic RSSI calibration
* [ ] Median filtering for noisy RSSI measurements
* [ ] Moving-average / Kalman filtering
* [ ] Automatic path-loss exponent estimation
* [ ] Improved SD-card data format such as CSV
* [ ] Real-time RSSI vs distance graphs
* [ ] Web dashboard for monitoring measurements
* [ ] Multiple access-point triangulation
* [ ] Indoor localization using multiple transmitters
* [ ] Error calculation against actual measured distance
* [ ] Machine-learning-based RSSI-to-distance estimation

## 📚 Technologies Used

* **ESP8266**
* **Wi-Fi / IEEE 802.11**
* **RSSI**
* **Log-Distance Path Loss Model**
* **Arduino C/C++**
* **I²C LCD**
* **SD Card Storage**
* **Embedded Systems**
* **IoT**

## 👨‍💻 Author

**P. Keerthi Reddy**

GitHub: [PKeerthi321](https://github.com/PKeerthi321)

## 📄 License

This project is intended for educational, experimental, and research purposes.

If you reuse or modify the project, please provide appropriate attribution.
