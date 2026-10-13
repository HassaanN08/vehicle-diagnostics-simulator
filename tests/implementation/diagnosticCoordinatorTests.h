#pragma once

void testEngineSession();

void testWrongFrameLength();

void testUnknownUdsRequestId();

void testUnsupportedUdsFunction();

void testIncorrectUdsPayloadLength();

void testReadDataByIdentifier();

void testClearDiagnosticInformation();

void testMultiFrameTransport();

void testReadDtcInformation();

void testTxTimeoutClearsOutstandingResponse();

inline void diagnosticCoordinatorTests() {
    testEngineSession();
    testWrongFrameLength();
    testUnknownUdsRequestId();
    testUnsupportedUdsFunction();
    testIncorrectUdsPayloadLength();
    testReadDataByIdentifier();
    testClearDiagnosticInformation();
    testMultiFrameTransport();
    testReadDtcInformation();
    testTxTimeoutClearsOutstandingResponse();
}