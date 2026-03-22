# Automatic Parking Truck

Experimental project for an **autonomous truck parking system** using a hybrid **Arduino + Python architecture**.

## Description

This project aims to develop a system capable of assisting or automating truck parking using distance and orientation sensors.

The architecture is divided into:

*  **Arduino UNO** → Sensor reading and communication
*  **Arduino Mega 2560** → Motor control + IMU reading
*  **Python (WebUI)** → Data processing and real-time visualization

Communication between components is handled via:

* UART (between Arduinos)
* Bridge (Arduino ↔ Python)
* WebSockets (Python ↔ Web interface)

---

##  Main Components

### 🔹 Sensors

* Ultrasonic sensors (multiple directions)
* ToF sensor (rear distance)
* IMU (vehicle orientation)

### 🔹 Control

* Motor control system on Arduino Mega
* Command sending from web interface

---

##  Architecture

```text
[ Web UI ]
     ↑ ↓ (WebSocket)
[ Python Backend ]
     ↑ ↓ (Bridge)
[ Arduino UNO ]
     ↑ ↓ (UART)
[ Arduino Mega ]
     ↓
[ Motors + IMU ]
```

---

## Web Interface

The system is controlled through a **web server** with a frontend built using:

* HTML
* CSS
* JavaScript

This interface allows:

* Real-time visualization of sensor data
* Sending control commands to the truck
* Monitoring system status

It also includes a simple visualization system that shows:

* The orientation of the truck tractor
* How the trailer follows the movement
---

## Project Structure

* `CamionBaseV0_1.ino` → Main Arduino UNO code
* `MotoresClass.ino` → Motor control on Arduino Mega
* `main.py` → Backend + Web UI
* `truck.cpp / truck.h` → Custom library (sensor handling & compatibility fixes)

---

## Current Features

* Multi-sensor distance reading
* ToF sensor integration
* IMU data parsing and processing
* Real-time visualization via WebSockets
* Remote motor control from web interface

---

## Technologies Used

* Arduino (C++)
* Python
* WebSockets
* UART
* I2C

---

## Project Status

🧪 Early development stage.

Planned improvements:

* Autonomous parking algorithms
* Trajectory control optimization
* AI / advanced decision-making integration

---

## Author

* Aitor Lazarraga

---
