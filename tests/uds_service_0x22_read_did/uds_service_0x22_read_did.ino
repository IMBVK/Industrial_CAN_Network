#include <SPI.h>
#include <mcp2515.h>

// ============================================================
// MCP2515 CONFIGURATION
// ============================================================

#define MCP1_CS 53
#define MCP2_CS 49

#define MCP1_INT 2
#define MCP2_INT 3

#define CAN_SPEED CAN_500KBPS
#define CAN_CLOCK MCP_8MHZ

// ============================================================
// CAN IDs
// ============================================================

#define UDS_REQUEST_ID  0x700
#define UDS_RESPONSE_ID 0x708

// ============================================================
// UDS SERVICE
// ============================================================

#define UDS_SERVICE_READ_DID 0x22

// DID: F190
// Commonly associated with VIN in automotive diagnostics.
// Here we use simulated ECU data for learning/testing.
#define TEST_DID_HIGH 0xF1
#define TEST_DID_LOW  0x90

// Simulated ECU data
uint8_t ecuData[4] =
{
    0x12,
    0x34,
    0x56,
    0x78
};

// ============================================================
// MCP2515 OBJECTS
// ============================================================

MCP2515 mcp2515_1(MCP1_CS);
MCP2515 mcp2515_2(MCP2_CS);

// ============================================================
// PRINT CAN FRAME
// ============================================================

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
        {
            Serial.print("0");
        }

        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
    }

    Serial.println();
}

// ============================================================
// WAIT FOR FRAME
// ============================================================

bool waitForFrame(
    MCP2515 &mcp,
    struct can_frame &frame,
    unsigned long timeoutMs
)
{
    unsigned long startTime = millis();

    while (millis() - startTime < timeoutMs)
    {
        if (mcp.readMessage(&frame) == MCP2515::ERROR_OK)
        {
            return true;
        }

        delay(1);
    }

    return false;
}

// ============================================================
// CHECK UDS REQUEST
// ============================================================

bool isCorrectRequest(struct can_frame &frame)
{
    if (frame.can_id != UDS_REQUEST_ID)
    {
        return false;
    }

    if (frame.can_dlc != 3)
    {
        return false;
    }

    if (frame.data[0] != UDS_SERVICE_READ_DID)
    {
        return false;
    }

    if (frame.data[1] != TEST_DID_HIGH)
    {
        return false;
    }

    if (frame.data[2] != TEST_DID_LOW)
    {
        return false;
    }

    return true;
}

// ============================================================
// CHECK UDS RESPONSE
// ============================================================

