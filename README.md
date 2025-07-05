# Design and Implementation of Encryption-Supported Vehicle Keyless Entry Systems

---

This repository contains the source code, hardware designs, and 3D enclosure models for the graduation project "Design and Implementation of Encryption-Supported Vehicle Keyless Entry Systems," submitted to Izmir Democracy University, Faculty of Engineering, Department of Electrical and Electronics Engineering in 2025.

**Authors:** Ata GÜNEŞ and Esra CESUR

## Project Overview

This thesis addresses the security vulnerabilities in conventional vehicle keyless entry systems by developing and implementing two distinct, encryption-supported solutions:

1.  **RF/RFID Hybrid System:** Enhances traditional 433 MHz Radio Frequency (RF) technology with Radio Frequency Identification (RFID) for proximity-based ignition authorization and integrates Advanced Encryption Standard (AES) for secure communication.
2.  **Bluetooth Low Energy (BLE) System:** Utilizes modern BLE technology for secure, smartphone-based vehicle access control via a custom mobile application and a dedicated key fob, also secured with AES and Hopping Code algorithms.

Both systems were developed as low-cost, fully functional prototypes, demonstrating significant advancements over standard keyless entry systems in terms of security and usability.

## Repository Contents

This repository is organized as follows:

* **`3D_Enclosure_Designs/`**: Contains the 3D models for the custom key fob enclosures, designed in Tinkercad. These files are ready for 3D printing.
* **`MIT_AES_Combined_Button.ino`**: Arduino source code for the BLE system's key fob transmitter, including AES encryption and button-based control.
* **`MIT_AES_Combined_Servo.ino`**: Arduino source code for the BLE system's vehicle receiver, handling BLE communication, AES decryption, and servo motor control.
* **`RF-RFID_System_final.fzz`**: Fritzing design file for the physical layout of the RF/RFID system prototype. This visualizes the circuit assembly.
* **`RX_RF_RFID_AES_combined.ino`**: Arduino source code for the RF/RFID system's vehicle receiver, managing RF/RFID communication, AES decryption, and servo motor control.
* **`Smart Car Control.apk`**: The Android application package (APK) for the custom mobile application used with the BLE system.
* **`TX_AES.ino`**: Arduino source code for the RF/RFID system's key fob transmitter, incorporating AES encryption.

## Key Features

* **Enhanced Security:** Implementation of AES encryption and Hopping Code algorithms to prevent replay attacks and ensure secure communication.
* **Dual System Design:** Two distinct prototypes (RF/RFID and BLE) offer versatile solutions for keyless entry.
* **Proximity-Based Ignition (RF/RFID):** Utilizes RFID for an additional layer of security, requiring proximity for vehicle ignition.
* **Smartphone Integration (BLE):** A custom Android mobile application for convenient vehicle control via Bluetooth.
* **Physical Prototypes:** Includes designs for 3D-printed enclosures for practical and portable key fobs.

