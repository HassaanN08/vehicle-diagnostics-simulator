#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>

#include "isotp/Sender.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "can/CANFrame.h"

inline void senderTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    const ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    const ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    {
        Sender engineSender { engine->getRequestCANId() };
        Sender brakeSender { brake->getRequestCANId()};
        Sender batterySender { battery->getRequestCANId()};

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { engineSender.receivePayload({}) };
        assert(!frame);

        frame = engineSender.receivePayload(originalPayload);
        assert(frame);
        assert(frame->getFramePayload() == correctFirstFrame->getFramePayload());
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        engineSender.receiveFC(*CANFrame::createCANFrame(0x7E0, {0x30, 0x00, 0xF1}));
        assert(engineSender.getCurrentState() == SenderState::ReadyToSendCF);
        assert(engineSender.getCurrentBlockSize() == 0x00);
        assert(engineSender.getCurrentSTmin() == 0xF1);
        assert(engineSender.getCurrentOffset() == 6);

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::ReadyToSendCF);
        assert(frame);
        assert(frame->getFramePayload() == correctCF1->getFramePayload());
        assert(engineSender.getCurrentOffset() == 13);

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::ReadyToSendCF);
        assert(frame);
        assert(frame->getFramePayload() == correctCF2->getFramePayload());
        assert(engineSender.getCurrentOffset() == 20);

        frame = engineSender.receivePayload(originalPayload);
        assert(!frame);

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::Idle);
        assert(frame);
        assert(frame->getFramePayload() == correctCF3->getFramePayload());
        assert(engineSender.getCurrentOffset() == 0);
    }

    {
        Sender engineSender { engine->getRequestCANId() };
        Sender brakeSender { brake->getRequestCANId()};
        Sender batterySender { battery->getRequestCANId()};

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { engineSender.receivePayload({}) };
        assert(!frame);

        frame = engineSender.receivePayload(originalPayload);
        assert(frame);
        assert(frame->getFramePayload() == correctFirstFrame->getFramePayload());
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        engineSender.receiveFC(*CANFrame::createCANFrame(0x7E0, {0x30, 0x02, 0xF1}));

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::ReadyToSendCF);
        assert(frame);
        assert(frame->getFramePayload() == correctCF1->getFramePayload());

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        assert(frame);
        assert(frame->getFramePayload() == correctCF2->getFramePayload());

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        assert(!frame);

        engineSender.receiveFC(*CANFrame::createCANFrame(0x7E0, {0x31, 0x00, 0x00}));
        assert(engineSender.getCurrentOffset() == 20);

        engineSender.receiveFC(*CANFrame::createCANFrame(0x7E0, {0x30, 0x00, 0xF1}));
        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::Idle);
        assert(frame);
        assert(frame->getFramePayload() == correctCF3->getFramePayload());
        assert(engineSender.getCurrentOffset() == 0);
    }

    {
        Sender engineSender { engine->getRequestCANId() };
        Sender brakeSender { brake->getRequestCANId()};
        Sender batterySender { battery->getRequestCANId()};

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { engineSender.receivePayload({}) };
        assert(!frame);

        frame = engineSender.receivePayload(originalPayload);
        assert(frame);
        assert(frame->getFramePayload() == correctFirstFrame->getFramePayload());
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        engineSender.receiveFC(*CANFrame::createCANFrame(0x7E0, {0x30, 0x02, 0xF1}));

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::ReadyToSendCF);
        assert(frame);
        assert(frame->getFramePayload() == correctCF1->getFramePayload());

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        assert(frame);
        assert(frame->getFramePayload() == correctCF2->getFramePayload());

        frame = engineSender.getNextCF();
        assert(engineSender.getCurrentState() == SenderState::WaitingForFlowControl);
        assert(!frame);

        engineSender.receiveFC(*CANFrame::createCANFrame(0x7E0, {0x32, 0x00, 0x00}));
        assert(engineSender.getCurrentOffset() == 0);
    }

    {
        Sender engineSender { engine->getRequestCANId() };
        Sender brakeSender { brake->getRequestCANId()};
        Sender batterySender { battery->getRequestCANId()};

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93};

        auto correctSingleFrame { CANFrame::createCANFrame(0x7E0, {0x07, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93}) };

        auto frame { engineSender.receivePayload({}) };
        assert(!frame);
        
        frame = engineSender.receivePayload(originalPayload);
        assert(frame);
        assert(frame->getFramePayload() == correctSingleFrame->getFramePayload());
        assert(engineSender.getCurrentState() == SenderState::Idle);

        frame = engineSender.receivePayload(originalPayload);
        assert(frame);
        assert(frame->getFramePayload() == correctSingleFrame->getFramePayload());
        assert(engineSender.getCurrentState() == SenderState::Idle);
    }
}