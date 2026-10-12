#pragma once

#include "can/CANFrame.h"
#include "isotp/IsoTp.h"
#include "isotp/Sender.h"
#include "uds/UDSServer.h"
#include "domain/ECU.h"

#include <vector>
#include <cstdint>
#include <optional>

enum class DiagnosticCoordinatorResult {
    Processed,
    Waiting,
    ResponseOutstanding,
    Error,
};

enum class DiagnosticCoordinatorState {
    ResponseOutstanding,
    NoResponseOutstanding,
};

class DiagnosticCoordinator {
    ECU* m_ecu;
    IsoTp m_isoTpEndpoint;
    DiagnosticCoordinatorState m_currentState { DiagnosticCoordinatorState::NoResponseOutstanding };
    std::vector<std::uint8_t> m_responsePayload;

    DiagnosticCoordinator(ECU* ecu, IsoTp isoTp)
        : m_ecu { ecu }
        , m_isoTpEndpoint { isoTp } {}



    public:
        static inline std::optional<DiagnosticCoordinator> createDiagnosticCoordinator(ECU* ecu, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0) {
            if (!ecu) return std::nullopt;; 
            auto isoTp { IsoTp::createIsoTpEndpoint(ecu->getRequestCANId(), ecu->getResponseCANId(), blockSize, STmin) };
            if (isoTp.has_value()) {
                return DiagnosticCoordinator(ecu, *isoTp);
            } else return std::nullopt;
        }

        DiagnosticCoordinatorResult coordinate(const CANFrame& frame) {
            IsoTpReceiveFrameResult result { m_isoTpEndpoint.receiveFrame(frame) };

            if (result == IsoTpReceiveFrameResult::Error || result == IsoTpReceiveFrameResult::InvalidFrameId)
                return DiagnosticCoordinatorResult::Error;

            if (result == IsoTpReceiveFrameResult::WaitingForNextCF || result == IsoTpReceiveFrameResult::NothingYet)
                return DiagnosticCoordinatorResult::Waiting;
            
            if (result == IsoTpReceiveFrameResult::CompletedPayloadIsReady) {
                if (m_currentState == DiagnosticCoordinatorState::NoResponseOutstanding){
                    m_responsePayload = UDSServer::server(*m_ecu, m_isoTpEndpoint.getCompleteReassembledPayload());
                    m_currentState = DiagnosticCoordinatorState::ResponseOutstanding;
                } else {
                    return DiagnosticCoordinatorResult::ResponseOutstanding;
                }
            }

            return DiagnosticCoordinatorResult::Processed;
        }

        std::optional<CANFrame> getOutgoingFrame() {
            if (!m_responsePayload.empty()) {
                auto frame { m_isoTpEndpoint.sendPayload(m_responsePayload) };

                if (frame.has_value())
                    m_responsePayload.clear();

                return frame;
            } else 
                return m_isoTpEndpoint.getNextFrame();
        }

        IsoTpTimeoutResponse checkTimeout() {
            return m_isoTpEndpoint.checkTimeout();
        }

        void confirmOutgoingFrameSent() { 
            if (m_isoTpEndpoint.getAwaitingFrame() == FrameAwaitingAcknowledgement::FinalFrame ||
            m_isoTpEndpoint.getAwaitingFrame() == FrameAwaitingAcknowledgement::SingleFrame) {
                m_currentState = DiagnosticCoordinatorState::NoResponseOutstanding;
            }
            
            m_isoTpEndpoint.confirmOutgoingFrameSent();
        }
};
