# Industrial CAN Network with Dual MCP2515

A practical automotive/industrial CAN bus learning project built using an **Arduino Mega 2560** and **two MCP2515 CAN controller modules**.

The project is being developed step-by-step to build practical knowledge of CAN communication, embedded systems, diagnostics, testing, and automotive communication concepts.

---

## Project Status

### Current Status: TWO-NODE CAN COMMUNICATION WORKING

The current hardware setup successfully demonstrates:

- Two MCP2515 CAN controllers connected to one Arduino Mega 2560
- Independent SPI chip-select control
- CANH/CANL bus connection between both nodes
- 500 kbps CAN communication
- 8 MHz MCP2515 crystal configuration
- Node 1 → Node 2 message transmission
- Node 2 → Node 1 message transmission
- CAN ID verification
- DLC verification
- Data-byte verification
- Successful two-way CAN communication

Example test result:

```text
========================================
 INDUSTRIAL CAN - TWO NODE TEST
========================================

[NODE 1] Initializing MCP2515...
[NODE 1] CS  = 53
[NODE 1] INT = 2
[NODE 1] READY

[NODE 2] Initializing MCP2515...
[NODE 2] CS  = 49
[NODE 2] INT = 3
[NODE 2] READY

========================================
 TEST 1: NODE 1 -> NODE 2
========================================

[NODE 1] Sending ID 0x100...
[NODE 1] TX SUCCESS
[NODE 2] RX | ID: 0x100 | DLC: 8 | DATA: 11 22 33 44 55 66 77 88
[NODE 2] MESSAGE RECEIVED CORRECTLY

========================================
 TEST 2: NODE 2 -> NODE 1
========================================

[NODE 2] Sending ID 0x200...
[NODE 2] TX SUCCESS
[NODE 1] RX | ID: 0x200 | DLC: 8 | DATA: AA BB CC DD EE FF 12 34
[NODE 1] MESSAGE RECEIVED CORRECTLY

========================================
 TWO-NODE CAN TEST COMPLETE
========================================
```

---

## Project Overview

The current system contains two CAN nodes using two MCP2515 controllers connected to an Arduino Mega 2560.

```text
                         CAN BUS

              CANH ========================== CANH
                |                              |
              CANL ========================== CANL
                |                              |
        +-------+--------+             +-------+--------+
        |    NODE 1      |             |    NODE 2      |
        |                |             |                |
        |   MCP2515 #1   |             |   MCP2515 #2   |
        |   CS = 53      |             |   CS = 49      |
        |   INT = 2      |             |   INT = 3      |
        +-------+--------+             +-------+--------+
                |                              |
                +---------------+--------------+
                                |
                        Arduino Mega 2560
```

Both MCP2515 modules share the Arduino Mega SPI bus.

The modules are selected independently using separate chip-select pins.

---

## Hardware

### Main Controller

- Arduino Mega 2560

### CAN Controllers

- MCP2515 CAN controller module #1
- MCP2515 CAN controller module #2

### MCP2515 Crystal

- 8 MHz

---

## MCP2515 Configuration

| Parameter | MCP2515 #1 | MCP2515 #2 |
|---|---:|---:|
| CS Pin | 53 | 49 |
| INT Pin | 2 | 3 |
| CAN Speed | 500 kbps | 500 kbps |
| Crystal | 8 MHz | 8 MHz |

Both MCP2515 modules share the Arduino Mega SPI bus.

The chip-select pins are separate so the Arduino can communicate with each MCP2515 independently.

---

## Arduino Mega SPI

The Arduino Mega 2560 hardware SPI pins are:

| SPI Signal | Arduino Mega Pin |
|---|---:|
| MOSI | 51 |
| MISO | 50 |
| SCK | 52 |
| SS / SPI control | 53 |

The MCP2515 modules share:

- MOSI
- MISO
- SCK

Each module has its own CS pin:

```text
MCP2515 #1 CS -> Arduino Mega pin 53
MCP2515 #2 CS -> Arduino Mega pin 49
```

---

## CAN Bus Connections

The two MCP2515 modules are connected to the same CAN bus.

```text
MCP2515 #1                 MCP2515 #2

CANH  -------------------- CANH
CANL  -------------------- CANL
```

