# Industrial CAN Network with Dual MCP2515

A practical automotive/industrial CAN bus learning project built using an Arduino Mega 2560 and two MCP2515 CAN controller modules.

The project is being developed step-by-step to build practical knowledge of:

- CAN bus fundamentals
- CAN message transmission and reception
- Multi-node CAN communication
- CAN arbitration
- CAN identifiers
- CAN filtering
- CAN error handling
- CAN diagnostics / UDS concepts
- CAN FD concepts
- Embedded C/C++
- SPI communication
- CAN debugging and testing
- Python-based CAN automation
- HIL/test concepts
- Vector CANoe/CANalyzer concepts
- Automotive software concepts
- AUTOSAR awareness
- Functional Safety awareness
- ASPICE awareness

The goal is to develop a realistic embedded/CAN project suitable for automotive and industrial embedded engineering roles.

---

# 1. Project Overview

The system currently contains two independent CAN nodes:

```text
                    CAN BUS

        CANH ===================== CANH
              |                |
              |                |
        CANL ===================== CANL
              |                |
              |                |
        +-------------+    +-------------+
        |   NODE 1    |    |   NODE 2    |
        |             |    |             |
        | MCP2515 #1  |    | MCP2515 #2  |
        +------+------+    +------+------+
               |                  |
               | SPI              | SPI
               |                  |
        +------+------------------+------+
        |       Arduino Mega 2560        |
        +--------------------------------+