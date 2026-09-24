#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "isotp/Receiver.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "can/CANFrame.h"

inline void receiverTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    const ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    const ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    auto engineReceiver { Receiver::createReceiver(engine->getRequestCANId(), engine->getResponseCANId()) };
    auto brakeReceiver { Receiver::createReceiver(brake->getRequestCANId(), brake->getResponseCANId()) };
    auto batteryReceiver { Receiver::createReceiver(battery->getRequestCANId(), battery->getResponseCANId()) };

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}) };

        auto incorrectFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89}) };

        ReceivePayloadResult result { engineReceiver->receivePayload(correctFirstFrame->getFramePayload()) };
        assert(result == ReceivePayloadResult::NeedToSendFC);
        assert(engineReceiver->getCurrentState() == ReceiverState::SenderPaused);

        auto flowControlFrame { engineReceiver->getFlowControlFrame() };
        std::vector<std::uint8_t> flowControlPayload { flowControlFrame->getFramePayload() };
        assert(flowControlPayload[0] == 0x30 && flowControlPayload[1] == 0x00 && flowControlPayload[2] == 0x00);

        result = engineReceiver->receivePayload(correctCF1->getFramePayload());
        assert(result == ReceivePayloadResult::WaitingForMoreFrames);

        result = engineReceiver->receivePayload(correctCF2->getFramePayload());
        assert(result == ReceivePayloadResult::WaitingForMoreFrames);

        result = engineReceiver->receivePayload(correctCF3->getFramePayload());
        assert(result == ReceivePayloadResult::CompletedPayload);

        std::vector<std::uint8_t> assembledPayload { engineReceiver->getReassembledPayload() };
        std::vector<std::uint8_t> partialReassemblyBuffer { engineReceiver->getPartialReassemblyBuffer() };
        assert(assembledPayload == originalPayload && assembledPayload.size() == originalPayload.size());
        assert(engineReceiver->getCurrentState() == ReceiverState::Idle);
        assert(partialReassemblyBuffer.empty());

        result = engineReceiver->receivePayload(incorrectFirstFrame->getFramePayload());
        assert(result == ReceivePayloadResult::TransportError);
    }

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89};

        auto correctSingleFrame { CANFrame::createCANFrame(0x7E0, {0x05, 0x22, 0xF1, 0x90, 0xF1, 0x89}) };
        auto incorrectSingleFrame { CANFrame::createCANFrame(0x7E0, {0x02, 0x22, 0xF1, 0x90, 0xF1, 0x89}) };
       
        ReceivePayloadResult result { engineReceiver->receivePayload(incorrectSingleFrame->getFramePayload()) };
        assert(result == ReceivePayloadResult::TransportError);
        assert(engineReceiver->getCurrentState() == ReceiverState::Idle);

        result = engineReceiver->receivePayload(correctSingleFrame->getFramePayload());
        assert(result == ReceivePayloadResult::CompletedPayload);
        assert(engineReceiver->getCurrentState() == ReceiverState::Idle);

        std::vector<std::uint8_t> assembledPayload { engineReceiver->getReassembledPayload() };
        assert(assembledPayload == originalPayload);
    }

    {
        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto incorrectFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x07, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}) };

        ReceivePayloadResult result { engineReceiver->receivePayload(correctFirstFrame->getFramePayload()) };
        assert(result == ReceivePayloadResult::NeedToSendFC);
        assert(engineReceiver->getCurrentState() == ReceiverState::SenderPaused);

        auto flowControlFrame { engineReceiver->getFlowControlFrame() };
        std::vector<std::uint8_t> flowControlPayload { flowControlFrame->getFramePayload() };
        assert(flowControlPayload[0] == 0x30 && flowControlPayload[1] == 0x00 && flowControlPayload[2] == 0x00);

        result = engineReceiver->receivePayload(correctCF1->getFramePayload());
        assert(result == ReceivePayloadResult::WaitingForMoreFrames);

        result = engineReceiver->receivePayload(correctCF3->getFramePayload());
        assert(result == ReceivePayloadResult::TransportError);

        std::vector<std::uint8_t> assembledPayload { engineReceiver->getReassembledPayload() };
        assert(assembledPayload.empty());
        assert(engineReceiver->getCurrentState() == ReceiverState::Idle);

        result = engineReceiver->receivePayload(incorrectFirstFrame->getFramePayload());
        assert(result == ReceivePayloadResult::TransportError);

        assert(engineReceiver->getCurrentState() == ReceiverState::Idle);

        result = engineReceiver->receivePayload(correctCF1->getFramePayload());
        assert(result == ReceivePayloadResult::TransportError);

        result = engineReceiver->receivePayload(correctFirstFrame->getFramePayload());
        assert(result == ReceivePayloadResult::NeedToSendFC);

        flowControlFrame = engineReceiver->getFlowControlFrame();
        flowControlPayload = flowControlFrame->getFramePayload();
        assert(flowControlPayload[0] == 0x30 && flowControlPayload[1] == 0x00 && flowControlPayload[2] == 0x00);

        result = engineReceiver->receivePayload(correctCF1->getFramePayload());
        assert(result == ReceivePayloadResult::WaitingForMoreFrames);

        result = engineReceiver->receivePayload(correctFirstFrame->getFramePayload());
        assert(result == ReceivePayloadResult::NeedToSendFC);
        assembledPayload = engineReceiver->getPartialReassemblyBuffer();
        assert(assembledPayload.size() == 6);
    }

    {
        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x90, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };

        ReceivePayloadResult result { engineReceiver->receivePayload(correctFirstFrame->getFramePayload()) };
        assert(result == ReceivePayloadResult::NeedToSendFC);
        assert(engineReceiver->getCurrentState() == ReceiverState::SenderPaused);

        auto flowControlFrame { engineReceiver->getFlowControlFrame() };
        std::vector<std::uint8_t> flowControlPayload { flowControlFrame->getFramePayload() };
        assert(flowControlPayload[0] == 0x30 && flowControlPayload[1] == 0x00 && flowControlPayload[2] == 0x00);

        for (int i { 0 }; i < 14; ++i) {
            correctCF = CANFrame::createCANFrame(0x7E0, {static_cast<std::uint8_t>(0x21 + i), 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03});
            engineReceiver->receivePayload(correctCF->getFramePayload());
        }

        assert(engineReceiver->getNextCFSequenceNumber() == 15);

        correctCF = CANFrame::createCANFrame(0x7E0, {0x2F, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03});
        engineReceiver->receivePayload(correctCF->getFramePayload());

        assert(engineReceiver->getNextCFSequenceNumber() == 0);

        correctCF = CANFrame::createCANFrame(0x7E0, {0x20, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03});
        engineReceiver->receivePayload(correctCF->getFramePayload());

        assert(engineReceiver->getNextCFSequenceNumber() == 1);
    }
}