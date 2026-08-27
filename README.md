# Industrial CAN Network & Diagnostics Platform

An industry-oriented embedded Controller Area Network (CAN) communication and diagnostics platform developed using Arduino, MCP2515 CAN controllers, TJA1050 CAN transceivers, and Vector CANoe/CANalyzer concepts.

The project is being developed step-by-step from CAN fundamentals toward a practical multi-node communication, diagnostics, monitoring, and automated testing platform.

---

## Project Objective

The objective of this project is to build and document a practical CAN-based embedded communication system while developing the skills commonly used in automotive and industrial embedded systems.

The project focuses on:

- Classical CAN 2.0B
- CAN frame structure
- CAN arbitration
- CAN identifiers
- CAN message transmission and reception
- MCP2515 CAN controller
- TJA1050 CAN transceiver
- SPI communication
- Multi-node CAN communication
- Message filtering
- Interrupt-driven reception
- Periodic message scheduling
- Heartbeat monitoring
- Fault detection and handling
- Diagnostics
- CAN Transport Protocol (ISO-TP)
- UDS concepts
- DBC-based signal definitions
- CANoe/CANalyzer analysis
- CAPL-based testing concepts
- Automated CAN testing
- Python-based test automation

---

# Hardware

## Currently Available

- Arduino Mega 2560
- 3 × MCP2515 CAN controller modules
- TJA1050 CAN transceiver interfaces integrated on the MCP2515 modules
- Logic analyzer
- Breadboard
- Jumper wires

## MCP2515 Configuration

The MCP2515 modules used in this project have an 8 MHz oscillator.

Initial CAN configuration:

```text
CAN Bitrate: 500 kbit/s
MCP2515 Oscillator: 8 MHz