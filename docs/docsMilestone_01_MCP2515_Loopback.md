# Milestone 01 — MCP2515 Loopback Test

## Objective

Verify communication between the Arduino Mega 2560 and the MCP2515 CAN controller through the SPI interface.

## Hardware

- Arduino Mega 2560
- MCP2515 CAN controller/transceiver module
- 8 MHz MCP2515 oscillator

## Configuration

- CAN bitrate: 500 kbit/s
- MCP2515 oscillator: 8 MHz
- SPI interface: Arduino Mega hardware SPI
- Chip Select (CS): Digital pin 53
- Loopback mode: Enabled

## SPI Connections

| MCP2515 | Arduino Mega 2560 |
|---|---:|
| VCC | 5V |
| GND | GND |
| SCK | 52 |
| SI / MOSI | 51 |
| SO / MISO | 50 |
| CS | 53 |

## Test CAN Frame

CAN ID:

`0x100`

DLC:

`8`

Data:

`11 22 33 44 55 66 77 88`

## Result

The MCP2515 successfully transmitted and received the CAN frame in internal loopback mode.

```text
CAN ID: 0x100
DLC: 8
DATA: 11 22 33 44 55 66 77 88

MCP2515 LOOPBACK TEST PASSED