#include <SPI.h>
#include <mcp2515.h>

// =====================================================
// MCP2515 CONNECTIONS
// =====================================================

#define MCP1_CS  53
#define MCP1_INT 2

#define MCP2_CS  49
#define MCP2_INT 3

MCP2515 mcp2515_1(MCP1_CS);
MCP2515 mcp2515_2(MCP2_CS);


// =====================================================
// PRINT CAN FRAME
// =====================================================

void printFrame(const char* nodeName, struct can_frame &frame)
{
    Serial.print(nodeName);
    Serial.print(" | ID: 0x");
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
    Serial.println(" INDUSTRIAL CAN - ARBITRATION TEST");
    Serial.println("========================================");

    Serial.println();
    Serial.println("NODE 1 -> CAN ID 0x300");
    Serial.println("NODE 2 -> CAN ID 0x100");
    Serial.println("Expected winner: NODE 2 (0x100)");
    Serial.println();


    // =================================================
    // NODE 1 INITIALIZATION
    // =================================================

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

    if (mcp2515_2.setNormalMode()
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] Normal mode FAILED");
        while (1);
    }

    Serial.println("[NODE 2] READY");
    Serial.println("[NODE 2] CS = 49");
    Serial.println("[NODE 2] INT = 3");


    // =================================================
    // CREATE NODE 1 FRAME
    // ID = 0x300
    // =================================================

    struct can_frame tx1;

    tx1.can_id = 0x300;
    tx1.can_dlc = 8;

    uint8_t data1[8] =
    {
        0x30, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07
    };

    for (uint8_t i = 0; i < 8; i++)
    {
        tx1.data[i] = data1[i];
    }


    // =================================================
    // CREATE NODE 2 FRAME
    // ID = 0x100
    // =================================================

    struct can_frame tx2;

    tx2.can_id = 0x100;
    tx2.can_dlc = 8;

    uint8_t data2[8] =
    {
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17
    };

    for (uint8_t i = 0; i < 8; i++)
    {
        tx2.data[i] = data2[i];
    }


    // =================================================
    // ARBITRATION TEST
    // =================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" STARTING ARBITRATION TEST");
    Serial.println("========================================");

    Serial.println();
    Serial.println("[NODE 1] CAN ID = 0x300");
    Serial.println("[NODE 2] CAN ID = 0x100");

    Serial.println();
    Serial.println("Lower CAN ID has higher bus priority.");
    Serial.println("Therefore:");
    Serial.println("0x100 should win over 0x300.");

    Serial.println();
    Serial.println("Requesting NODE 1 transmission...");

    MCP2515::ERROR result1 =
        mcp2515_1.sendMessage(MCP2515::TXB0, &tx1);

    if (result1 == MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] Transmission request accepted.");
    }
    else
    {
        Serial.println("[NODE 1] Transmission request FAILED.");
    }


    Serial.println();
    Serial.println("Requesting NODE 2 transmission...");

    MCP2515::ERROR result2 =
        mcp2515_2.sendMessage(MCP2515::TXB0, &tx2);

    if (result2 == MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] Transmission request accepted.");
    }
    else
    {
        Serial.println("[NODE 2] Transmission request FAILED.");
    }


    // =================================================
    // WAIT FOR CAN BUS ACTIVITY
    // =================================================

    delay(100);


    // =================================================
    // CHECK NODE 1 RECEIVE BUFFER
    // =================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" RECEIVED FRAMES");
    Serial.println("========================================");

    struct can_frame rx1;

    if (mcp2515_1.readMessage(&rx1)
        == MCP2515::ERROR_OK)
    {
        printFrame("[NODE 1]", rx1);
    }
    else
    {
        Serial.println("[NODE 1] No received frame.");
    }


    // =================================================
    // CHECK NODE 2 RECEIVE BUFFER
    // =================================================

    struct can_frame rx2;

    if (mcp2515_2.readMessage(&rx2)
        == MCP2515::ERROR_OK)
    {
        printFrame("[NODE 2]", rx2);
    }
    else
    {
        Serial.println("[NODE 2] No received frame.");
    }


    // =================================================
    // FINAL RESULT
    // =================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" ARBITRATION TEST COMPLETE");
    Serial.println("========================================");

    Serial.println();
    Serial.println("Expected priority:");
    Serial.println("0x100 > 0x300");

    Serial.println();
    Serial.println("NOTE:");
    Serial.println("A true arbitration event requires both");
    Serial.println("controllers to request transmission while");
    Serial.println("the CAN bus is available at the same time.");
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
}