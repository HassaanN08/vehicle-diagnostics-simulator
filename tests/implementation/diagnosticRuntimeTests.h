#pragma once

#include <cassert>
#include <vector>
#include <iostream>

#include "app/DiagnosticRuntime.h"
#include "domain/Vehicle.h"
#include "domain/ECU.h"
#include "can/CANFrame.h"

void diagnosticRuntimeTests() {
    Vehicle vehicle { "Mercedez G-Wagon" };
    DiagnosticRuntime runtime { vehicle };
    ECU* engine { vehicle.findEcuByRequestCanId(0x7E0) };
    ECU* brake { vehicle.findEcuByRequestCanId(0x7E1) };
    ECU* battery { vehicle.findEcuByRequestCanId(0x7E2) };

    auto engineTesterIsoTp { IsoTp::createIsoTpEndpoint(engine->getResponseCANId(), engine->getRequestCANId()) };
    auto brakeTesterIsoTp { IsoTp::createIsoTpEndpoint(brake->getResponseCANId(), brake->getRequestCANId()) };
    auto batteryTesterIsoTp { IsoTp::createIsoTpEndpoint(battery->getResponseCANId(), battery->getRequestCANId()) };
    assert(engineTesterIsoTp && brakeTesterIsoTp && batteryTesterIsoTp);

    engine->addDtc(*DTC::createDTC(0x120300));
    engine->addDtc(*DTC::createDTC(0x340171));
    engine->addDtc(*DTC::createDTC(0x560F11));
    assert(engine->setDTCStatus(0x120300, 0x08) == DtcResult::dtcStatusSet);
    assert(engine->setDTCStatus(0x340171, 0x08) == DtcResult::dtcStatusSet);
    assert(engine->setDTCStatus(0x560F11, 0x01) == DtcResult::dtcStatusSet);

    std::vector<std::uint8_t> ctsPayload { 0x30, 0x00, 0x00 };
    auto correctFirstFrame { CANFrame::createCANFrame(0x7E8, {0x10, 0x0B, 0x59, 0x02, 0x09, 0x12, 0x03, 0x00}) };
    auto correctCF1 { CANFrame::createCANFrame(0x7E8, {0x21, 0x08, 0x34, 0x01, 0x71, 0x08}) };

    auto frame { CANFrame::createCANFrame(0x7E9, {0x00, 0x01, 0x02, 0x03}) };
    assert(frame.has_value());
    assert(runtime.receiveCANFrame(*frame) == DiagnosticRuntimeResult::CoordinatorNotFound);

    frame = engineTesterIsoTp->sendPayload({0x19, 0x02, 0x08});
    assert(frame.has_value());
    
    DiagnosticRuntimeResult result { runtime.receiveCANFrame(*frame) };
    assert(result == DiagnosticRuntimeResult::FrameRouted);

    frame = runtime.getOutgoingFrame();
    assert(frame.has_value());
    assert(frame->getFramePayload().size() == correctFirstFrame->getFramePayload().size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == correctFirstFrame->getFramePayload()[i]);
    }

    IsoTpReceiveFrameResult isoTpResult { engineTesterIsoTp->receiveFrame(*frame) };

    assert(isoTpResult == IsoTpReceiveFrameResult::OutgoingCanFrameReady);

    frame = engineTesterIsoTp->getNextFrame();
    assert(frame);
    assert(frame->getFramePayload().size() == ctsPayload.size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == ctsPayload[i]);
    }

    result = runtime.receiveCANFrame(*frame);
    assert(result == DiagnosticRuntimeResult::FrameRouted);

    frame = runtime.getOutgoingFrame();
    assert(frame.has_value());
    assert(frame->getFramePayload().size() == correctCF1->getFramePayload().size());

    for (std::size_t i { 0 }; i < frame->getFramePayload().size(); ++i) {
        assert(frame->getFramePayload()[i] == correctCF1->getFramePayload()[i]);
    }

    frame = runtime.getOutgoingFrame();
    assert(!frame);

    //Test wrong ISO-TP metadata
    frame = CANFrame::createCANFrame(0x7E0, {0x03, 0x10, 0x01});

    result = runtime.receiveCANFrame(*frame);
    assert(result == DiagnosticRuntimeResult::ProcessingError);

    auto returnedFrame { runtime.getOutgoingFrame() };

    assert(!returnedFrame.has_value());

    assert(battery->getCurrentDiagnosticSession() == DiagnosticSession::Default);
}