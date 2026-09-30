#include <app/DiagnosticCoordinator.h>
#include <domain/Vehicle.h>
#include <domain/ECU.h>

#include <unordered_map>
#include <cstdint>
#include <vector>
#include <utility>

enum class GetCANFrameResult {
    FrameRouted,
    CoordinatorNotFound,
};

class DiagnosticRuntime {
    std::unordered_map<std::uint32_t, DiagnosticCoordinator> m_diagnosticCoordinators {};

    Vehicle& m_vehicle;

    public:
        DiagnosticRuntime(Vehicle& vehicle)
            : m_vehicle { vehicle } {
                for (ECU& ecu : vehicle.m_ecuList) {
                    auto coordinator { DiagnosticCoordinator::createDiagnosticCoordinator(&ecu) };
                    if (coordinator.has_value())
                        m_diagnosticCoordinators.emplace(ecu.getRequestCANId(), std::move(coordinator));
                    else continue;
                }
            }
        
        GetCANFrameResult receiveCANFrame(const CANFrame& frame) {
            std::uint32_t frameId { frame.getFrameId() };
            
            if (m_diagnosticCoordinators.find(frameId) == m_diagnosticCoordinators.end()) {
                return GetCANFrameResult::CoordinatorNotFound;
            }

            m_diagnosticCoordinators[frameId].coordinate(frame);
            return GetCANFrameResult::FrameRouted;
        }

        
};