#include <SPI.h>
#include <mcp2515.h>

// ============================================================
// MCP2515 CONFIGURATION
// ============================================================

#define MCP1_CS 53
#define MCP2_CS 49

#define MCP1_INT 2
#define MCP2_INT 3

MCP2515 mcp2515_1(MCP1_CS);
MCP2515 mcp2515_2(MCP2_CS);

// ============================================================
// UDS CAN IDs
// ============================================================

#define UDS_REQUEST_ID   0x700
#define UDS_RESPONSE_ID  0x708

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
            Serial.print("0");

        Serial.print(frame.data[i], HEX);
        Serial.print(" ");
    }

    Serial.println();
}

// ============================================================
// SEND UDS REQUEST
// ============================================================

bool sendUDSRequest(uint8_t service, uint8_t subFunction)
{
    struct can_frame request;

    request.can_id = UDS_REQUEST_ID;
    request.can_dlc = 2;

    request.data[0] = service;
    request.data[1] = subFunction;

    Serial.println();
    Serial.println("[TESTER] Sending UDS request...");

    Serial.print("[TESTER] Service = 0x");
    Serial.println(service, HEX);

    Serial.print("[TESTER] Sub-function = 0x");
    Serial.println(subFunction, HEX);

    if (mcp2515_1.sendMessage(&request) == MCP2515::ERROR_OK)
    {
        Serial.println("[TESTER] TX SUCCESS");
        return true;
    }

    Serial.println("[TESTER] TX FAILED");
    return false;
}

// ============================================================
// PROCESS UDS REQUEST
// ============================================================

void processUDSRequest(struct can_frame &request)
{
    Serial.println();
    Serial.println("[ECU] Diagnostic request received");

    printFrame("[ECU] RX", request);

    // --------------------------------------------------------
    // Check for UDS Service 0x10
    // --------------------------------------------------------

    if (request.can_dlc >= 2 &&
        request.data[0] == 0x10)
    {
        uint8_t session = request.data[1];

        Serial.println("[ECU] Service 0x10 detected");
        Serial.print("[ECU] Requested session = 0x");
        Serial.println(session, HEX);

        // ----------------------------------------------------
        // Default Session = 0x01
        // ----------------------------------------------------

        if (session == 0x01)
        {
            struct can_frame response;

            response.can_id = UDS_RESPONSE_ID;
            response.can_dlc = 2;

            // Positive response:
            // 0x10 + 0x40 = 0x50
            response.data[0] = 0x50;
            response.data[1] = 0x01;

            Serial.println("[ECU] Sending positive response...");

            if (mcp2515_2.sendMessage(&response)
                == MCP2515::ERROR_OK)
            {
                Serial.println("[ECU] TX SUCCESS");

                printFrame("[ECU] TX", response);
            }
            else
            {
                Serial.println("[ECU] TX FAILED");
            }
        }

        // ----------------------------------------------------
        // Unsupported session
        // ----------------------------------------------------

        else
        {
            struct can_frame response;

            response.can_id = UDS_RESPONSE_ID;
            response.can_dlc = 3;

            // UDS Negative Response:
            // 0x7F <service> <NRC>

            response.data[0] = 0x7F;
            response.data[1] = 0x10;

            // NRC 0x12 = Sub-function not supported
            response.data[2] = 0x12;

            Serial.println("[ECU] Sending negative response...");

            if (mcp2515_2.sendMessage(&response)
                == MCP2515::ERROR_OK)
            {
                Serial.println("[ECU] Negative response sent");

                printFrame("[ECU] TX", response);
            }
        }
    }

    else
    {
        Serial.println("[ECU] Unsupported diagnostic request");
    }
}

// ============================================================
// WAIT FOR UDS RESPONSE
// ============================================================

