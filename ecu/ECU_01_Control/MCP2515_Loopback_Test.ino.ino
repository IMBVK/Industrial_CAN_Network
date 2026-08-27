#include <SPI.h>
#include <mcp2515.h>

MCP2515 mcp2515(53);

struct can_frame canMsg;

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" INDUSTRIAL CAN - MCP2515 TEST");
    Serial.println("=================================");

    SPI.begin();

    Serial.println("Resetting MCP2515...");

    if (mcp2515.reset() != MCP2515::ERROR_OK)
    {
        Serial.println("ERROR: MCP2515 reset failed!");
        while (1);
    }

    Serial.println("MCP2515 reset OK.");

    Serial.println("Configuring CAN: 500 kbps / 8 MHz");

    if (mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ) != MCP2515::ERROR_OK)
    {
        Serial.println("ERROR: CAN bitrate configuration failed!");
        while (1);
    }

    Serial.println("CAN bitrate configured.");

    if (mcp2515.setLoopbackMode() != MCP2515::ERROR_OK)
    {
        Serial.println("ERROR: Could not enter loopback mode!");
        while (1);
    }

    Serial.println("MCP2515 loopback mode enabled.");

    // Create CAN frame
    canMsg.can_id  = 0x100;
    canMsg.can_dlc = 8;

    canMsg.data[0] = 0x11;
    canMsg.data[1] = 0x22;
    canMsg.data[2] = 0x33;
    canMsg.data[3] = 0x44;
    canMsg.data[4] = 0x55;
    canMsg.data[5] = 0x66;
    canMsg.data[6] = 0x77;
    canMsg.data[7] = 0x88;

    Serial.println();
    Serial.println("Sending CAN frame...");

    if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK)
    {
        Serial.println("CAN frame transmitted internally.");
    }
    else
    {
        Serial.println("ERROR: CAN transmission failed!");
        while (1);
    }

    // Try receiving the frame
    struct can_frame receivedMsg;

    if (mcp2515.readMessage(&receivedMsg) == MCP2515::ERROR_OK)
    {
        Serial.println("CAN frame received!");
        Serial.println();

        Serial.print("CAN ID: 0x");
        Serial.println(receivedMsg.can_id, HEX);

        Serial.print("DLC: ");
        Serial.println(receivedMsg.can_dlc);

        Serial.print("DATA: ");

        for (int i = 0; i < receivedMsg.can_dlc; i++)
        {
            if (receivedMsg.data[i] < 0x10)
            {
                Serial.print("0");
            }

            Serial.print(receivedMsg.data[i], HEX);
            Serial.print(" ");
        }

        Serial.println();
        Serial.println();
        Serial.println("=================================");
        Serial.println(" MCP2515 LOOPBACK TEST PASSED");
        Serial.println("=================================");
    }
    else
    {
        Serial.println("ERROR: No CAN frame received!");
    }
}

void loop()
{
}