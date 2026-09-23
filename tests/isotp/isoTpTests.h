#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "isotp/NewIsoTp.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "can/CANFrame.h"

inline void isoTpTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    const ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    const ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    auto engineIsoTpEndpoint { NewIsoTp::createNewIsoTpEndpoint(engine->getRequestCANId(), engine->getResponseCANId()) };
    auto testerIsoTpEndpoint { NewIsoTp::createNewIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x00, 0x00 };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { testerIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame);
        assert(frame->getFramePayload() == correctFirstFrame->getFramePayload());

        IsoTpReceiveFrameResult result { engineIsoTpEndpoint->receiveFrame(*frame) };
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame = engineIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload() == CtsPayload);

        result = testerIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload() == correctCF1->getFramePayload());

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);

        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload() == correctCF2->getFramePayload());

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);

        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload() == correctCF3->getFramePayload());

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        std::vector<std::uint8_t> completedPayload { engineIsoTpEndpoint->getCompleteReassembledPayload() };

        assert(completedPayload == originalPayload);
    }
}