bool isCorrectResponse(struct can_frame &frame)
{
    if (frame.can_id != UDS_RESPONSE_ID)
    {
        return false;
    }

    if (frame.can_dlc != 7)
    {
        return false;
    }

    // Positive response for Service 0x22
    // 0x22 + 0x40 = 0x62
    if (frame.data[0] != 0x62)
    {
        return false;
    }

    // DID
    if (frame.data[1] != TEST_DID_HIGH)
    {
        return false;
    }

    if (frame.data[2] != TEST_DID_LOW)
    {
        return false;
    }

    // ECU data
    for (uint8_t i = 0; i < 4; i++)
    {
        if (frame.data[i + 3] != ecuData[i])
        {
            return false;
        }
    }

    return true;
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    SPI.begin();

    Serial.println();
    Serial.println("========================================");
    Serial.println(" INDUSTRIAL CAN - UDS SERVICE 0x22 TEST");
    Serial.println("========================================");

    // ========================================================
    // NODE 1 - DIAGNOSTIC TESTER
    // ========================================================

    Serial.println();
    Serial.println("[NODE 1] Initializing MCP2515...");

    mcp2515_1.reset();

    if (mcp2515_1.setBitrate(CAN_SPEED, CAN_CLOCK)
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] Bitrate configuration FAILED");
        while (1);
    }

    if (mcp2515_1.setNormalMode()
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 1] Normal mode configuration FAILED");
        while (1);
    }

    Serial.println("[NODE 1] READY");
    Serial.println("[NODE 1] ROLE = DIAGNOSTIC TESTER");
    Serial.println("[NODE 1] CS = 53");
    Serial.println("[NODE 1] INT = 2");
    Serial.println("[NODE 1] REQUEST ID = 0x700");

    // ========================================================
    // NODE 2 - ECU
    // ========================================================

    Serial.println();
    Serial.println("[NODE 2] Initializing MCP2515...");

    mcp2515_2.reset();

    if (mcp2515_2.setBitrate(CAN_SPEED, CAN_CLOCK)
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] Bitrate configuration FAILED");
        while (1);
    }

    if (mcp2515_2.setNormalMode()
        != MCP2515::ERROR_OK)
    {
        Serial.println("[NODE 2] Normal mode configuration FAILED");
        while (1);
    }

    Serial.println("[NODE 2] READY");
    Serial.println("[NODE 2] ROLE = ECU");
    Serial.println("[NODE 2] CS = 49");
    Serial.println("[NODE 2] INT = 3");
    Serial.println("[NODE 2] RESPONSE ID = 0x708");

    // ========================================================
    // TEST INFORMATION
    // ========================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" UDS SERVICE 0x22 - READ DATA BY ID");
    Serial.println("========================================");

    Serial.println();
    Serial.println("[TEST] Service = 0x22");
    Serial.println("[TEST] DID = 0xF190");

    Serial.println("[TEST] Request:");
    Serial.println("       22 F1 90");

    Serial.println("[TEST] Expected positive response:");
    Serial.println("       62 F1 90 12 34 56 78");

    // ========================================================
    // BUILD UDS REQUEST
    // ========================================================

    struct can_frame request;

    request.can_id = UDS_REQUEST_ID;
    request.can_dlc = 3;

    request.data[0] = UDS_SERVICE_READ_DID;
    request.data[1] = TEST_DID_HIGH;
    request.data[2] = TEST_DID_LOW;

    // ========================================================
    // TESTER SENDS REQUEST
    // ========================================================

    Serial.println();
    Serial.println("[TESTER] Sending ReadDataByIdentifier request...");

    if (mcp2515_1.sendMessage(&request)
        != MCP2515::ERROR_OK)
    {
        Serial.println("[TESTER] TX FAILED");
        while (1);
    }

    Serial.println("[TESTER] TX SUCCESS");

    // ========================================================
    // ECU RECEIVES REQUEST
    // ========================================================

    struct can_frame ecuRequest;

    if (!waitForFrame(mcp2515_2, ecuRequest, 1000))
    {
        Serial.println("[ECU] RX TIMEOUT");
        while (1);
    }

    printFrame("[ECU] RX", ecuRequest);

    // ========================================================
    // ECU VALIDATES REQUEST
    // ========================================================

    if (!isCorrectRequest(ecuRequest))
    {
        Serial.println("[ECU] INVALID UDS REQUEST");
        while (1);
    }

    Serial.println("[ECU] Service 0x22 detected");
    Serial.println("[ECU] DID 0xF190 detected");
    Serial.println("[ECU] DID data available");

    // ========================================================
    // ECU BUILDS POSITIVE RESPONSE
    // ========================================================

    struct can_frame response;

    response.can_id = UDS_RESPONSE_ID;
    response.can_dlc = 7;

    // Positive response:
    // 0x22 + 0x40 = 0x62

    response.data[0] = 0x62;

    // Return requested DID
    response.data[1] = TEST_DID_HIGH;
    response.data[2] = TEST_DID_LOW;

    // Return ECU data
    response.data[3] = ecuData[0];
    response.data[4] = ecuData[1];
    response.data[5] = ecuData[2];
    response.data[6] = ecuData[3];

    // ========================================================
    // ECU SENDS RESPONSE
    // ========================================================

    Serial.println();
    Serial.println("[ECU] Sending positive response...");

    if (mcp2515_2.sendMessage(&response)
        != MCP2515::ERROR_OK)
    {
        Serial.println("[ECU] TX FAILED");
        while (1);
    }

    Serial.println("[ECU] TX SUCCESS");

    printFrame("[ECU] TX", response);

    // ========================================================
    // TESTER RECEIVES RESPONSE
    // ========================================================

    struct can_frame testerResponse;

    if (!waitForFrame(mcp2515_1, testerResponse, 1000))
    {
        Serial.println("[TESTER] RX TIMEOUT");
        while (1);
    }

    printFrame("[TESTER] RX", testerResponse);

    // ========================================================
    // VALIDATE RESPONSE
    // ========================================================

    Serial.println();
    Serial.println("[TESTER] Validating UDS response...");

    if (isCorrectResponse(testerResponse))
    {
        Serial.println("[TEST] PASS - DID 0xF190 READ SUCCESSFULLY");
    }
    else
    {
        Serial.println("[TEST] FAIL - INVALID UDS RESPONSE");
        while (1);
    }

    // ========================================================
    // TEST COMPLETE
    // ========================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" UDS SERVICE 0x22 TEST COMPLETE");
    Serial.println("========================================");
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
}