CANH is connected to CANH.

CANL is connected to CANL.

A common ground/reference should also be maintained between connected hardware.

CAN bus termination should be provided at the physical ends of the bus as required by the network topology.

---

## Interrupt Connections

The MCP2515 interrupt outputs are connected as follows:

```text
MCP2515 #1 INT -> Arduino Mega pin 2
MCP2515 #2 INT -> Arduino Mega pin 3
```

Current software configuration:

```cpp
#define MCP1_CS 53
#define MCP2_CS 49
```

Current interrupt assignments:

```text
MCP2515 #1 -> INT 2
MCP2515 #2 -> INT 3
```

---

## CAN Configuration

The current test uses:

```text
CAN Speed:       500 kbps
MCP2515 Crystal: 8 MHz
CAN Mode:       Normal Mode
```

Both MCP2515 controllers are configured with the same CAN bitrate and crystal configuration.

---

## CAN Test 1 — Node 1 to Node 2

Node 1 transmits:

```text
CAN ID: 0x100
DLC:    8
DATA:   11 22 33 44 55 66 77 88
```

Node 2 successfully receives:

```text
ID:   0x100
DLC:  8
DATA: 11 22 33 44 55 66 77 88
```

Result:

```text
[NODE 1] TX SUCCESS
[NODE 2] MESSAGE RECEIVED CORRECTLY
```

---

## CAN Test 2 — Node 2 to Node 1

Node 2 transmits:

```text
CAN ID: 0x200
DLC:    8
DATA:   AA BB CC DD EE FF 12 34
```

Node 1 successfully receives:

```text
ID:   0x200
DLC:  8
DATA: AA BB CC DD EE FF 12 34
```

Result:

```text
[NODE 2] TX SUCCESS
[NODE 1] MESSAGE RECEIVED CORRECTLY
```

---

## CAN Frame Used in Testing

The current test uses standard CAN identifiers with 8-byte data payloads.

Example:

```text
ID       DLC       DATA
0x100     8        11 22 33 44 55 66 77 88
```

Where:

- **ID** = CAN identifier
- **DLC** = Data Length Code
- **DATA** = CAN payload

Second test:

```text
ID       DLC       DATA
0x200     8        AA BB CC DD EE FF 12 34
```

---

## Loopback Test

An earlier hardware verification stage used MCP2515 loopback mode.

Loopback mode allowed the MCP2515 controller to verify CAN frame transmission and reception internally without requiring normal communication between two external CAN nodes.

After the controller-level test passed, the project was moved to **Normal Mode** for actual two-node CAN communication.

The current communication test uses:

```cpp
setNormalMode()
```

---

## Software

The project currently uses:

- Arduino IDE
- Embedded C/C++
- SPI
- MCP2515 CAN controller
- MCP2515 Arduino library

Example library include:

```cpp
#include <SPI.h>
#include <mcp2515.h>
```

---

## Current Pin Configuration

```cpp
#define MCP1_CS 53
#define MCP2_CS 49
```

Interrupt assignments:

```text
MCP2515 #1 -> INT 2
MCP2515 #2 -> INT 3
```

SPI:

```text
MOSI -> 51
MISO -> 50
SCK  -> 52
```

---

## Project Structure

```text
Industrial_CAN_Network/
│
├── README.md
│
├── docs/
│   └── CAN documentation and project notes
│
├── ecu/
│   └── ECU_01_Control/
│       └── MCP2515_Loopback_Test/
│           └── MCP2515_Dual_Controller_Test.ino
│
└── tests/
    └── CAN communication tests
```

---

## CAN Communication Architecture

The basic communication architecture is:

```text
                 Arduino Mega 2560
                         |
              +----------+----------+
              |                     |
             SPI                   SPI
              |                     |
       +------+-------+      +------+-------+
       | MCP2515 #1   |      | MCP2515 #2   |
       | CS = 53      |      | CS = 49      |
       | INT = 2      |      | INT = 3      |
       +------+-------+      +------+-------+
              |                     |
            CANH                  CANH
              |                     |
              +-------- CAN BUS ----+
              |                     |
            CANL                  CANL
```

The two MCP2515 controllers share the same SPI bus but have independent CS lines.

