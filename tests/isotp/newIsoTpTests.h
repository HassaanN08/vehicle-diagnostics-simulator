#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "isotp/NewIsoTp.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "can/CANFrame.h"

void newIsoTpTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    const ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    const ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    IsoTp::Receiver engineReceiver;
    IsoTp::Receiver brakeReceiver;
    IsoTp::Receiver batteryReceiver;

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}) };

        ReceiveFrameResult result { engineReceiver.receiveFrame(*correctFirstFrame) };
        assert(result == ReceiveFrameResult::NeedToSendFC);
        assert(engineReceiver.getCurrentState() == ReceiverState::Reassembling);

        std::array<std::uint8_t, 3> flowControlFrame { engineReceiver.getFlowControlFrame() };
        assert(flowControlFrame[0] == 0x30 & flowControlFrame[1] == 0x00 & flowControlFrame[2] == 0x00);

        result = engineReceiver.receiveFrame(*correctCF1);
        assert(result == ReceiveFrameResult::WaitingForMoreFrames);

        result = brakeReceiver.receiveFrame(*correctFirstFrame);
        assert(result == ReceiveFrameResult::NeedToSendFC && brakeReceiver.getUsefulBytesCollected() == 6);

        result = engineReceiver.receiveFrame(*correctCF2);
        assert(result == ReceiveFrameResult::WaitingForMoreFrames);

        result = engineReceiver.receiveFrame(*correctCF3);
        assert(result == ReceiveFrameResult::CompletedPayload);

        std::vector<std::uint8_t> assembledPayload { engineReceiver.getReassembledPayload() };
        std::vector<std::uint8_t> partialReassemblyBuffer { engineReceiver.getPartialReassemblyBuffer() };
        assert(assembledPayload == originalPayload && assembledPayload.size() == originalPayload.size());
        assert(engineReceiver.getCurrentState() == ReceiverState::Idle);
        assert(partialReassemblyBuffer.empty());
    }

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89};

        auto correctSingleFrame { CANFrame::createCANFrame(0x7E0, {0x05, 0x22, 0xF1, 0x90, 0xF1, 0x89}) };
        auto incorrectSingleFrame { CANFrame::createCANFrame(0x7E0, {0x02, 0x22, 0xF1, 0x90, 0xF1, 0x89}) };
       
        ReceiveFrameResult result { engineReceiver.receiveFrame(*incorrectSingleFrame) };
        assert(result == ReceiveFrameResult::TransportError);
        assert(engineReceiver.getCurrentState() == ReceiverState::Idle);

        result = engineReceiver.receiveFrame(*correctSingleFrame);
        assert(result == ReceiveFrameResult::CompletedPayload);
        assert(engineReceiver.getCurrentState() == ReceiverState::Idle);

        std::vector<std::uint8_t> assembledPayload { engineReceiver.getReassembledPayload() };
        assert(assembledPayload == originalPayload);
    }

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}) };

        ReceiveFrameResult result { engineReceiver.receiveFrame(*correctFirstFrame) };
        assert(result == ReceiveFrameResult::NeedToSendFC);
        assert(engineReceiver.getCurrentState() == ReceiverState::Reassembling);

        std::array<std::uint8_t, 3> flowControlFrame { engineReceiver.getFlowControlFrame() };
        assert(flowControlFrame[0] == 0x30 & flowControlFrame[1] == 0x00 & flowControlFrame[2] == 0x00);

        result = engineReceiver.receiveFrame(*correctCF1);
        assert(result == ReceiveFrameResult::WaitingForMoreFrames);

        result = engineReceiver.receiveFrame(*correctCF3);
        assert(result == ReceiveFrameResult::TransportError);

        std::vector<std::uint8_t> assembledPayload { engineReceiver.getReassembledPayload() };
        assert(assembledPayload.empty());
        assert(engineReceiver.getCurrentState() == ReceiverState::Idle);
    }
}