# Realtime Firmware

This repository contains the firmware running on the **ESP32**, responsible for real-time data acquisition, processing, and communication.

## Overview

The firmware handles multiple sensor inputs and processes them in real time, while also providing network connectivity and task management.

## Features

- **Sensor Data Acquisition**
  - Heart rate
  - SpO₂ (blood oxygen)
  - Temperature
  - Motion
  - Microphone input

- **Real-Time Processing**
  - Threshold detection
  - Signal filtering

- **Networking**
  - WiFi connectivity
  - Embedded HTTP server

- **Task Management**
  - Scheduling and concurrency using FreeRTOS (if enabled)

## Architecture

The system is designed to:
- Continuously collect sensor data  
- Process and filter signals in real time  
- Serve data over a network interface  
- Efficiently manage tasks on the ESP32  

## Hardware

- ESP32 microcontroller  
- Supported sensors:
  - MAX30102 - Heart rate / SpO₂ sensor  
  - MAX30205 - Body Temperature sensor  
  - MPU-6050 - Motion sensor (e.g., accelerometer)  
  - MAX9814 - Microphone
  - MQ135 - Air Quality sensor  