bool waitForUDSResponse()
{
    unsigned long startTime = millis();

    struct can_frame response;

    while (millis() - startTime < 1000)
    {
        if (mcp2515_1.readMessage(&response)
            == MCP2515::ERROR_OK)
        {
            if (response.can_id == UDS_RESPONSE_ID)
            {
                printFrame("[TESTER] RX", response);

                // ------------------------------------------------
                // Positive response to 0x10
                // ------------------------------------------------

                if (response.can_dlc >= 2 &&
                    response.data[0] == 0x50 &&
                    response.data[1] == 0x01)
                {
                    Serial.println();
                    Serial.println(
                        "[TEST] PASS - UDS Default Session accepted"
                    );

                    return true;
                }

                // ------------------------------------------------
                // Negative response
                // ------------------------------------------------

                if (response.can_dlc >= 3 &&
                    response.data[0] == 0x7F)
                {
                    Serial.println();
                    Serial.println(
                        "[TEST] NEGATIVE RESPONSE RECEIVED"
                    );

                    Serial.print("[TEST] Service = 0x");
                    Serial.println(response.data[1], HEX);

                    Serial.print("[TEST] NRC = 0x");
                    Serial.println(response.data[2], HEX);

                    return false;
                }
            }
        }
    }

    Serial.println();
    Serial.println("[TEST] FAIL - No UDS response received");

    return false;
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    SPI.begin();

    pinMode(MCP1_CS, OUTPUT);
    pinMode(MCP2_CS, OUTPUT);

    digitalWrite(MCP1_CS, HIGH);
    digitalWrite(MCP2_CS, HIGH);

    Serial.println();
    Serial.println("========================================");
    Serial.println(" INDUSTRIAL CAN - UDS BASIC TEST");
    Serial.println("========================================");

    // ========================================================
    // NODE 1 - TESTER
    // ========================================================

    Serial.println();
    Serial.println("[NODE 1] Initializing MCP2515...");

    mcp2515_1.reset();

    if (mcp2515_1.setBitrate(
            CAN_500KBPS,
            MCP_8MHZ) != MCP2515::ERROR_OK)
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

    if (mcp2515_2.setBitrate(
            CAN_500KBPS,
            MCP_8MHZ) != MCP2515::ERROR_OK)
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
    Serial.println("[NODE 2] ROLE = ECU");
    Serial.println("[NODE 2] CS = 49");
    Serial.println("[NODE 2] INT = 3");
    Serial.println("[NODE 2] RESPONSE ID = 0x708");

    // ========================================================
    // UDS TEST
    // ========================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" UDS TEST - SERVICE 0x10");
    Serial.println("========================================");

    Serial.println();
    Serial.println("[TEST] Diagnostic Session Control");
    Serial.println("[TEST] Request: 10 01");
    Serial.println("[TEST] Expected response: 50 01");

    // --------------------------------------------------------
    // Send request from Tester
    // --------------------------------------------------------

    if (!sendUDSRequest(0x10, 0x01))
    {
        Serial.println("[TEST] UDS REQUEST FAILED");
        while (1);
    }

    // --------------------------------------------------------
    // ECU receives request
    // --------------------------------------------------------

    struct can_frame ecuRequest;

    unsigned long startTime = millis();

    bool requestReceived = false;

    while (millis() - startTime < 1000)
    {
        if (mcp2515_2.readMessage(&ecuRequest)
            == MCP2515::ERROR_OK)
        {
            if (ecuRequest.can_id == UDS_REQUEST_ID)
            {
                requestReceived = true;
                break;
            }
        }
    }

    if (!requestReceived)
    {
        Serial.println("[ECU] FAIL - No diagnostic request received");
        while (1);
    }

    processUDSRequest(ecuRequest);

    // --------------------------------------------------------
    // Tester receives ECU response
    // --------------------------------------------------------

    waitForUDSResponse();

    // ========================================================
    // COMPLETE
    // ========================================================

    Serial.println();
    Serial.println("========================================");
    Serial.println(" UDS BASIC TEST COMPLETE");
    Serial.println("========================================");
}

void loop()
{
}