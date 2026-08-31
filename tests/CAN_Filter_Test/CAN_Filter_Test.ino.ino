#include <SPI.h>
#include <mcp2515.h>

// =====================================================
// MCP2515 CONNECTIONS
// =====================================================

#define MCP1_CS   53
#define MCP1_INT  2

#define MCP2_CS   49
#define MCP2_INT  3

MCP2515 mcp2515_1(MCP1_CS);
MCP2515 mcp2515_2(MCP2_CS);


// =====================================================
// PRINT CAN FRAME
// =====================================================

void printFrame(const char* nodeName, struct can_frame &frame)
{
    Serial.print(nodeName);
    Serial.print(" RX | ID: 0x");

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


// =====================================================
// SETUP
// =====================================================

void setup()
{
    Serial.begin(115200);

    SPI.begin();

    Serial.println();
    Serial.println("========================================");
    Serial.println("       INDUSTRIAL CAN - FILTER TEST");
    Serial.println("========================================");


    // =================================================
    // NODE 1 INITIALIZATION
    // =================================================

    Serial.println();
    Serial.println("[NODE 1] Initializing MCP2515...");

    mcp2515_1.reset();

    if (mcp2515_1.setBitrate(CAN_500KBPS, MCP_8MHZ)
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] Bitrate configuration FAILED");
        while (1);
    }

    if (mcp2515_1.setNormalMode()
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] Normal mode FAILED");
        while (1);
    }

    Serial.println("[NODE 1] READY");
    Serial.println("[NODE 1] CS = 53");
    Serial.println("[NODE 1] INT = 2");


    // =================================================
    // NODE 2 INITIALIZATION
    // =================================================

    Serial.println();
    Serial.println("[NODE 2] Initializing MCP2515...");

    mcp2515_2.reset();

    if (mcp2515_2.setBitrate(CAN_500KBPS, MCP_8MHZ)
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] Bitrate configuration FAILED");
        while (1);
    }


    // =================================================
    // NODE 2 CAN FILTER CONFIGURATION
    //
    // ACCEPT ONLY STANDARD CAN ID 0x100
    // =================================================

    Serial.println("[NODE 2] Configuring CAN filter...");

    // Mask 0:
    // Check all 11 bits of the standard CAN identifier.
    if (mcp2515_2.setFilterMask(
            MCP2515::MASK0,
            false,
            0x7FF) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] MASK0 configuration FAILED");
        while (1);
    }

    // RXF0 and RXF1 belong to MASK0.
    if (mcp2515_2.setFilter(
            MCP2515::RXF0,
            false,
            0x100) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RXF0 configuration FAILED");
        while (1);
    }

    if (mcp2515_2.setFilter(
            MCP2515::RXF1,
            false,
            0x100) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RXF1 configuration FAILED");
        while (1);
    }


    // Mask 1:
    // Check all 11 bits of the standard CAN identifier.
    if (mcp2515_2.setFilterMask(
            MCP2515::MASK1,
            false,
            0x7FF) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] MASK1 configuration FAILED");
        while (1);
    }

    // RXF2-RXF5 belong to MASK1.
    if (mcp2515_2.setFilter(
            MCP2515::RXF2,
            false,
            0x100) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RXF2 configuration FAILED");
        while (1);
    }

    if (mcp2515_2.setFilter(
            MCP2515::RXF3,
            false,
            0x100) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RXF3 configuration FAILED");
        while (1);
    }

    if (mcp2515_2.setFilter(
            MCP2515::RXF4,
            false,
            0x100) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RXF4 configuration FAILED");
        while (1);
    }

    if (mcp2515_2.setFilter(
            MCP2515::RXF5,
            false,
            0x100) != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] RXF5 configuration FAILED");
        while (1);
    }


    // Now switch Node 2 to normal mode.
    if (mcp2515_2.setNormalMode()
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] Normal mode FAILED");
        while (1);
    }

    Serial.println("[NODE 2] READY");
    Serial.println("[NODE 2] CS = 49");
    Serial.println("[NODE 2] INT = 3");
    Serial.println("[NODE 2] FILTER = ACCEPT ONLY 0x100");


    // =================================================
    // TEST 1
    // SEND ID 0x100
    // =================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" TEST 1: SEND ID 0x100");
    Serial.println("========================================");

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

    Serial.println("[NODE 1] Sending ID 0x100...");

    if (mcp2515_1.sendMessage(&tx1)
        == MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] TX SUCCESS");
    }
    else
    {
        Serial.println("[NODE 1] TX FAILED");
        while (1);
    }

    delay(50);

    struct can_frame rx1;

    if (mcp2515_2.readMessage(&rx1)
        == MCP2515::ERROR_OK)
    {
        printFrame("[NODE 2]", rx1);

        if (rx1.can_id == 0x100)
        {
            Serial.println("[TEST 1] PASS - ID 0x100 ACCEPTED");
        }
        else
        {
            Serial.println("[TEST 1] FAIL - WRONG ID RECEIVED");
        }
    }
    else
    {
        Serial.println("[TEST 1] FAIL - ID 0x100 NOT RECEIVED");
    }


    // =================================================
    // TEST 2
    // SEND ID 0x200
    //
    // EXPECTED:
    // NODE 2 MUST REJECT THIS FRAME
    // =================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" TEST 2: SEND ID 0x200");
    Serial.println("========================================");

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

    Serial.println("[NODE 1] Sending ID 0x200...");

    if (mcp2515_1.sendMessage(&tx2)
        == MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] TX SUCCESS");
    }
    else
    {
        Serial.println("[NODE 1] TX FAILED");
        while (1);
    }

    delay(50);

    struct can_frame rx2;

    if (mcp2515_2.readMessage(&rx2)
        == MCP2515::ERROR_OK)
    {
        printFrame("[NODE 2]", rx2);

        if (rx2.can_id == 0x200)
        {
            Serial.println("[TEST 2] FAIL - ID 0x200 WAS ACCEPTED");
        }
        else
        {
            Serial.println("[TEST 2] FAIL - UNEXPECTED FRAME");
        }
    }
    else
    {
        Serial.println("[TEST 2] PASS - ID 0x200 REJECTED");
    }


    // =================================================
    // TEST COMPLETE
    // =================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println("      CAN FILTER TEST COMPLETE");
    Serial.println("========================================");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
}