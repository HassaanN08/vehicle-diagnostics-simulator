#pragma once

#include "app/DiagnosticCoordinator.h"
#include "domain/Vehicle.h"
#include "domain/ECU.h"
#include "can/CANFrame.h"

#include <unordered_map>
#include <cstdint>
#include <vector>
#include <utility>
#include <optional>

enum class DiagnosticRuntimeResult {
    FrameRouted,
    ProcessingError,
    CoordinatorNotFound,
};

class DiagnosticRuntime {
    std::unordered_map<std::uint32_t, DiagnosticCoordinator> m_diagnosticCoordinators {};

    DiagnosticCoordinator* m_currentCoordinator { nullptr };

    Vehicle& m_vehicle;

    public:
        DiagnosticRuntime(Vehicle& vehicle)
            : m_vehicle { vehicle } {
                for (ECU& ecu : vehicle.m_ecuList) {
                    auto coordinator { DiagnosticCoordinator::createDiagnosticCoordinator(&ecu) };
                    if (coordinator.has_value())
                        m_diagnosticCoordinators.emplace(ecu.getRequestCANId(), std::move(*coordinator));
                    else continue;
                }
            }
        
        DiagnosticRuntimeResult receiveCANFrame(const CANFrame& frame) {
            auto coordinator { m_diagnosticCoordinators.find(frame.getFrameId()) };
            if (coordinator == m_diagnosticCoordinators.end()) {
                return DiagnosticRuntimeResult::CoordinatorNotFound;
            }

            DiagnosticCoordinatorResult result { coordinator->second.coordinate(frame) };
            if (result == DiagnosticCoordinatorResult::Error)
                return DiagnosticRuntimeResult::ProcessingError;
            else
                return DiagnosticRuntimeResult::FrameRouted;
        }

        std::optional<CANFrame> getOutgoingFrame() {
            for (auto& [CANId, coordinator] : m_diagnosticCoordinators) {
                auto frame { coordinator.getOutgoingFrame() };
                if (frame.has_value()) {
                    m_currentCoordinator = &coordinator;
                    return *frame;
                }
            }

            return std::nullopt;
        }

        void checkIsoTpTimeout() {
            for (auto& [CANId, coordinator] : m_diagnosticCoordinators) {
                coordinator.checkTimeout();
            }
        }

        void confirmOutgoingFrameSent() {
            if (m_currentCoordinator != nullptr) {
                m_currentCoordinator->confirmOutgoingFrameSent();
                m_currentCoordinator = nullptr;
            }
        }
};