---

## CAN Identifiers

The current test uses:

```text
Node 1 -> Node 2
CAN ID = 0x100

Node 2 -> Node 1
CAN ID = 0x200
```

These identifiers are used to distinguish the two test messages.

---

## Development Roadmap

The project will be expanded progressively.

### CAN Fundamentals

- CAN frame structure
- Standard and extended CAN identifiers
- CAN arbitration
- CAN message priority
- CAN bus states
- ACK mechanism
- CAN bit timing
- CAN error handling

### CAN Filtering

- Acceptance filters
- Acceptance masks
- Message filtering
- Interrupt-based reception

### ECU Development

- ECU node architecture
- Periodic CAN messages
- Event-driven messages
- Signal encoding and decoding
- Sensor data simulation
- Actuator control messages

### Diagnostics

- Diagnostic CAN communication
- UDS concepts
- Diagnostic request/response
- Service IDs
- Positive and negative responses
- Diagnostic session concepts
- DTC concepts
- ISO-TP concepts

### Advanced CAN

- CAN FD concepts
- CAN FD frame structure
- Higher data rates
- CAN bus load analysis

### Testing

- Automated CAN testing
- Python CAN tools
- Test scripts
- Message validation
- Error injection
- Regression testing
- Hardware-in-the-loop concepts

### Automotive Tools

- Vector CANalyzer concepts
- Vector CANoe concepts
- CAN trace analysis
- CAN database concepts
- DBC files

### Automotive Software

- ECU software architecture
- AUTOSAR awareness
- Functional Safety awareness
- ISO 26262 concepts
- ASPICE awareness

---

## Future Hardware Architecture

The project may later be expanded into a multi-ECU CAN network.

```text
                       CAN BUS
====================================================

       |                 |                 |
       |                 |                 |
   +-------+         +-------+         +-------+
   | ECU 1 |         | ECU 2 |         | ECU 3 |
   +-------+         +-------+         +-------+
       |                 |                 |
   MCP2515           MCP2515            ESP32
```

Possible ECU roles:

- Control ECU
- Sensor ECU
- Motor controller
- Gateway ECU
- Diagnostic ECU

---

## Learning Objectives

The project is intended to provide practical experience with:

- CAN bus communication
- Embedded C/C++
- Arduino Mega 2560
- MCP2515
- SPI communication
- CAN transceiver interfaces
- Multi-node CAN communication
- CAN message structure
- CAN identifiers
- CAN arbitration
- CAN filtering
- CAN diagnostics
- UDS concepts
- ISO-TP concepts
- CAN FD concepts
- Embedded testing
- CAN debugging
- Python-based automation
- HIL testing concepts
- CANoe/CANalyzer concepts
- Automotive ECU concepts
- AUTOSAR awareness
- Functional Safety awareness
- ASPICE awareness

---

## Repository Goals

The long-term goal is to develop a realistic embedded CAN communication and diagnostics platform that demonstrates practical skills relevant to automotive and industrial embedded systems engineering.

The project is being developed incrementally, with each stage tested on real hardware before moving to the next stage.

---

## Current Status Summary

```text
HARDWARE
Arduino Mega 2560              ✓
MCP2515 #1                     ✓
MCP2515 #2                     ✓
CANH / CANL connection         ✓

SPI
Shared SPI bus                 ✓
MCP2515 #1 CS = 53            ✓
MCP2515 #2 CS = 49            ✓

INTERRUPTS
MCP2515 #1 INT = 2            ✓
MCP2515 #2 INT = 3            ✓

CAN
500 kbps                       ✓
8 MHz crystal                  ✓
Normal mode                    ✓

COMMUNICATION
Node 1 -> Node 2               ✓
Node 2 -> Node 1               ✓
CAN ID verification            ✓
DLC verification               ✓
Payload verification            ✓
Two-way CAN communication      ✓

NEXT DEVELOPMENT STAGE
CAN filtering
Interrupt-driven reception
Periodic CAN messages
Signal encoding/decoding
```

---

## Author

**Varun Bhandekar**

GitHub:

```text
https://github.com/IMBVK
```

---

## License

This project is intended for educational, experimental, and portfolio development purposes.