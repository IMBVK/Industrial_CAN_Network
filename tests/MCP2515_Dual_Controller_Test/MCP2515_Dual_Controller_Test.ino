#include <SPI.h>
#include <mcp2515.h>

#define MCP1_CS 53
#define MCP2_CS 49

MCP2515 mcp2515_1(MCP1_CS);
MCP2515 mcp2515_2(MCP2_CS);

void printFrame(const char* nodeName, struct can_frame &frame)
{
    Serial.print(nodeName);
    Serial.print(" | ID: 0x");

    if (frame.can_id < 0x100)
        Serial.print("0");

    Serial.print(frame.can_id, HEX);

    Serial.print(" | DLC: ");
    Serial.print(frame.can_dlc);

    Serial.print(" | DATA: ");

    for (uint8_t i = 0; i < frame.can_dlc; i++)
    {
        if (frame.data[i] < 0x10)
            Serial.print("0");

        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
    }

    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    SPI.begin();

    Serial.println();
    Serial.println("========================================");
    Serial.println(" INDUSTRIAL CAN - TWO NODE TEST");
    Serial.println("========================================");

    // ==================================================
    // NODE 1 INITIALIZATION
    // ==================================================

    Serial.println();
    Serial.println("[NODE 1] Initializing MCP2515...");
    Serial.println("[NODE 1] CS  = 53");
    Serial.println("[NODE 1] INT = 2");

    if (mcp2515_1.reset() != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] RESET FAILED");
        while (1);
    }

    if (mcp2515_1.setBitrate(CAN_500KBPS, MCP_8MHZ) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] BITRATE CONFIGURATION FAILED");
        while (1);
    }

    if (mcp2515_1.setNormalMode() != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] NORMAL MODE FAILED");
        while (1);
    }

    Serial.println("[NODE 1] READY");


    // ==================================================
    // NODE 2 INITIALIZATION
    // ==================================================

    Serial.println();
    Serial.println("[NODE 2] Initializing MCP2515...");
    Serial.println("[NODE 2] CS  = 49");
    Serial.println("[NODE 2] INT = 3");

    if (mcp2515_2.reset() != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RESET FAILED");
        while (1);
    }

    if (mcp2515_2.setBitrate(CAN_500KBPS, MCP_8MHZ) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] BITRATE CONFIGURATION FAILED");
        while (1);
    }

    if (mcp2515_2.setNormalMode() != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] NORMAL MODE FAILED");
        while (1);
    }

    Serial.println("[NODE 2] READY");


    // ==================================================
    // NODE 1 -> NODE 2
    // ==================================================

    struct can_frame tx1;

    tx1.can_id = 0x100;
    tx1.can_dlc = 8;

    uint8_t data1[8] =
    {
        0x11, 0x22, 0x33, 0x44,
        0x55, 0x66, 0x77, 0x88
    };

    for (uint8_t i = 0; i < 8; i++)
    {
        tx1.data[i] = data1[i];
    }

    Serial.println();
    Serial.println("========================================");
    Serial.println(" TEST 1: NODE 1 -> NODE 2");
    Serial.println("========================================");

    Serial.println("[NODE 1] Sending ID 0x100...");

    if (mcp2515_1.sendMessage(&tx1) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] TX FAILED");
        while (1);
    }

    Serial.println("[NODE 1] TX SUCCESS");


    delay(100);


    struct can_frame rx2;

    if (mcp2515_2.readMessage(&rx2) == MCP2515::ERROR_OK)
    {
        printFrame("[NODE 2] RX", rx2);

        if (rx2.can_id == 0x100)
        {
            Serial.println("[NODE 2] MESSAGE RECEIVED CORRECTLY");
        }
        else
        {
            Serial.println("[NODE 2] WRONG CAN ID");
        }
    }
    else
    {
        Serial.println("[NODE 2] RX FAILED");
    }


    // ==================================================
    // NODE 2 -> NODE 1
    // ==================================================

    struct can_frame tx2;

    tx2.can_id = 0x200;
    tx2.can_dlc = 8;

    uint8_t data2[8] =
    {
        0xAA, 0xBB, 0xCC, 0xDD,
        0xEE, 0xFF, 0x12, 0x34
    };

    for (uint8_t i = 0; i < 8; i++)
    {
        tx2.data[i] = data2[i];
    }

    Serial.println();
    Serial.println("========================================");
    Serial.println(" TEST 2: NODE 2 -> NODE 1");
    Serial.println("========================================");

    Serial.println("[NODE 2] Sending ID 0x200...");

    if (mcp2515_2.sendMessage(&tx2) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] TX FAILED");
        while (1);
    }

    Serial.println("[NODE 2] TX SUCCESS");


    delay(100);


    struct can_frame rx1;

    if (mcp2515_1.readMessage(&rx1) == MCP2515::ERROR_OK)
    {
        printFrame("[NODE 1] RX", rx1);

        if (rx1.can_id == 0x200)
        {
            Serial.println("[NODE 1] MESSAGE RECEIVED CORRECTLY");
        }
        else
        {
            Serial.println("[NODE 1] WRONG CAN ID");
        }
    }
    else
    {
        Serial.println("[NODE 1] RX FAILED");
    }


    // ==================================================
    // TEST COMPLETE
    // ==================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" TWO-NODE CAN TEST COMPLETE");
    Serial.println("========================================");
}

void loop()
{
}