#pragma once
#include <string>
#include <vector>
#include "ECU.h"
#include "DTC.h"
#include "DiagnosticLog.h"
#include "CANBus.h"
#include "CANFrame.h"
#include "CANTrafficReport.h"

enum class ECUStatus : int;
enum class ClearFaultResult : int;
enum class ECUStatusResult : int;
enum class VehicleSession : int;
enum class DiagnosticSessionResult : int;

class Vehicle {
    std::string name;
    std::vector<ECU> units;
    DiagnosticLog logs;
    ECU* searchECUByName(const std::string& ecuName);
    VehicleSession currentSession;
    CANBus bus;

    public:
        Vehicle(const std::string& name);

        std::string getName() const;

        void displayLogs() const;

        bool addECU(const ECU& ecu, const bool log);

        void addDTCToECU(const std::string& ecuName, const DTC& dtc);

        ECUStatusResult setECUStatusByName(const std::string ecuName, const ECUStatus& state);

        void displayAllECUs() const;

        void scanVehicle();

        bool displayECUFaultHistory(const std::string& ecuName);

        bool hasActiveFaults() const;

        bool doesECUExist(const std::string& ecuName);

        DiagnosticSessionResult setDiagnosticSession(const VehicleSession& session);

        VehicleSession getDiagnosticSession() const;

        ClearFaultResult clearFaultsFromECU(const std::string& ecuName);

        void transmitCANFrame(const CANFrame& frame);

        void displayCANBusTraffic() const;

        bool CANTrafficExists() const;

        size_t CANTrafficCount() const;

        void displayDecodedCANFrames() const;

        std::string analyzeCANBusTrafficReport() const;
};