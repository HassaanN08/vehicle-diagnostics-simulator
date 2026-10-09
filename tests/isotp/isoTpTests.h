#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdint>
#include <thread>

#include "isotp/IsoTp.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"
#include "can/CANFrame.h"

inline void isoTpTests() {
    Vehicle vehicle {"Mercedez Benz"};
    const ECU* engine {vehicle.findEcuByRequestCanId(0x7E0)};
    const ECU* brake {vehicle.findEcuByRequestCanId(0x7E1)};
    const ECU* battery {vehicle.findEcuByRequestCanId(0x7E2)};

    {
        auto engineIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getRequestCANId(), engine->getResponseCANId()) };
        auto testerIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93};

        auto correctSingleFrame { CANFrame::createCANFrame(0x7E0, {0x07, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93}) };

        auto frame { testerIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame);
        assert(frame->getFramePayload().size() == correctSingleFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctSingleFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult result { engineIsoTpEndpoint->receiveFrame(*frame) };
        assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        std::vector<std::uint8_t> completedPayload { engineIsoTpEndpoint->getCompleteReassembledPayload() };

        assert(completedPayload == originalPayload);
    }

    {
        auto engineIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getRequestCANId(), engine->getResponseCANId()) };
        auto testerIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x00, 0x00 };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { testerIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame);
        assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult result { engineIsoTpEndpoint->receiveFrame(*frame) };
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame = engineIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == CtsPayload[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::CTS);
        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == correctCF1->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);

        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == correctCF2->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctCF2->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);

        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == correctCF3->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctCF3->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        std::vector<std::uint8_t> completedPayload { engineIsoTpEndpoint->getCompleteReassembledPayload() };

        assert(completedPayload == originalPayload);
    }

    {
        auto engineIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getRequestCANId(), engine->getResponseCANId(), 2) };
        auto testerIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId(), 2) };

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x02, 0x00 };

        auto incorrentFCFrame { CANFrame::createCANFrame(0x7E1, { 0x30, 0x02, 0x00 }) };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { testerIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame);
        assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult result { engineIsoTpEndpoint->receiveFrame(*frame) };
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame = engineIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == CtsPayload[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*incorrentFCFrame);
        assert(result == IsoTpReceiveFrameResult::InvalidFrameId);

        engineIsoTpEndpoint->confirmOutgoingFrameSent();
        testerIsoTpEndpoint->confirmOutgoingFrameSent();

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        assert(engineIsoTpEndpoint->checkTimeout() == IsoTpTimeoutResponse::RxTimedOut);
        assert(testerIsoTpEndpoint->checkTimeout() == IsoTpTimeoutResponse::TxTimedOut);

        frame = testerIsoTpEndpoint->sendPayload(originalPayload);
        assert(frame);
        assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame = engineIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == CtsPayload[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*incorrentFCFrame);
        assert(result == IsoTpReceiveFrameResult::InvalidFrameId);

        result = testerIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::CTS);
        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == correctCF1->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);

        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == correctCF2->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctCF2->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);

        frame = engineIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == CtsPayload[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::CTS);

        frame = testerIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == correctCF3->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctCF3->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame);
        assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        std::vector<std::uint8_t> completedPayload { engineIsoTpEndpoint->getCompleteReassembledPayload() };

        assert(completedPayload == originalPayload);
    }

    {
        auto brakeIsoTpEndpoint { IsoTp::createIsoTpEndpoint(brake->getRequestCANId(), brake->getResponseCANId(), 2) };
        auto testerIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId(), 2) };

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x02, 0x00 };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { testerIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame);
        assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult result { brakeIsoTpEndpoint->receiveFrame(*frame) };
        assert(result == IsoTpReceiveFrameResult::InvalidFrameId);
    }

    {
        auto engineIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getRequestCANId(), engine->getResponseCANId()) };
        auto testerIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x00, 0x00 };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame1 { testerIsoTpEndpoint->sendPayload(originalPayload) };
        auto frame2 { engineIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame1);
        assert(frame1->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame1->getFramePayload().size(); ++i) {
            assert(frame1->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }
        assert(frame2);
        assert(frame2->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame2->getFramePayload().size(); ++i) {
            assert(frame2->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult result { engineIsoTpEndpoint->receiveFrame(*frame1) };
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame1 = engineIsoTpEndpoint->getNextFrame();
        assert(frame1->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame1->getFramePayload().size(); ++i) {
            assert(frame1->getFramePayload()[i] == CtsPayload[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*frame2);
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame2 = testerIsoTpEndpoint->getNextFrame();
        assert(frame2->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame2->getFramePayload().size(); ++i) {
            assert(frame2->getFramePayload()[i] == CtsPayload[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*frame1);
        assert(result == IsoTpReceiveFrameResult::CTS);
        frame1 = testerIsoTpEndpoint->getNextFrame();
        assert(frame1->getFramePayload().size() == correctCF1->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame1->getFramePayload().size(); ++i) {
            assert(frame1->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame2);
        assert(result == IsoTpReceiveFrameResult::CTS);
        frame2 = engineIsoTpEndpoint->getNextFrame();
        assert(frame2->getFramePayload().size() == correctCF1->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame2->getFramePayload().size(); ++i) {
            assert(frame2->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame1);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);
        frame1 = testerIsoTpEndpoint->getNextFrame();
        assert(frame1->getFramePayload().size() == correctCF2->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame1->getFramePayload().size(); ++i) {
            assert(frame1->getFramePayload()[i] == correctCF2->getFramePayload()[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*frame2);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);
        frame2 = engineIsoTpEndpoint->getNextFrame();
        assert(frame2->getFramePayload().size() == correctCF2->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame2->getFramePayload().size(); ++i) {
            assert(frame2->getFramePayload()[i] == correctCF2->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame1);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);
        frame1 = testerIsoTpEndpoint->getNextFrame();
        assert(frame1->getFramePayload().size() == correctCF3->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame1->getFramePayload().size(); ++i) {
            assert(frame1->getFramePayload()[i] == correctCF3->getFramePayload()[i]);
        }

        result = testerIsoTpEndpoint->receiveFrame(*frame2);
        assert(result == IsoTpReceiveFrameResult::WaitingForNextCF);
        frame2 = engineIsoTpEndpoint->getNextFrame();
        assert(frame2->getFramePayload().size() == correctCF3->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame2->getFramePayload().size(); ++i) {
            assert(frame2->getFramePayload()[i] == correctCF3->getFramePayload()[i]);
        }

        result = engineIsoTpEndpoint->receiveFrame(*frame1);
        assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        result = testerIsoTpEndpoint->receiveFrame(*frame2);
        assert(result == IsoTpReceiveFrameResult::CompletedPayloadIsReady);

        std::vector<std::uint8_t> completedPayload1 { engineIsoTpEndpoint->getCompleteReassembledPayload() };
        std::vector<std::uint8_t> completedPayload2 { testerIsoTpEndpoint->getCompleteReassembledPayload() };

        assert(completedPayload1 == originalPayload);
        assert(completedPayload2 == originalPayload);
    }

    {
        auto engineIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getRequestCANId(), engine->getResponseCANId(), 2) };
        auto testerIsoTpEndpoint { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId(), 2) };

        std::vector<std::uint8_t> originalPayload {0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01, 0x07};
        std::vector<std::uint8_t> CtsPayload { 0x30, 0x02, 0x00 };

        auto correctFirstFrame { CANFrame::createCANFrame(0x7E0, {0x10, 0x15, 0x22, 0xF1, 0x90, 0xF1, 0x89, 0xF1}) };
        auto correctCF1 { CANFrame::createCANFrame(0x7E0, {0x21, 0x93, 0xF1, 0x87, 0x01, 0x02, 0x01, 0x03}) };
        auto correctCF2 { CANFrame::createCANFrame(0x7E0, {0x22, 0x01, 0x04, 0x01, 0x05, 0x01, 0x06, 0x01}) };
        auto correctCF3 { CANFrame::createCANFrame(0x7E0, {0x23, 0x07}) };

        auto frame { testerIsoTpEndpoint->sendPayload(originalPayload) };
        assert(frame);
        assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
        }

        IsoTpReceiveFrameResult result { engineIsoTpEndpoint->receiveFrame(*frame) };
        assert(result == IsoTpReceiveFrameResult::OutgoingCanFrameReady);
        frame = engineIsoTpEndpoint->getNextFrame();
        assert(frame->getFramePayload().size() == CtsPayload.size());

        for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
            assert(frame->getFramePayload()[i] == CtsPayload[i]);
        }

        engineIsoTpEndpoint->confirmOutgoingFrameSent();
        testerIsoTpEndpoint->confirmOutgoingFrameSent();

        std::this_thread::sleep_for(std::chrono::milliseconds(1100));

        assert(engineIsoTpEndpoint->checkTimeout() == IsoTpTimeoutResponse::RxTimedOut);
    }
}