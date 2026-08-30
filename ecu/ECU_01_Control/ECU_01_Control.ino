#include <Arduino.h>
#include "src/ECU_Control.h"

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" ECU 01 - CONTROL ECU");
    Serial.println("=================================");

    if (!ECU_Control_Init())
    {
        Serial.println("ERROR: ECU initialization failed!");

        while (1)
        {
        }
    }

    Serial.println("ECU 01 initialized successfully.");
    Serial.println("CAN bitrate: 500 kbit/s");
    Serial.println("MCP2515 clock: 8 MHz");
    Serial.println("CAN ID: 0x100");
    Serial.println("Cycle time: 100 ms");
}

void loop()
{
    ECU_Control_Run();
}