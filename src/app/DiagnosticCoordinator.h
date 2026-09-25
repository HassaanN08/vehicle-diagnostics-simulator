#pragma once

#include "can/CANFrame.h"
#include "isotp/IsoTp.h"
#include "uds/UDSServer.h"
#include "domain/ECU.h"
#include "domain/Vehicle.h"

#include <vector>
#include <cstdint>
#include <optional>

class DiagnosticCoordinator {
    ECU* m_ecu;
    IsoTp m_isoTpEndpoint;
    std::vector<std::uint8_t> m_responsePayload;

    DiagnosticCoordinator(ECU* ecu, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0)
        : m_ecu { ecu }
        , m_isoTpEndpoint { *IsoTp::createIsoTpEndpoint(ecu->getRequestCANId(), ecu->getResponseCANId(), blockSize, STmin) } {}



    public:
        static inline std::optional<DiagnosticCoordinator> createDiagnosticCoordinator(ECU* ecu, const std::uint8_t blockSize = 0, const std::uint8_t STmin = 0) {
            auto isoTp { IsoTp::createIsoTpEndpoint(ecu->getRequestCANId(), ecu->getResponseCANId(), blockSize, STmin) };
            if (isoTp.has_value()) {
                return DiagnosticCoordinator(ecu, blockSize, STmin);
            } else return std::nullopt;
        }

        IsoTpReceiveFrameResult coordinate(const CANFrame& frame) {
            if (frame.getFrameId() != m_ecu->getRequestCANId()) return IsoTpReceiveFrameResult::InvalidFrameId;

            IsoTpReceiveFrameResult result { m_isoTpEndpoint.receiveFrame(frame) };
            
            if (result == IsoTpReceiveFrameResult::CompletedPayloadIsReady) {
                m_responsePayload = UDSServer::server(*m_ecu, m_isoTpEndpoint.getCompleteReassembledPayload());
            }

            return result;
        }

        std::optional<CANFrame> getResponse() {
            if (m_responsePayload.empty()) {
                return  std::nullopt;
            }

            auto frame { m_isoTpEndpoint.sendPayload(m_responsePayload) };
            m_responsePayload.clear();

            return frame;
        }

        std::optional<CANFrame> getNextFrame()  {
            if (m_responsePayload.empty())
                return m_isoTpEndpoint.getNextFrame();
            else return std::nullopt;
        }

        IsoTpTimeoutResponse checkTimeout() {
            return m_isoTpEndpoint.checkTimeout();
        }
};
